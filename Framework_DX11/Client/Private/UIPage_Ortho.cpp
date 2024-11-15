#include "stdafx.h"
#include "..\Public\UIPage_Ortho.h"

#include "GameInstance.h"
#include "UIPage_Ortho_Include.h"

CUIPage_Ortho::CUIPage_Ortho(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Ortho::CUIPage_Ortho(const CUIPage_Ortho& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Ortho::Initialize_Prototype()
{
	// 모든 UIPage는 클론 안 함, 여기서 모든 세팅 끝내기 

	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_vecPageAction.resize(_int(PAGEACTION::ACTION_END));

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	Initialize_Ortho_Info();

	m_bRender = false;

	return S_OK;
}

HRESULT CUIPage_Ortho::Initialize(void* pArg)
{
	

	return S_OK;
}

void CUIPage_Ortho::Priority_Update(_float fTimeDelta)
{
	// 임시 코드 
	// 직교 UI가 아직 안전하지 않음 
	// 비활성화 상태로 git에 올려 두기 
	return;

	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Ortho::Update(_float fTimeDelta)
{
	// 임시 코드 
	// 직교 UI가 아직 안전하지 않음 
	// 비활성화 상태로 git에 올려 두기 
	return;

	__super::Update(fTimeDelta);
}

void CUIPage_Ortho::Late_Update(_float fTimeDelta)
{
	// 임시 코드 
	// 직교 UI가 아직 안전하지 않음 
	// 비활성화 상태로 git에 올려 두기 
	return;


	CheckHost(fTimeDelta);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Ortho::Render()
{
	return S_OK;
}

void CUIPage_Ortho::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Ortho::CloseAction()
{
	__super::CloseAction();
}

HRESULT CUIPage_Ortho::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	for (_int i = 0; i < _int(PART_GROUP::GROUP_END); ++i)
	{
		UG_CTRL* pNew = new UG_CTRL;
		m_Ctrllist.push_back(pNew);
		m_vecOrtho_Group_Ctrl.push_back(pNew);
	}

	return S_OK;
}

void CUIPage_Ortho::Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE eType, void* pObj)
{
	ORTHO_HOST* pNew = new ORTHO_HOST;
	pNew->eType = eType;
	pNew->pHost = static_cast<CGameObject*>(pObj);
	m_Ortho_Host_list.push_back(pNew);
}

void CUIPage_Ortho::Initialize_Ortho_Info()
{
	// 직교 UI 별 보정치 세팅 
	m_vecOrtho_Adjust.resize(_int(PART_GROUP::GROUP_END)); // <- 파트 그룹을 기준으로 정한다 (부모 인덱스가 -1인 것 기준)
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,-1.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,0.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,0.f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,0.f,0.f }; // 몬스터 특수공격
}

void CUIPage_Ortho::CheckHost(_float fTimeDelta)
{
	for (list<CUIPage_Ortho::OR_HOST*>::iterator iter = m_Ortho_Host_list.begin(); iter!= m_Ortho_Host_list.end();)
	{
		// Host 정보 제거 논리 
		/*
		1. host가 nullptr이거나 dead이다 
		
		
		*/

		if (((*iter)->pHost == nullptr) || ((*iter)->pHost->Get_Dead()))
		{
			// iter->pHost는 얕은 복사로 가져왔으며 addref 하지 않았음
			Safe_Delete(*iter);
			iter = m_Ortho_Host_list.erase(iter);
		}
		else
		{
			CheckHost_By_ObjType((*iter)->pHost, (*iter)->eType, fTimeDelta);
			++iter;
		}
	}
}

void CUIPage_Ortho::CheckHost_By_ObjType(CGameObject* pHost, UI_ORTHO_OBJ_TYPE eType, _float fTimeDelta)
{
	// 일반 몬스터
	if (eType == UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER)
	{
		// 직교 UI를 ON/OFF 하는 논리 
		// 각 직교 UI 별 운영 논리 


		// HP 바
		_float fHP_Ratio = 0.5f; // <- 테스트 수치임 추후 연결 필요

		Make_Ortho_UI(pHost, PART_GROUP::GROUP_HP_FRAME, fTimeDelta, fHP_Ratio);

		// 포커싱
		Make_Ortho_UI(pHost, PART_GROUP::GROUP_FOCUS, fTimeDelta);

		// 특수공격
		Make_Ortho_UI(pHost, PART_GROUP::GROUP_SPECIAL_HIT, fTimeDelta);
	}
}

void CUIPage_Ortho::Make_Ortho_UI(CGameObject* pHost, PART_GROUP eGroup, _float fTimeDelta, _float fRatio)
{
	_float2 fPos = { 0.f,0.f };
	_float fDistance = 0.f;
	if (!Make_OrthoGraphy_Position(pHost, eGroup, &fPos, &fDistance))
		return;

	m_vecPart[m_vecOrtho_Group_Ctrl[_int(eGroup)]->PartIndexlist.front()]->fAdjust = fPos;

	if (eGroup == PART_GROUP::GROUP_HP_FRAME)
	{
		m_vecPart[m_vecOrtho_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FILL)]->PartIndexlist.front()]->fRatio = fRatio;
		__super::UpdatePart_ByIndex(m_vecOrtho_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FILL)]->PartIndexlist.front(), fTimeDelta);
		OR_RENDER* pNew = new OR_RENDER(fDistance, m_vecPart[m_vecOrtho_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FILL)]->PartIndexlist.front()]);
	}

	for (auto& iter : m_vecOrtho_Group_Ctrl[_int(eGroup)]->PartIndexlist)
	{
		__super::UpdatePart_ByIndex(iter, fTimeDelta);
		OR_RENDER* pNew = new OR_RENDER(fDistance, m_vecPart[iter]);
	}
}

_bool CUIPage_Ortho::Make_OrthoGraphy_Position(CGameObject* pHost, PART_GROUP eGroup, _float2* fPosition, _float* fDistance_Cam)
{
	const _matrix mMat = pHost->Get_Transform()->Get_WorldMatrix();
	const _matrix mProj = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
	const _matrix mView = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	_float3 fAdj = m_vecOrtho_Adjust[_int(eGroup)];
	_vector vResult = { fAdj.x, fAdj.y, fAdj.z, 0.f };

	// 투영 좌표 계산
	vResult = XMVector3Transform(vResult, mMat);
	vResult = XMVector3Transform(vResult, mView);
	vResult = XMVector3Transform(vResult, mProj);

	// W나누기
	_float4 fResult{};
	XMStoreFloat4(&fResult, vResult);

	if (fResult.w < 0.f)
		return false;

	
	_vector vDistance = pHost->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pGameInstance->Get_CamPosition_Vec4();

	*fDistance_Cam = sqrt(pow(vDistance.m128_f32[0], 2) + pow(vDistance.m128_f32[1], 2) + pow(vDistance.m128_f32[2], 2));
	fPosition->x = fResult.x / fResult.w;
	fPosition->y = fResult.y / fResult.w;

	return true;
}


CUIPage_Ortho* CUIPage_Ortho::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Ortho* pInstance = new CUIPage_Ortho(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Ortho"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Ortho::Clone(void* pArg)
{
	CUIPage_Ortho* pInstance = new CUIPage_Ortho(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Ortho"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Ortho::Free()
{
	__super::Free();

	for (auto& iter : m_Ortho_Host_list)
		Safe_Delete(iter);

	m_Ortho_Host_list.clear();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
	m_vecOrtho_Adjust.clear();
}
