#include "stdafx.h"

//#include "ImGuizmo.h"
#include <io.h>

#include "Controller_PostProcess.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

void CCamera_Manager::Update_CameraSetting()
{
	static _float fCameraFar = m_pGameInstance->Get_Far();

	ImGui::DragFloat("Far", &fCameraFar, 5.f, 1.f, 1000.f);

	m_pGameInstance->Find_Camera(LEVEL_TOOL)->Set_Far(fCameraFar);
}

void CCamera_Manager::Update_PBR()
{
	_bool* pIsPBR = m_pGameInstance->Get_IsOnPBR();

	static _bool isOnPBR = false;

	ImGui::Checkbox("PBR Value", &isOnPBR);

	*pIsPBR = isOnPBR;

	
}

void CCamera_Manager::Update_SSAO()
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

void CCamera_Manager::Update_HDR()
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

void CCamera_Manager::Update_BLOOM()
{
	BLOOM_DESC* tDesc = m_pGameInstance->Get_BloomDesc();
	if (nullptr == tDesc)
		return;

	static _bool isOnBloom = false;
	static _float fBloomThreshold = tDesc->fThreshold;

	ImGui::Checkbox("BLOOM Value", &isOnBloom);


	if (tDesc->isOnBloom)
	{
		ImGui::DragFloat("BloomThreshold", &fBloomThreshold, 0.05f);
	}

	tDesc->isOnBloom = isOnBloom;
	tDesc->fThreshold = fBloomThreshold;
}

void CCamera_Manager::Update_DOF()
{
	DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
	if (nullptr == tDesc)
		return;

	static _bool isOnDof = false;
	static _bool isInverse = false;
	static _float fDOF = tDesc->fDOF;

	ImGui::Checkbox("DOF Value", &isOnDof);


	if (tDesc->isOnDOF)
	{
		//ImGui::Checkbox("DOF Inverse", &isInverse);
		ImGui::DragFloat("DOF", &fDOF, 0.001f);
	}

	tDesc->isOnDOF = isOnDof;
	tDesc->isInverse = isInverse;
	tDesc->fDOF = fDOF;
}

void CCamera_Manager::Update_Radial()
{
	RADIAL_DESC* tDesc = m_pGameInstance->Get_RadialDesc();
	if (nullptr == tDesc)
		return;

	static _bool isOnRadial = tDesc->isOnRadial;
	static _Vec2 vCenterPos = tDesc->vRadialCenterPos;
	static _float fRadialPower = tDesc->fRadialPower;
	static _float fRadius = tDesc->fRadius;

	ImGui::Checkbox("Radial Value", &isOnRadial);


	if (tDesc->isOnRadial)
	{
		ImGui::DragFloat("CenterX", &vCenterPos.x, 0.02f);
		ImGui::DragFloat("CenterY", &vCenterPos.y, 0.02f);
		ImGui::DragFloat("RadialPower", &fRadialPower, 0.005f);
	}

	tDesc->isOnRadial = isOnRadial;
	tDesc->vRadialCenterPos = vCenterPos;
	tDesc->fRadialPower = fRadialPower;
	tDesc->fRadius = fRadius;
}

void CCamera_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
