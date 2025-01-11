#include "stdafx.h"
#include "Stargazer.h"
#include "GameInstance.h"
#include "Pawn.h"

#include "Stargazer.h"

#include "Effect_Container.h"
#include "Effect_Manager.h"

#include "GameInterface_Controller.h"

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
	 
	m_iAnim_Close = m_pModelCom[RESTORED]->Find_AnimationIndex("AS_Close_Idle", 2.f);
	m_iAnim_Open = m_pModelCom[RESTORED]->Find_AnimationIndex("AS_Open", 6.f);
	m_iAnim_OpenIdle = m_pModelCom[RESTORED]->Find_AnimationIndex("AS_Open_Idle", 2.f);
	m_iAnim_Broken = m_pModelCom[BROKEN]->Find_AnimationIndex("AS_Stargazer_broken_idle", 3.f);
	m_iAnim_Restore = m_pModelCom[BROKEN]->Find_AnimationIndex("AS_Stargazer_restore", 3.f);
	
	m_pModelCom[RESTORED]->SetUp_Animation(m_iAnim_Close, true);
	m_pModelCom[BROKEN]->SetUp_Animation(m_iAnim_Broken, true);

	m_pCurrentModel = m_pModelCom[BROKEN];

	m_strObjectTag = TEXT("Stargazer");

	_Vec3 vEffectPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vEffectPos.y += 0.9f;

	CEffect_Container* pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("StarGazer_Active"), nullptr, nullptr,
		vEffectPos);
	m_Effects.push_back(pEffect);

	pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("StarGazer_Deactive"), nullptr, nullptr,
		(_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_Effects.push_back(pEffect);

	m_Effects[1]->Set_Loop(true);

	return S_OK;
}

void CStargazer::Priority_Update(_float fTimeDelta)
{
	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Priority_Update(fTimeDelta);
	}
}

void CStargazer::Update(_float fTimeDelta)
{
	m_pCurrentModel->Play_Animation(fTimeDelta);

	if (m_bStartRestore && m_bFirstRestored)
	{
		m_bFirstRestored = false;
		//키 입력 확인 부분은 나중에 UI에서 받아와야 함
		static_cast<CPawn*>(m_pPlayer)->Set_Respawn_Cell_Num(m_iCurrnetCellNum);

		if (m_bRestored == false)
		{
			m_Effects[1]->Set_Loop(false);
			m_pCurrentModel->SetUp_Animation(m_iAnim_Restore);
		}

		m_pSoundCom->Play2D(TEXT("AMB_OJ_PR_Stargazer_Restore_Activated.wav"), &g_fEffectVolume);
	}

	//복구 애니메이션 진행 중
	if (m_bRestored == false && m_pCurrentModel->Get_CurrentAnimationIndex() == m_iAnim_Restore)
	{
		m_fRestoreTimer += fTimeDelta;

		if (m_fRestoreTimer > 3.3f)	//미리 작동
		{
			m_pModelCom[RESTORED]->Play_Animation(fTimeDelta);
		}

		if (m_fRestoreTimer > 3.8f)
		{
			m_bRestored = true;
			m_pCurrentModel = m_pModelCom[RESTORED];
			m_fRestoreTimer = 0.f; 
		}
	}

	if (m_bRestored && m_isClose)
	{
		m_isClose = false;
		m_isOpening = true;
		m_pCurrentModel->SetUp_NextAnimation(m_iAnim_Open, false, 0 , 0);
		m_pSoundCom->Play2D(TEXT("AMB_OJ_PR_Stargazer_Open_01.wav"), &g_fEffectVolume);
	}

	//열리는 중->열리고 나서 기본 애니로 변경
	if (m_isOpening && m_pCurrentModel->Get_IsEndAnimArray()[m_iAnim_Open])
	{
		m_isOpening = false;
		m_isOpened = true;
		m_Effects[0]->Set_Loop(true);
		m_pCurrentModel->SetUp_NextAnimation(m_iAnim_OpenIdle, true);
	}

	if(m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());


	_uint iFrame = m_pCurrentModel->Get_CurrentFrame();
	if (m_pCurrentModel == m_pModelCom[BROKEN])
	{
		if (m_iAnim_Restore == m_pCurrentModel->Get_CurrentAnimationIndex())
		{
			if (iFrame >= 210 && !m_isInteractEffect)
			{
				_Vec3 vEffectPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				vEffectPos.y += 0.9f;
				CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("StarGazer_Interaction"), vEffectPos);

				GET_GAMEINTERFACE->Show_Inform(INFORM_MESSAGE::INFORM_STARGAZER);

				m_isInteractEffect = true;
			}
		}
	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Update(fTimeDelta);
	}
}

void CStargazer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Late_Update(fTimeDelta);
	}

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

	_uint		iNumMeshes = m_pCurrentModel->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pCurrentModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pCurrentModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (nullptr != m_pCurrentModel->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pCurrentModel->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		if (nullptr != m_pCurrentModel->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
		{
			if (FAILED(m_pCurrentModel->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		if (FAILED(m_pCurrentModel->Render((_uint)i)))
			return E_FAIL;
	}

	if (m_fRestoreTimer > 3.5f && m_bRestored == false)
	{
		_uint		iNumMeshes = m_pModelCom[RESTORED]->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom[RESTORED]->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom[RESTORED]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", DIFFUSE, (_uint)i)))
				return E_FAIL;

			if (nullptr != m_pModelCom[RESTORED]->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
			{
				if (FAILED(m_pModelCom[RESTORED]->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
					return E_FAIL;
			}

			if (nullptr != m_pModelCom[RESTORED]->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
			{
				if (FAILED(m_pModelCom[RESTORED]->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(2)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;
			}

			if (FAILED(m_pModelCom[RESTORED]->Render((_uint)i)))
				return E_FAIL;
		}
	}
	return S_OK;

}

void CStargazer::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bCollison = true;
	}
}

void CStargazer::OnCollisionStay(CGameObject* pOther)
{
}

void CStargazer::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bCollison = false;
	}
}

HRESULT CStargazer::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stargazer_Broken"),
		TEXT("Com_Model0"), reinterpret_cast<CComponent**>(&m_pModelCom[BROKEN]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stargazer"),
		TEXT("Com_Model1"), reinterpret_cast<CComponent**>(&m_pModelCom[RESTORED]))))
		return E_FAIL;

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_VoiceSound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderDesc{};
	ColliderDesc.fRadius = { 2.f };
	ColliderDesc.vCenter = { 0.f, 0.5f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
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

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 0.5f;
	CapsuleDesc.fRadius = 0.25f;
	RigidBodyDesc.pGeometryDesc = &CapsuleDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;
	return S_OK;
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
	
	if (m_isCloned)
	{
		for (auto& pEffect : m_Effects)
		{
			pEffect->Set_Cloned(false);
			Safe_Release(pEffect);
		}
		m_Effects.clear();
	}

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);

	for (int i = 0; i < 2; ++i)
	{
		Safe_Release(m_pModelCom[i]);
	}

	Safe_Release(m_pRigidBodyCom);
}
