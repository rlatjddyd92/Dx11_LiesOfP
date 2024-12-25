#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11Buffer* Get_Buffer() const {
		return m_pBuffer;
	}

	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

	ID3D11UnorderedAccessView* Get_UAV() const {
		return m_pUAV;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Initialize_Array(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize);

	// 직접 desc을 정의해서 넘겨주자
	HRESULT Initialize_For_Desc(D3D11_BUFFER_DESC* const& pBufferDesc,
		D3D11_TEXTURE2D_DESC* const& pTextureDesc,
		D3D11_SHADER_RESOURCE_VIEW_DESC* const& pShaderResourcesViewDesc,
		D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUnorderedAccessViewDesc, const _float4& vClearColor);

	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_ShaderResource_NonVTX(class CShader_NonVTX* pShader, const _char* pConstantName);
	HRESULT Bind_UnorderedAccess(class CShader* pShader, const _char* pConstantName);
	void Clear();
	HRESULT Copy(ID3D11Texture2D* pTexture);

#ifdef _DEBUG
public:
	HRESULT Initialize_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif


private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	ID3D11Buffer*				m_pBuffer = { nullptr };
	ID3D11Texture2D*			m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	ID3D11UnorderedAccessView*	m_pUAV = { nullptr };
	_float4						m_vClearColor = {};

#ifdef _DEBUG

private:
	_float4x4					m_WorldMatrix = {};

#endif





public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	static CRenderTarget* Create_Array(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize = 1);
	static CRenderTarget* Create_For_Desc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		D3D11_BUFFER_DESC* const& pBufferDesc,
		D3D11_TEXTURE2D_DESC* const& pTextureDesc,
		D3D11_SHADER_RESOURCE_VIEW_DESC* const& pSRVDesc, 
		D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUAVDesc, const _float4& vClearColor);

	virtual void Free() override;
};

END