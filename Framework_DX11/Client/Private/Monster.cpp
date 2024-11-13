#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"
#include "Player.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};
	if (pArg != nullptr)
	{
		CGameObject::GAMEOBJECT_DESC* pDesc = static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);
		memcpy(&Desc, pDesc, sizeof(CGameObject::GAMEOBJECT_DESC));
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;



	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CMonster::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

#ifdef _DEBUG
	for (auto& pCollider : m_pColliderCom)
		pCollider->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
#endif
}

void CMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	/*CComponent*		pTargetCollider = m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"));
	if (nullptr == pTargetCollider)
		return;*/

	//CComponent* pTargetCollider = m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0, CPlayer::PART_WEAPON);
	//if (nullptr == pTargetCollider)
	//	return;
	//
	//for (size_t i = 0; i < TYPE_END; i++)
	//{
	//	m_pColliderCom[i]->Intersect(dynamic_cast<CCollider*>(pTargetCollider));
	//}

		//if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		//m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
		//for (auto& pCollider : m_pColliderCom)
		//	m_pGameInstance->Add_DebugObject(pCollider);
#endif
	}
}

HRESULT CMonster::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", DIFFUSE, (_uint)i)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}



	return S_OK;
}

void CMonster::Change_State(const _uint iState, void* pArg)
{
	m_pFSMCom->Change_State(iState, pArg);
}

void CMonster::Change_Animation(_uint iAnimIndex, _bool IsLoop, _bool bSetup)
{
	if (bSetup)
	{
		m_pModelCom->SetUp_Animation(iAnimIndex, IsLoop);
	}
	else
	{
		m_pModelCom->SetUp_NextAnimation(iAnimIndex, IsLoop);
	}
}

void CMonster::Look_Player()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = m_pGameInstance->Get_CamPosition_Vec4();//임시사용 캠 포지션
		
	if (!(XMVectorGetX(vPos) == XMVectorGetX(vCamPos))
		&& !(XMVectorGetZ(vPos) == XMVectorGetZ(vCamPos)))
	{
		m_pTransformCom->LookAt(XMVectorSetY(vCamPos, 0.f));

	}
}

_float CMonster::Calc_Distance_XZ()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vPosTarget = m_pGameInstance->Get_CamPosition_Vec4();//임시사용 캠 포지션

	_float fDist = XMVectorGetX(XMVector3Length(XMVectorSetY(vPos, 0) - XMVectorSetY(vPosTarget, 0)));

	//X, Y 값 플레이어 비교로 위치구하기
	return fDist;
}

HRESULT CMonster::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Ready_FSM()
{
	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSMCom))))
		return E_FAIL;



	return S_OK;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	for (auto& pCollider : m_pColliderCom)
		Safe_Release(pCollider);
	if (m_pFSMCom != nullptr)
	{
		m_pFSMCom->Release_States();
	}
	Safe_Release(m_pFSMCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}