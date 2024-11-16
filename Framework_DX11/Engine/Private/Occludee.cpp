#include "Occludee.h"

COccludee::COccludee(ID3D11Device* pDevice)
	: m_pDevice{ pDevice }
{
	Safe_AddRef(m_pDevice);
}

HRESULT COccludee::Initialize()
{
	//DeferredContext는 여러개 생성 가능
	m_pDevice->CreateDeferredContext(0, &m_pDefferedContext);

	ZeroMemory(&m_Hiz_VP, sizeof(D3D11_VIEWPORT));
	m_Hiz_VP.TopLeftX = 0;
	m_Hiz_VP.TopLeftY = 0;
	m_Hiz_VP.Width = 1280.f;
	m_Hiz_VP.Height = 720.f;
	m_Hiz_VP.MinDepth = 0.f;
	m_Hiz_VP.MaxDepth = 1.f;

	m_pDefferedContext->RSSetViewports(1, &m_Hiz_VP);
	m_pDefferedContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//	m_pDefferedContext->IASetVertexBuffers(0, 1, m_Screen_DB->VertexBuf->GetAddress(), &m_Screen_DB->Stride, &m_Screen_DB->Offset);
	//m_pDefferedContext->IASetIndexBuffer(m_Screen_DB->IndexBuf->Get(), DXGI_FORMAT_R32_UINT, 0);

    return S_OK;
}

COccludee* COccludee::Create(ID3D11Device* pDevice)
{
	COccludee* pInstance = new COccludee(pDevice);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : COccludee"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COccludee::Free()
{
	__super::Free();

	Safe_Release(m_pDefferedContext);
	Safe_Release(m_pDevice);
}
