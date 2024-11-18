#include "stdafx.h"
#include "..\Public\UIPage_Ortho.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"
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
	//return;

	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Ortho::Update(_float fTimeDelta)
{
	// 임시 코드 
	// 직교 UI가 아직 안전하지 않음 
	// 비활성화 상태로 git에 올려 두기 
	//return;

	__super::Update(fTimeDelta);
}

void CUIPage_Ortho::Late_Update(_float fTimeDelta)
{
	// 임시 코드 
	// 직교 UI가 아직 안전하지 않음 
	// 비활성화 상태로 git에 올려 두기 
	//return;

	m_fTimeDelta = fTimeDelta;
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

CHECK_MOUSE CUIPage_Ortho::Check_Mouse_By_Part_In_Page()
{
	__super::Check_Mouse_By_Part_In_Page();

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Ortho::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	for (_int i = 0; i < _int(PART_GROUP::GROUP_END); ++i)
	{
		UG_CTRL* pNew = new UG_CTRL;
		m_vec_Group_Ctrl.push_back(pNew);
	}

	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		_int iGroup = m_vecPart[i]->iGroupIndex;

		if ((iGroup >= 0) && (iGroup < _int(PART_GROUP::GROUP_END)))
			m_vec_Group_Ctrl[iGroup]->PartIndexlist.push_back(i);
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

HRESULT CUIPage_Ortho::Render_Ortho_UI(CUIRender_Client* pRender_Client)
{
	// 여기서 렌더 해야 함
	
	while (!m_queue_Ortho_Render_Ctrl.empty())
	{
		OR_RENDER* pRender = m_queue_Ortho_Render_Ctrl.top();
		m_queue_Ortho_Render_Ctrl.pop();
		PART_GROUP eType = pRender->eType;
		m_vec_Group_Ctrl[_int(eType)]->fRatio = pRender->fRatio;
		m_fX = pRender->fPosition.x;
		m_fY = pRender->fPosition.y;

		if (eType == PART_GROUP::GROUP_HP_COUNT)
			m_vecPart[m_vec_Group_Ctrl[_int(eType)]->PartIndexlist.front()]->strText = pRender->strText;

		if (eType == PART_GROUP::GROUP_HP_FILL)
			m_vecPart[m_vec_Group_Ctrl[_int(eType)]->PartIndexlist.front()]->fRatio = pRender->fRatio;

		for (auto& iter : m_vec_Group_Ctrl[_int(eType)]->PartIndexlist)
		{
			if ((eType == PART_GROUP::GROUP_HP_COUNT) || (eType == PART_GROUP::GROUP_HP_FILL))
				m_vecPart[0]->fPosition = { m_fX, m_fY };
		

			__super::UpdatePart_ByIndex(iter, m_fTimeDelta);

			

			pRender_Client->Render_Part(*m_vecPart[iter], *this, false);
		}

		Safe_Delete(pRender);
	}

	return S_OK;
}

void CUIPage_Ortho::Initialize_Ortho_Info()
{
	// 직교 UI 별 보정치 세팅 
	m_vecOrtho_Adjust.resize(_int(PART_GROUP::GROUP_END)); // <- 파트 그룹을 기준으로 정한다 (부모 인덱스가 -1인 것 기준)
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,3.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,3.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,3.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.f,0.f }; // 몬스터 특수공격
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
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace);
			++iter;
		}
	}
}

