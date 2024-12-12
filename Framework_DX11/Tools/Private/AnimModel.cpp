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

void CAnimModel::ChangeAnim(_int iAnimIndex)
{
	m_pModelCom->ReadyDenyNextTranslate(m_iSubRootBone);
	m_vSubRootMoveStack = _Vec3{ 0, 0, 0 };
	m_pModelCom->SetUp_NextAnimation(iAnimIndex, true);
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
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(0.f,0.f,0.f));
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

	if (KEY_TAP(KEY::R))
		m_bRender = !m_bRender;

	if (KEY_HOLD(KEY::I))
		m_pTransformCom->Go_Straight(fTimeDelta);//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetX(vPos, XMVectorGetX(vPos) + 0.1f));

	if (KEY_TAP(KEY::C))
		m_bSubRootMove = !m_bSubRootMove;

	if (KEY_TAP(KEY::E))
	{
		m_bRemoteTuning = !m_bRemoteTuning;
		m_pModelCom->Set_RemoteTuning(m_bRemoteTuning);
	}

	_bool	bEndCheck{false};
	_vector vRootMove{};
	if (m_pModelCom != nullptr)
	{
		if (m_pModelCom->Get_Animations().size() > 0)
		{
			vRootMove = m_pModelCom->Play_Animation(fTimeDelta);
		}
	}
	

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vRootMove = XMVector3TransformNormal(vRootMove, m_pTransformCom->Get_WorldMatrix());

	if (m_bSubRootMove && !m_pModelCom->Get_isChangeAni())
	{
		_Vec3 vMove = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_iSubRootBone)->Translation();
		_float4x4 TransMat;
		XMStoreFloat4x4(&TransMat, m_pModelCom->Get_Bones()[m_iSubRootBone]->Get_TransformationMatrix());
		//TransMat._43 = TransMat._42 = TransMat._41 = 0.f;
		TransMat._41 = 0.f;

		m_pModelCom->Get_Bones()[m_iSubRootBone]->Set_TransformationMatrix(TransMat);;

		m_pModelCom->Update_Bone();

		vMove = XMVector3TransformNormal(vMove, m_pTransformCom->Get_WorldMatrix());

		vMove.y = 0;

		//m_pRigidBody->Set_Velocity((vMove - m_vRootMoveStack) / fTimeDelta);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3{ vPos } + (vMove - _Vec3{ m_vSubRootMoveStack }));

		m_vSubRootMoveStack = vMove;
	}

	vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vRootMove - m_vRootMoveStack);



}

void CAnimModel::Late_Update(_float fTimeDelta)
{
	if (*(m_tDesc.pUpdateCtr) == false)
	{
		return;
	}

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	if(true == m_bRender)
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
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

		// EMISSIVE
		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::EMISSIVE))
		{
			m_fEmissive = 1.f;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmessiveTexture", EMISSIVE, (_uint)i)))
				return E_FAIL;
		}
		else
		{
			m_fEmissive = 0.f;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissive, sizeof(_float))))
			return E_FAIL;

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
   	if (FAILED(__super::Add_Component(LEVEL_TOOL, szModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CComponent* CAnimModel::Change_Component(const _wstring& strComponentTag, CComponent* pComponent, _uint iPartObjIndex)
{
	CComponent* pOut = __super::Find_Component(strComponentTag);
	if (pOut != nullptr)
	{
		Safe_Release(pOut);
		m_Components.erase(strComponentTag);
	}
	m_Components.emplace(strComponentTag, pComponent);

	if (strComponentTag == TEXT("Com_Model"))
	{
		m_pModelCom = dynamic_cast<CModel*>(pComponent);
	}

	Safe_AddRef(pComponent);

	return pOut;
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
