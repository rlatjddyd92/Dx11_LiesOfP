#include "stdafx.h"
#include "Lift_Controller.h"
#include "GameInstance.h"
#include "Lift_Floor.h"
CLift_Controller::CLift_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLift_Controller::CLift_Controller(const CLift_Controller& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLift_Controller::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLift_Controller::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x - 0.05f, pDesc->vScale.y - 0.05f, pDesc->vScale.z - 0.05f);
	m_pTransformCom->Rotation(0.f, -25.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_bShadow = pDesc->bShadow;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayer = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY);
	
	m_iAnim_Idle = m_pModelCom->Find_AnimationIndex("AS_Idle", 2.5f);
	m_iAnim_Open = m_pModelCom->Find_AnimationIndex("AS_Lever_Open", 2.5f);
	m_pModelCom->SetUp_Animation(m_iAnim_Open);

	m_strObjectTag = TEXT("Lift_Controller");

	return S_OK;
}

void CLift_Controller::Priority_Update(_float fTimeDelta)
{
	if(m_pLift_Floor == nullptr)
		m_pLift_Floor = static_cast<CLift_Floor*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Lift_Floor"), 0));
}

void CLift_Controller::Update(_float fTimeDelta)
{
	//컨트롤러 작동, 엘베가 도착해 있으면 작동 X
	if (m_bClose_with_Player && m_bMoveFloor == false && Is_Close_Between_Lift_Floor() == false)
	{
		//키 입력 확인 부분은 나중에 UI에서 받아와야 함
		if (KEY_TAP(KEY::E))
		{
			m_bMoveFloor = true;
		}
	}

	//컨트롤러 애니메이션 설정
	if (m_bMoveFloor && fActiveTimer == 0.f)
	{
		m_pModelCom->SetUp_Animation(m_iAnim_Open);
	}
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_bMoveFloor)
	{
		fActiveTimer += fTimeDelta;

		//바닥 작동 시킨다.
		if (m_bActiveFloor == false && fActiveTimer > 4.f)
		{
			Set_Floor_Dir();
		}
	}
	//바닥 이동이 끝난 후
	if (m_bActiveFloor && m_pLift_Floor->Get_isMoving() == false)
	{
		m_bMoveFloor = false;
		m_bActiveFloor = false;
		fActiveTimer = 0.f;
	}

	Calculate_Distance_Between_Player();

	if(m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CLift_Controller::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CLift_Controller::Render()
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

HRESULT CLift_Controller::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lift_Controller"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 1.0f, 0.5f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.f, 1.0f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

void CLift_Controller::Calculate_Distance_Between_Player()
{
	_Vec4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_Vec4 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVectorGetX(XMVector4Length(vMyPos - vPlayerPos));

	if (fDistance < 2.f)
		m_bClose_with_Player = true;
	else
		m_bClose_with_Player = false;
}

_bool CLift_Controller::Is_Close_Between_Lift_Floor()
{
	_Vec4 vFloor = m_pLift_Floor->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_Vec4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (fabs(vFloor.y - vCurPos.y) < 1.f)
		return true;

	return false;
}

void CLift_Controller::Set_Floor_Dir()
{
	m_bActiveFloor = true;

	_Vec4 vControllerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//바닥 이동방향 세팅
	if (vControllerPos.y > -100.f)
	{
		m_pLift_Floor->Set_Move_Dir(CLift_Floor::UP);
	}
	else
	{
		m_pLift_Floor->Set_Move_Dir(CLift_Floor::DOWN);
	}
}

CLift_Controller* CLift_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLift_Controller* pInstance = new CLift_Controller(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLift_Controller"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLift_Controller::Clone(void* pArg)
{
	CLift_Controller* pInstance = new CLift_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLift_Controller"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLift_Controller::Free()
{
	__super::Free();

//	Safe_Delete(m_pPlayer);
//	Safe_Delete(m_pLift_Floor);
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
