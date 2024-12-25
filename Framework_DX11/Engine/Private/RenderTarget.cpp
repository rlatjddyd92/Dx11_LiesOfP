#include "..\Public\RenderTarget.h"

#include "Shader.h"
#include "Shader_NonVTX.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	m_vClearColor = vClearColor;

	D3D11_TEXTURE2D_DESC	TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Initialize_Array(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor, _uint iArraySize)
{
	m_vClearColor = vClearColor;

	D3D11_TEXTURE2D_DESC	TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = iArraySize;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc{};
	ZeroMemory(&RenderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	RenderTargetViewDesc.Format = ePixelFormat;
	RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RenderTargetViewDesc.Texture2DArray.ArraySize = iArraySize;
	RenderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
	RenderTargetViewDesc.Texture2DArray.MipSlice = 0;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, &RenderTargetViewDesc, &m_pRTV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourcesViewDesc{};
	ZeroMemory(&ShaderResourcesViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ShaderResourcesViewDesc.Format = ePixelFormat;
	ShaderResourcesViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	ShaderResourcesViewDesc.Texture2DArray.ArraySize = iArraySize;
	ShaderResourcesViewDesc.Texture2DArray.FirstArraySlice = 0;
	ShaderResourcesViewDesc.Texture2DArray.MipLevels = 1;
	ShaderResourcesViewDesc.Texture2DArray.MostDetailedMip = 0;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, &ShaderResourcesViewDesc, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Initialize_For_Desc(D3D11_BUFFER_DESC* const& pBufferDesc, D3D11_TEXTURE2D_DESC* const& pTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC* const& pSRVDesc, D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUAVDesc, const _float4& vClearColor)
{
	HRESULT hr = S_OK;

	// 버퍼가 있는 경우 -> 우리가 만드는 렌더타겟이 아님
	if (nullptr != pBufferDesc) 
	{
		// 이거 못 만들면 소용 없어
		if (FAILED(m_pDevice->CreateBuffer(pBufferDesc, nullptr, &m_pBuffer)))
			return E_FAIL;

		if (nullptr != pSRVDesc)
		{
			if (pBufferDesc->BindFlags & D3D11_BIND_SHADER_RESOURCE)
				hr |= m_pDevice->CreateShaderResourceView(m_pBuffer, pSRVDesc, &m_pSRV);
		}

		if (nullptr != pUAVDesc)
		{
			if (pBufferDesc->BindFlags & D3D11_BIND_UNORDERED_ACCESS)
				hr |= m_pDevice->CreateUnorderedAccessView(m_pBuffer, pUAVDesc, &m_pUAV);
		}
	}
	else if (nullptr != pTextureDesc)	// 텍스쳐 만들경우 -> 여러 경우를 따져야함
	{
		// 이거 못 만들면 아무것도 못 만들어
		if (FAILED(m_pDevice->CreateTexture2D(pTextureDesc, nullptr, &m_pTexture2D)))
			return E_FAIL;

		// 렌더타겟 바인드 할거임?
		if (pTextureDesc->BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
				return E_FAIL;
		}

		if (nullptr != pSRVDesc || pTextureDesc->BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			hr |= m_pDevice->CreateShaderResourceView(m_pTexture2D, pSRVDesc, &m_pSRV);
		}

		if (nullptr != pUAVDesc || pTextureDesc->BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			hr |= m_pDevice->CreateUnorderedAccessView(m_pTexture2D, pUAVDesc, &m_pUAV);
		}

	}

	m_vClearColor = vClearColor;

	return hr;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);
}

HRESULT CRenderTarget::Bind_ShaderResource_NonVTX(CShader_NonVTX* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);
}

HRESULT CRenderTarget::Bind_UnorderedAccess(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_UAV(pConstantName, m_pUAV);;
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
}

HRESULT CRenderTarget::Copy(ID3D11Texture2D * pTexture)
{
	m_pContext->CopyResource(pTexture, m_pTexture2D);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Initialize_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint		iNumViewport = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;

	m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
		return E_FAIL;

	pShader->Begin(0);

	pVIBuffer->Bind_Buffers();

	pVIBuffer->Render();

	return S_OK;
}

#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderTarget * CRenderTarget::Create_Array(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize)
{
	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Array(iWidth, iHeight, ePixelFormat, vClearColor, iArraySize)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderTarget* CRenderTarget::Create_For_Desc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, D3D11_BUFFER_DESC* const& pBufferDesc, D3D11_TEXTURE2D_DESC* const& pTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC* const& pSRVDesc, D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUAVDesc, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_For_Desc(pBufferDesc, pTextureDesc, pSRVDesc, pUAVDesc, vClearColor)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderTarget::Free()
{
	__super::Free();	

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);


	Safe_Release(m_pUAV);
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexture2D);
}
