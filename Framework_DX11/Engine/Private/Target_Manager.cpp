#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget_Array(const _wstring & strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor, _uint iArraySize)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create_Array(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor, iArraySize);
	if(nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget_For_Desc(const _wstring& strTargetTag, D3D11_BUFFER_DESC* const& pBufferDesc, D3D11_TEXTURE2D_DESC* const& pTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC* const& pSRVDesc, D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUAVDesc, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create_For_Desc(m_pDevice, m_pContext, pBufferDesc, pTextureDesc, pSRVDesc, pUAVDesc, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring & strMRTTag, const _wstring & strTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;


	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(strMRTTag, MRTList);		
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool isClear)
{
	// 초기화해주기
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = { nullptr };
	//ID3D11UnorderedAccessView* pUAV[D3D11_1_UAV_SLOT_COUNT] = { nullptr };

	//m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT, pSRV);
	//m_pContext->CSSetUnorderedAccessViews(0, D3D11_1_UAV_SLOT_COUNT, pUAV, nullptr);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iIndex = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		if (isClear)
			pRenderTarget->Clear();
		pRenderTargets[iIndex++] = pRenderTarget->Get_RTV();
	}

	if (nullptr != pDSV)
	{
		m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		m_pContext->OMSetRenderTargets(iIndex, pRenderTargets, pDSV);
	}
	else
		m_pContext->OMSetRenderTargets(iIndex, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);		

	return S_OK;	
}

HRESULT CTarget_Manager::Bind_ShaderResource(CShader * pShader, const _wstring & strTargetTag, const _char * pConstantName)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);	
}

HRESULT CTarget_Manager::Bind_ShaderResource_NonVTX(CShader_NonVTX* pShader, const _wstring& strTargetTag, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource_NonVTX(pShader, pConstantName);
}

HRESULT CTarget_Manager::Bind_UnorderedAccess(CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_UnorderedAccess(pShader, pConstantName);
}

HRESULT CTarget_Manager::Copy_RenderTarget(const _wstring & strTargetTag, ID3D11Texture2D * pTexture)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Copy(pTexture);	
}

HRESULT CTarget_Manager::Clear_MRT(const _wstring& strMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
	}

	return S_OK;
}

HRESULT CTarget_Manager::Clear_RTV(const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	pRenderTarget->Clear();

	return S_OK;
}

ID3D11Buffer* CTarget_Manager::Get_Buffer(const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_Buffer();
}

ID3D11UnorderedAccessView* CTarget_Manager::Get_UAV(const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_UAV();
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const _wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Initialize_Debug(fX, fY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render(const _wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render(pShader, pVIBuffer);

	return S_OK;
}

#endif

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _wstring & strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring & strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

CTarget_Manager * CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTarget_Manager*		pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();	
	}
		
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();



	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
