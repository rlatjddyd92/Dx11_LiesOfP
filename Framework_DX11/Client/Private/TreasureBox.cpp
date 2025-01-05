#include "stdafx.h"
#include "TreasureBox.h"
#include "GameInstance.h"
#include "Item_Dropped.h"
#include "Effect_Container.h"
#include "Effect_Manager.h"

CTreasureBox::CTreasureBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTreasureBox::CTreasureBox(const CTreasureBox& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTreasureBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTreasureBox::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(0.f, pDesc->vRotation.y + 90.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_bShadow = pDesc->bShadow;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(1, true);

	m_iAnim_Close_Idle = m_pModelCom->Find_AnimationIndex("AS_Close_Idle", 2.f);
	m_iAnim_Open = m_pModelCom->Find_AnimationIndex("AS_Open", 2.f);
	m_iAnim_Open_Idle = m_pModelCom->Find_AnimationIndex("AS_Open_Idle", 2.f);

	m_pModelCom->SetUp_Animation(m_iAnim_Close_Idle, true);

	m_strObjectTag = TEXT("TreasureBox");

	m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("ItemBox_Closed"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr);
	m_pEffect->Set_Loop(true);

	return S_OK;
}

void CTreasureBox::Priority_Update(_float fTimeDelta)
{
	if (m_bOpen == false)
	{
		m_pEffect->Priority_Update(fTimeDelta);
	}
}

void CTreasureBox::Update(_float fTimeDelta)
{
	if (m_bOpen)
	{
		m_pModelCom->SetUp_NextAnimation(m_iAnim_Open, false);

		if(m_bAddItem == false)
		{
			m_bAddItem = true;
			CItem_Dropped::ITEM_DROPPED_DESC pDesc = {};
			pDesc.vParentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item_Dropped"), TEXT("Prototype_GameObject_Item_Dropped"), &pDesc);
		}
	}
	else
		m_pEffect->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CTreasureBox::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	if (m_bOpen == false)
	{
		m_pEffect->Late_Update(fTimeDelta);
	}
#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CTreasureBox::Render()
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

void CTreasureBox::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bCollision = true;
	}
}

void CTreasureBox::OnCollisionStay(CGameObject* pOther)
{
}

void CTreasureBox::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bCollision = false;
	}
}

HRESULT CTreasureBox::Ready_Components(OBJECT_DEFAULT_DESC* pDesc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreasureBox"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.f, 0.8f, 0.8f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.f, 1.0f, ColliderDesc.vExtents.z * 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = nullptr;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
		PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
		PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
		PxRigidDynamicLockFlag::eLOCK_LINEAR_Z
		;

	physX::GeometryBox BoxDesc;
	BoxDesc.vSize = _Vec3(1.f, 1.f, 1.f);
	RigidBodyDesc.pGeometry = &BoxDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

CTreasureBox* CTreasureBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTreasureBox* pInstance = new CTreasureBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTreasureBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasureBox::Clone(void* pArg)
{
	CTreasureBox* pInstance = new CTreasureBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTreasureBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreasureBox::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		m_pEffect->Set_Cloned(false);
		Safe_Release(m_pEffect);
	}

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigidBodyCom);
}
