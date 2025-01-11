#include "stdafx.h"
#include "Item_Throw.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

CItem_Throw::CItem_Throw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CItem_Throw::CItem_Throw(const CItem_Throw& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CItem_Throw::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Throw::Initialize(void* pArg)
{
	THROWITEM_DESC* pDesc = static_cast<THROWITEM_DESC*>(pArg);

	m_eType = pDesc->eType;
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pParentMatrix = pDesc->pParentWorldMatrix;
	m_vThrowDir = pDesc->vThrowDir;
	m_vThrowDir.Normalize();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_strObjectTag = TEXT("ThrowITem");

	m_isThrow = false;
	m_fThrowTime = 0.f;

	_matrix		SocketMatrix = *m_pSocketMatrix;

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pTransformCom->Set_WorldMatrix(m_WorldMatrix);



	return S_OK;
}

void CItem_Throw::Priority_Update(_float fTimeDelta)
{
	m_Effect->Priority_Update(fTimeDelta);
}

void CItem_Throw::Update(_float fTimeDelta)
{
	if (!m_isExplosion)
	{
		if (!m_isRealThrow && m_fThrowTime > 0.2f)
		{
			m_Effect->Set_Loop(true);
			m_isRealThrow = true;
			m_pRigidBodyCom->Add_Force(m_vThrowDir);
		}

		if (!m_isRealThrow)
		{
			_matrix		SocketMatrix = *m_pSocketMatrix;

			for (size_t i = 0; i < 3; i++)
			{
				SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
			}
			XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

			m_pTransformCom->Set_WorldMatrix(m_WorldMatrix);

			_Vec3 vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pRigidBodyCom->Set_GloblePose(vCurrentPos);
		}
		
		if (m_isThrow)
		{
			m_fThrowTime += fTimeDelta;

			_Vec3 vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fGroundY = m_pNavigationCom->Get_CellPosY(m_pTransformCom);

			m_pNavigationCom->isMove(vCurrentPos);
			
			if (vCurrentPos.y <= fGroundY || m_fThrowTime > 2.3f)
			{
				Explosion();
			}

			m_pRigidBodyCom->Update(fTimeDelta);

			if (m_pColliderCom != nullptr)
			{
				m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
				m_pGameInstance->Add_ColliderList(m_pColliderCom);
			}

			m_Effect->Update(fTimeDelta);
		}
	}
	else if (m_isExplosion)
	{
		if (!m_pSoundCom->Get_IsPlaying())
		{
			m_isDead = true;
		}
	}

}

void CItem_Throw::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_Effect->Late_Update(fTimeDelta);
#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CItem_Throw::Render()
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

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(1)))
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

void CItem_Throw::OnCollisionEnter(CGameObject* pOther)
{
	if (!m_isExplosion && pOther->Get_Tag() == TEXT("Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);


		switch (m_eType)
		{
		case SPECIAL_ITEM::SP_GRANADE:
			pMonster->Calc_DebuffGain(CMonster::SURFACE_EFFECT::SURFACE_FIRE, 3.f);
			pMonster->Calc_DamageGain(170.f, (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION), HIT_FIRE, ATK_STRONG);
			break;
		case SPECIAL_ITEM::SP_THERMITE:
			pMonster->Calc_DamageGain(155.f, (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION), HIT_FIRE, ATK_STRONG);
			break;
		case SPECIAL_ITEM::SP_THROW_BATTERY:
			pMonster->Calc_DebuffGain(CMonster::SURFACE_EFFECT::SURFACE_ELECTRIC, 3.f);
			pMonster->Calc_DamageGain(155.f, (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION), HIT_FIRE, ATK_STRONG);
			break;
		}

		Explosion();
	}
}

void CItem_Throw::OnCollisionStay(CGameObject* pOther)
{
}

void CItem_Throw::OnCollisionExit(CGameObject* pOther)
{
}

void CItem_Throw::Explosion()
{
	if (m_isExplosion && !m_isThrow)
		return;

	switch (m_eType)
	{
	case SPECIAL_ITEM::SP_GRANADE:
		CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Item_Fire"), (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pSoundCom->Play2D(TEXT("SE_PC_SK_FX_FlameThrower_Upgrade_End_01.wav"), &g_fEffectVolume);
		break;
	case SPECIAL_ITEM::SP_THERMITE:
		CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Item_Bomb"), (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pSoundCom->Play2D(TEXT("SE_PC_SK_FX_FlameThrower_Upgrade_Explo_01.wav"), &g_fEffectVolume);
		break;
	case SPECIAL_ITEM::SP_THROW_BATTERY:
		CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Item_Electric"), (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pSoundCom->Play2D(TEXT("SE_PC_SK_Hit_Electric_Common_01.wav"), &g_fEffectVolume); 
		break;
	}

	m_isExplosion = true;
}

void CItem_Throw::Throw()
{
	if (m_isThrow)
		return;

	_Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y = m_pParentMatrix->Translation().y;

	m_pNavigationCom->Research_Cell(vPos);

	m_vThrowDir.Normalize();

	m_vThrowDir *= 800.f;
	//m_vThrowDir.y *= 100.f;


	m_isThrow = true;
}

HRESULT CItem_Throw::Ready_Components()
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	_wstring strModelTag{};

	switch (m_eType)
	{
	case SPECIAL_ITEM::SP_GRANADE:
		strModelTag = TEXT("Prototype_Component_Model_Throw_Cluster");
		m_Effect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Player_Item_Fire_Throw"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr, _Vec3(0.f, 0.f, 0.15f));
		break;
	case SPECIAL_ITEM::SP_THERMITE:
		strModelTag = TEXT("Prototype_Component_Model_Throw_Thermite");
		m_Effect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Player_Item_Bomb_Throw"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr, _Vec3(0.f, 0.f, 0.15f));
		break;
	case SPECIAL_ITEM::SP_THROW_BATTERY:
		strModelTag = TEXT("Prototype_Component_Model_Throw_Battery");
		m_Effect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Player_Item_Electric_Throw"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr, _Vec3(0.f, 0.f, 0.15f));
		break;
	default:
		return E_FAIL;
	}

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderDesc{};
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.5f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = true;
	RigidBodyDesc.isUseClient = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = nullptr;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z
		;

	physX::GeometrySphere SphereDesc;
	SphereDesc.fRadius = 0.1f;
	RigidBodyDesc.pGeometryDesc = &SphereDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

CItem_Throw* CItem_Throw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Throw* pInstance = new CItem_Throw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CItem_Throw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Throw::Clone(void* pArg)
{
	CItem_Throw* pInstance = new CItem_Throw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CItem_Throw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Throw::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		m_Effect->Set_Cloned(false);
		Safe_Release(m_Effect);
	}

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pSoundCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