void CUIPage_Ortho::Make_Monster_HP_Bar(CGameObject* pHost, _float fTimeDelta, _float fDistance)
{
	// 여기에 HP 바를 띄우는 논리가 필요함 

	// 테스트 수치 
	_float fRatio = GET_GAMEINTERFACE->GetTestData()->fHP_Now / GET_GAMEINTERFACE->GetTestData()->fHP_Max;
	_float fDamege = GET_GAMEINTERFACE->GetTestData()->fHP_Damege;

	_float2 fPosition = { 0.f,0.f };
	if (!Make_OrthoGraphy_Position(pHost, PART_GROUP::GROUP_HP_FRAME, &fPosition))
		return;

	OR_RENDER* pRender_HP_Frame = new OR_RENDER;
	*pRender_HP_Frame = { fDistance ,fPosition,  PART_GROUP::GROUP_HP_FRAME, fRatio, {}, -1 };
	OR_RENDER* pRender_HP_Fill = new OR_RENDER;
	*pRender_HP_Fill = { fDistance ,fPosition,  PART_GROUP::GROUP_HP_FILL, fRatio, {}, -1 };
	OR_RENDER* pRender_HP_Demege = new OR_RENDER;
	*pRender_HP_Demege = { fDistance ,fPosition,  PART_GROUP::GROUP_HP_COUNT, fRatio, to_wstring(_int(fDamege)), -1};

	m_queue_Ortho_Render_Ctrl.push(pRender_HP_Frame);
	m_queue_Ortho_Render_Ctrl.push(pRender_HP_Fill);
	m_queue_Ortho_Render_Ctrl.push(pRender_HP_Demege);
}

void CUIPage_Ortho::Make_Monster_Focusing(CGameObject* pHost, _float fTimeDelta, _float fDistance)
{
	// 여기에 고정 대상 띄우는 논리가 필요함 

		// 테스트 수치 
	if (!GET_GAMEINTERFACE->GetTestData()->bFocus)
		return;

	_float2 fPosition = { 0.f,0.f };
	if (!Make_OrthoGraphy_Position(pHost, PART_GROUP::GROUP_FOCUS, &fPosition))
		return;

	OR_RENDER* pRender_Focusing = new OR_RENDER;
	*pRender_Focusing = { fDistance ,fPosition,  PART_GROUP::GROUP_FOCUS,1.f, {}, -1 };

	m_queue_Ortho_Render_Ctrl.push(pRender_Focusing);
}

void CUIPage_Ortho::Make_Monster_SpecialHit(CGameObject* pHost, _float fTimeDelta, _float fDistance)
{
	// 여기에 특수 공격 가능 상태 띄우는 논리가 필요함 

	// 테스트 수치 
	if (!GET_GAMEINTERFACE->GetTestData()->bSpecial_Attack)
		return;

	_float2 fPosition = { 0.f,0.f };
	if (!Make_OrthoGraphy_Position(pHost, PART_GROUP::GROUP_SPECIAL_HIT, &fPosition))
		return;

	OR_RENDER* pRender_Special = new OR_RENDER;
	*pRender_Special = { fDistance ,fPosition,  PART_GROUP::GROUP_SPECIAL_HIT,1.f, {}, -1 };

	m_queue_Ortho_Render_Ctrl.push(pRender_Special);
}


_bool CUIPage_Ortho::Make_OrthoGraphy_Position(CGameObject* pHost, PART_GROUP eGroup, _float2* fPosition)
{
	const _matrix mMat = pHost->Get_Transform()->Get_WorldMatrix();
	const _matrix mProj = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
	const _matrix mView = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	const _vector vSize = pHost->Get_Transform()->Get_Scaled();

	_float3 fAdj = m_vecOrtho_Adjust[_int(eGroup)];
	fAdj.x *= vSize.m128_f32[0];
	fAdj.y *= vSize.m128_f32[1];
	fAdj.z *= vSize.m128_f32[2];

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

	fPosition->x = fResult.x / fResult.w;
	fPosition->y = fResult.y / fResult.w;

	// 스크린 좌표로 변환
	fPosition->x = ((fPosition->x + 1.f) * 0.5) * 1280.f;
	fPosition->y = ((1.f - fPosition->y) * 0.5) * 720.f;

	return true;
}

_float CUIPage_Ortho::Check_Distance_From_Cam(CGameObject* pHost)
{
	_vector vDistance = pHost->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pGameInstance->Get_CamPosition_Vec4();
	return sqrt(pow(vDistance.m128_f32[0], 2) + pow(vDistance.m128_f32[1], 2) + pow(vDistance.m128_f32[2], 2));
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

	while (!m_queue_Ortho_Render_Ctrl.empty())
	{
		m_queue_Ortho_Render_Ctrl.pop();
	}
	
}
