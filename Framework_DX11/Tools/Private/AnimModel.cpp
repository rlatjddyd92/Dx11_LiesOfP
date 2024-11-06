#include "stdafx.h"
#include "AnimModel.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"

#include "Controller_EffectTool.h"

CAnimModel::CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CAnimModel::CAnimModel(const CAnimModel& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CAnimModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimModel::Initialize(void* pArg)
{
	ANIMMODEL_DESC* pDesc = static_cast<ANIMMODEL_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_pTransformCom->Rotation(_vector{0, 1, 0, 0}, XMConvertToRadians(180.f));

	//테스트용
	{
		m_pModelCom->SetUp_Animation(0, true);
		m_pModelCom->SetUp_NextAnimation(0, true);
		m_pModelCom->SetUp_NextAnimation_Boundary(0, true);
	}

	memcpy(&m_tDesc, pDesc, sizeof(ANIMMODEL_DESC));
	return S_OK;
}

void CAnimModel::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CAnimModel::Update(_float fTimeDelta)
{
	/*if ((GetKeyState(VK_LBUTTON) & 0x8000) &&
		true == m_pVIBufferCom->isPicking(m_pTransformCom->Get_WorldMatrix(), &vPickPos))
	{
		if (CImGui_Manager::Get_Instance()->Get_IsPicking())
		{
			CImGui_Manager::Get_Instance()->Set_ModelPos(vPickPos);
		}
	}*/

	if (*(m_tDesc.pUpdateCtr) == false)
	{
		return;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	//_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (KEY_HOLD(KEY::I))
		m_pTransformCom->Go_Straight(fTimeDelta);//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetX(vPos, XMVectorGetX(vPos) + 0.1f));
	
	_bool	bEndCheck{false};
	_vector vRootMove = m_pModelCom->Play_Animation(fTimeDelta, &bEndCheck);

	if (bEndCheck == true)//조건을 애니메이션이 끝났을때 or 변경 되었을때로
	{
		vRootMove = m_vRootMoveStack = XMVectorSet(0, 0, 0, 1);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vRootMove - m_vRootMoveStack);
		m_vRootMoveStack = vRootMove;
	}

}

void CAnimModel::Late_Update(_float fTimeDelta)
{
	if (*(m_tDesc.pUpdateCtr) == false)
	{
		return;
	}

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	if(false == CController_EffectTool::Get_Instance()->Get_JunhoCamera())
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CAnimModel::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
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

HRESULT CAnimModel::Ready_Components(ANIMMODEL_DESC* pAnimDesc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	_tchar szModelTag[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pAnimDesc->szModelTag, MAX_PATH, szModelTag, MAX_PATH);

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_AnimModel_Test"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CAnimModel* CAnimModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimModel* pInstance = new CAnimModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CAnimModel::Clone(void* pArg)
{
	CAnimModel* pInstance = new CAnimModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimModel::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
