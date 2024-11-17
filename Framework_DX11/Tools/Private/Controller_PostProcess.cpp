#include "stdafx.h"

//#include "ImGuizmo.h"
#include <io.h>

#include "Controller_PostProcess.h"

IMPLEMENT_SINGLETON(CController_PostProcess)

CController_PostProcess::CController_PostProcess()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_PostProcess::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

void CController_PostProcess::Update_SSAO()
{
	SSAO_DESC* tDesc = m_pGameInstance->Get_SSAODesc();
	if (nullptr == tDesc)
		return;

	static _bool isOnSSAO = false;
	static _float fRadius = 1.f;
	static _float fBias = 0.3f;
	static _float fAmount = 1.f;

	ImGui::Checkbox("SSAO Value", &isOnSSAO);

	tDesc->isOnSSAO = isOnSSAO;

	if (tDesc->isOnSSAO)
	{
		ImGui::DragFloat("Radius", &fRadius, 0.005f);
		ImGui::DragFloat("Bias", &fBias, 0.005f);
		ImGui::DragFloat("Amount", &fAmount, 0.05f);
	}
	tDesc->fRadius = fRadius;
	tDesc->fBias = fBias;
	tDesc->fAmount = fAmount;
}

void CController_PostProcess::Update_HDR()
{
	HDR_DESC* tDesc = m_pGameInstance->Get_HDRDesc();
	if (nullptr == tDesc)
		return;

	static _bool isOnHDR = false;
	static _float fMiddleGrey = tDesc->fMiddleGrey;
	static _float fLumWhiteSqr = tDesc->fLumWhiteSqr;

	ImGui::Checkbox("HDR Value", &isOnHDR);

	tDesc->isOnHDR = isOnHDR;

	if (tDesc->isOnHDR)
	{
		ImGui::DragFloat("MiddleGrey", &fMiddleGrey, 0.05f);
		ImGui::DragFloat("LumWhiteSqr", &fLumWhiteSqr, 0.05f);
	}

	tDesc->fMiddleGrey = fMiddleGrey;
	tDesc->fLumWhiteSqr = fLumWhiteSqr;
}

void CController_PostProcess::Update_BLOOM()
{
	BLOOM_DESC* tDesc = m_pGameInstance->Get_BloomDesc();
	if (nullptr == tDesc)
		return;

	static _bool isOnBloom = false;
	static _float fBloomThreshold = tDesc->fBloomThreshold;

	ImGui::Checkbox("BLOOM Value", &isOnBloom);


	if (tDesc->isOnBloom)
	{
		ImGui::DragFloat("BloomThreshold", &fBloomThreshold, 0.05f);
	}

	tDesc->isOnBloom = isOnBloom;
	tDesc->fBloomThreshold = fBloomThreshold;
}

void CController_PostProcess::Update_DOF()
{
	DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
	if (nullptr == tDesc)
		return;

	static _bool isOnDof = false;
	static _float fDOF = tDesc->fDOF;

	ImGui::Checkbox("DOF Value", &isOnDof);


	if (tDesc->isOnDOF)
	{
		ImGui::DragFloat("DOF", &fDOF, 0.02f, 0.f, 1.f);
	}

	tDesc->isOnDOF = isOnDof;
	tDesc->fDOF = fDOF;
}

void CController_PostProcess::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
