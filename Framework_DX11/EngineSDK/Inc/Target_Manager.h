#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_RenderTarget_Array(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize = 1);
	HRESULT Add_RenderTarget_For_Desc(const _wstring& strTargetTag,
		D3D11_BUFFER_DESC* const& pBufferDesc,
		D3D11_TEXTURE2D_DESC* const& pTextureDesc,
		D3D11_SHADER_RESOURCE_VIEW_DESC* const& pSRVDesc,
		D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUAVDesc, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isClear = true);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	HRESULT Bind_UnorderedAccess(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture);
	HRESULT Clear_MRT(const _wstring& strMRTTag);

	ID3D11Buffer* Get_Buffer(const _wstring& strTargetTag);
	ID3D11UnorderedAccessView* Get_UAV(const _wstring& strTargetTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
	
private:
	ID3D11Device*										m_pDevice = { nullptr };
	ID3D11DeviceContext*								m_pContext = { nullptr };	
	map<const _wstring, class CRenderTarget*>			m_RenderTargets;
	map<const _wstring, list<class CRenderTarget*>>		m_MRTs;

	ID3D11RenderTargetView*								m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView*								m_pDepthStencilView = { nullptr };
private:
	CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);


public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};



END