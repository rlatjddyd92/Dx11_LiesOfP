#include "stdafx.h"
#include "Stargazer.h"
#include "GameInstance.h"
#include "Pawn.h"
CStargazer::CStargazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CStargazer::CStargazer(const CStargazer& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CStargazer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStargazer::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(0.f, pDesc->vRotation.y, pDesc->vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_bShadow = pDesc->bShadow;
	m_iCurrnetCellNum = pDesc->iCurrentCellNum;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayer = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY);

	m_iAnim_Close = m_pModelCom->Find_AnimationIndex("AS_Close_Idle", 3.f);
	m_iAnim_Open = m_pModelCom->Find_AnimationIndex("AS_Open", 0.1f);
	m_iAnim_OpenIdle = m_pModelCom->Find_AnimationIndex("AS_Open_Idle", 3.f);

	m_pModelCom->SetUp_Animation(m_iAnim_Close, true);

	return S_OK;
}

void CStargazer::Priority_Update(_float fTimeDelta)
{
}

void CStargazer::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	Calculate_Distance_Between_Player();

	if (m_bClose_with_Player)
	{
		if (KEY_TAP(KEY::E))
		{
			//키 입력 확인 부분은 나중에 UI에서 받아와야 함
			static_cast<CPawn*>(m_pPlayer)->Set_Respawn_Cell_Num(m_iCurrnetCellNum);

			if (m_isClose)
			{
				m_isClose = false;
				m_isOpening = true;
				m_pModelCom->SetUp_NextAnimation(m_iAnim_Open);
			}
			
			//UI 기능 넣기
		}
	}

	//열리는 중->열리고 나서 기본 애니로 변경
	if (m_isOpening && m_pModelCom->Get_IsEndAnimArray())
	{
		m_isOpening = false;
		m_isOpened = true;
		m_pModelCom->SetUp_NextAnimation(m_iAnim_OpenIdle, true);
	}

	if(m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CStargazer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CStargazer::Render()
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

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;

}

HRESULT CStargazer::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stargazer"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.7f, 1.0f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

void CStargazer::Calculate_Distance_Between_Player()
{
	_Vec4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_Vec4 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVectorGetX(XMVector4Length(vMyPos - vPlayerPos));

	if(fDistance < 2.f)
		m_bClose_with_Player = true;
	else
		m_bClose_with_Player = false;
}

CStargazer* CStargazer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStargazer* pInstance = new CStargazer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CStargazer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStargazer::Clone(void* pArg)
{
	CStargazer* pInstance = new CStargazer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CStargazer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStargazer::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
