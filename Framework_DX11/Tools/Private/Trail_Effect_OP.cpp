#include "stdafx.h"
#include "Trail_Effect_OP.h"
#include "GameInstance.h"

CTrail_Effect_OP::CTrail_Effect_OP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CTrail_Effect_OP::CTrail_Effect_OP(const CTrail_Effect_OP& Prototype)
	: CEffect_Base(Prototype)
{
}

HRESULT CTrail_Effect_OP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Effect_OP::Initialize(void* pArg)
{
	TRAIL_OP_DESC* pDesc = static_cast<TRAIL_OP_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_ActionDesc = pDesc->InitDesc.ActionDesc;
	m_SaveDesc = *pDesc;


	m_vTestTop = _float3(5.f, 0.f, 0.f);
	m_ActionDesc.iShaderIndex = 1;
	m_ActionDesc.fScale = 0.1f;
	
	return S_OK;
}

void CTrail_Effect_OP::Priority_Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::G))
		m_bLoop = !m_bLoop;
	
}

void CTrail_Effect_OP::Update(_float fTimeDelta)
{
	_vector vTopDir = XMLoadFloat3(&m_vTestTop) - XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 15.f * fTimeDelta);
	vTopDir = XMVector3TransformNormal(vTopDir, RotationMatrix);

	XMStoreFloat3(&m_vTestTop, vTopDir);

	__super::Set_WorldMatrix();
	//if(true == m_pVIBufferCom->Update_Buffer(XMLoadFloat3(&m_vTestTop), 0.5f, m_bLoop, fTimeDelta))
	//	int a = 0;
	if (true == m_pVIBufferCom->Spread_Buffer(XMLoadFloat3(&m_vTestTop), 10.f, 0.25f, m_bLoop, fTimeDelta))
		int a = 0;;
}

void CTrail_Effect_OP::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this)))
		return;
}

HRESULT CTrail_Effect_OP::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_ActionDesc.vColor, sizeof m_ActionDesc.vColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_ActionDesc.vDivide, sizeof m_ActionDesc.vDivide)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSpeed", &m_ActionDesc.fSpriteSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_ActionDesc.fScale, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iState", &m_ActionDesc.iState, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_ActionDesc.iShaderIndex)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CTrail_Effect_OP::Reset()
{
	m_pVIBufferCom->Reset();
}

HRESULT CTrail_Effect_OP::Save(_wstring strFilePath)
{
	if (strFilePath.back() == L'\0')
		strFilePath.resize(strFilePath.size() - 1);

	_wstring strResultPath = strFilePath + TEXT("\\") + m_strEffectName + TEXT(".TE");

	_char FilePath[MAX_PATH] = {};
	int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (sizeNeeded > 0 && sizeNeeded <= MAX_PATH)
	{
		WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, FilePath, MAX_PATH, nullptr, nullptr);
	}

	ofstream outfile(FilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;
	
	outfile.write(reinterpret_cast<const _char*>(m_SaveDesc.szEffectName), sizeof(m_SaveDesc.szEffectName));
	outfile.write(reinterpret_cast<const _char*>(&m_SaveDesc.BufferDesc), sizeof(m_SaveDesc.BufferDesc));
	outfile.write(reinterpret_cast<const _char*>(&m_SaveDesc.InitDesc), sizeof(m_SaveDesc.InitDesc));

	outfile.close();

	return S_OK;
}

void CTrail_Effect_OP::Set_Desc(const ACTION_DESC& desc)
{
	m_SaveDesc.InitDesc.ActionDesc = desc;
	m_ActionDesc = desc;
}

CTrail_Effect_OP::TRAIL_OP_DESC CTrail_Effect_OP::Get_Desc()
{
	TRAIL_OP_DESC desc = {};
	desc.BufferDesc = m_SaveDesc.BufferDesc;
	desc.InitDesc.ActionDesc = m_ActionDesc;
	return desc;
}

HRESULT CTrail_Effect_OP::Ready_Components(const TRAIL_OP_DESC& Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Trail_OnePoint_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	Desc.BufferDesc.iNumInstance;
	Desc.BufferDesc.vLifeTime;

	CVIBuffer_Instancing::INSTANCE_DESC InstDesc = {};

	InstDesc.iNumInstance = Desc.BufferDesc.iNumInstance;
	InstDesc.vLifeTime = Desc.BufferDesc.vLifeTime;

	InstDesc.iNumInstance = 50;
	InstDesc.vLifeTime = _float2(2.f, 2.f);

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Trail_OnePoint_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstDesc)))
		return E_FAIL;

	return S_OK;
}

CTrail_Effect_OP* CTrail_Effect_OP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Effect_OP* pInstance = new CTrail_Effect_OP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CTrail_Effect_OP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Effect_OP::Clone(void* pArg)
{
	CTrail_Effect_OP* pInstance = new CTrail_Effect_OP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CTrail_Effect_OP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Effect_OP::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom_1);
	Safe_Release(m_pMaskTextureCom_2);
}
