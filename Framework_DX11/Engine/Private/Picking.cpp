#include "..\Public\Picking.h"

#include "GameInstance.h"

CPicking::CPicking(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPicking::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;
	_uint			iNumViewports = { 1 };

	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	D3D11_TEXTURE2D_DESC	TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = m_iViewportWidth = (_int)ViewportDesc.Width;
	TextureDesc.Height = m_iViewportHeight = (_int)ViewportDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = 0;

	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;	

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPickDepthTexture)))
		return E_FAIL;


	//오브젝트 피킹용
	//렌더타겟 복사용
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (_int)ViewportDesc.Width;
	textureDesc.Height = (_int)ViewportDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pPickObjectTexture)))
		return E_FAIL;

	//고른 픽셀 복사용
	D3D11_TEXTURE2D_DESC textureDescMini;
	ZeroMemory(&textureDescMini, sizeof(D3D11_TEXTURE2D_DESC));
	textureDescMini.Width = 1;
	textureDescMini.Height = 1 ;
	textureDescMini.MipLevels = 1;
	textureDescMini.ArraySize = 1;
	textureDescMini.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDescMini.SampleDesc.Count = 1;
	textureDescMini.SampleDesc.Quality = 0;
	textureDescMini.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDescMini.Usage = D3D11_USAGE_STAGING;
	textureDescMini.BindFlags = 0;
	textureDescMini.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&textureDescMini, nullptr, &m_pPickObjectTextureMini)))
		return E_FAIL;

	return S_OK;
}

_bool CPicking::Picking(_float3 * pPickPos)
{
	//m_pContext->CopyResource(m_pPickDepthTexture, );

	if (FAILED(m_pGameInstance->Copy_RenderTarget(TEXT("Target_PickDepth"), m_pPickDepthTexture)))
		return false;

	POINT			ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	if (ptMouse.x < 0 || ptMouse.x >= (_int)(m_iViewportWidth) ||
		ptMouse.y < 0 || ptMouse.y >= (_int)(m_iViewportHeight))
	{
		return false;
	}

	_uint		iPixelIndex = ptMouse.y * m_iViewportWidth + ptMouse.x;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pPickDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	_float		fProjZ = ((_float4*)SubResource.pData)[iPixelIndex].x;
	_bool		isPicked = static_cast<_bool>(((_float4*)SubResource.pData)[iPixelIndex].w);

	m_pContext->Unmap(m_pPickDepthTexture, 0);

	if (false == isPicked)
		return false;

	_float4 vProjPos;

	/* 투영공간상의 위치. */
	vProjPos.x = ptMouse.x / (m_iViewportWidth * 0.5f) - 1.f;
	vProjPos.y = ptMouse.y / -(m_iViewportHeight * 0.5f) + 1.f;
	vProjPos.z = fProjZ;
	vProjPos.w = 1.f;

	/* 뷰공간상의 위치. */
	_vector	vViewPos = XMVector3TransformCoord(XMLoadFloat4(&vProjPos), m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ));

	_vector vWorldPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW));

	XMStoreFloat3(pPickPos, vWorldPos);

	return true;
}

_bool CPicking::Picking_Object(_uint* pPickID)
{
	if (FAILED(m_pGameInstance->Copy_RenderTarget(TEXT("Target_PickObjectDepth"), m_pPickObjectTexture)))
		return false;

	POINT			ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	if (ptMouse.x < 0 || ptMouse.x >= (_int)(m_iViewportWidth) ||
		ptMouse.y < 0 || ptMouse.y >= (_int)(m_iViewportHeight))
	{
		return false;
	}

	// 텍스처 복사
	D3D11_BOX srcBox;
	srcBox.left = ptMouse.x;
	srcBox.right = ptMouse.x + 1;
	srcBox.bottom = ptMouse.y + 1;
	srcBox.top = ptMouse.y;
	srcBox.front = 0;
	srcBox.back = 1;

	D3D11_MAPPED_SUBRESOURCE msr = {};
	ZeroMemory(&msr, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->CopySubresourceRegion(m_pPickObjectTextureMini, 0, 0, 0, 0, m_pPickObjectTexture, 0, &srcBox);

	m_pContext->Map(m_pPickObjectTextureMini, 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);

	// 픽셀의 색상 값 추출
	_float4 fcolorValue = *reinterpret_cast<_float4*>(msr.pData);

	// 각 색상 채널 추출 후 ID로 변환
	UINT8 r = static_cast<UINT8>(round(fcolorValue.x * 255));
	UINT8 g = static_cast<UINT8>(round(fcolorValue.y * 255));
	UINT8 b = static_cast<UINT8>(round(fcolorValue.z * 255));
	UINT8 a = static_cast<UINT8>(round(fcolorValue.w * 255));


	uint32_t hash = (static_cast<uint32_t>(a) << 24) |
		(static_cast<uint32_t>(r) << 16) |
		(static_cast<uint32_t>(g) << 8) |
		static_cast<uint32_t>(b);

	hash /= 100;

	*pPickID = static_cast<_uint>(hash);

	m_pContext->Unmap(m_pPickObjectTextureMini, 0);

	return true;
}

_vector CPicking::Compute_Height(_fvector vWorldPos, _fmatrix ViewMatrix, _fmatrix ProjMatrix)
{
	if (FAILED(m_pGameInstance->Copy_RenderTarget(TEXT("Target_Height"), m_pPickDepthTexture)))
		return XMVectorZero();

	_vector		vTargetProjPos = XMVector3TransformCoord(vWorldPos, ViewMatrix);
	vTargetProjPos = XMVector3TransformCoord(vTargetProjPos, ProjMatrix);

	/*-1 -> 0
	1 -> viewport.width

	1 -> 0
	-1 -> Viewport.height*/

	_float2		vTargetPos = _float2(XMVectorGetX(vTargetProjPos) * m_iViewportWidth * 0.5f + m_iViewportWidth * 0.5f,
		XMVectorGetY(vTargetProjPos) * m_iViewportHeight * -0.5f + m_iViewportHeight * 0.5f);


	_uint		iPixelIndex = _uint(vTargetPos.y) * m_iViewportWidth + _uint(vTargetPos.x);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pPickDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	_float		fProjZ = ((_float4*)SubResource.pData)[iPixelIndex].x;
	_bool		isPicked = static_cast<_bool>(((_float4*)SubResource.pData)[iPixelIndex].w);

	m_pContext->Unmap(m_pPickDepthTexture, 0);

	if (false == isPicked)
		return XMVectorZero();

	_float4 vProjPos;

	/* 투영공간상의 위치. */
	vProjPos.x = vTargetPos.x / (m_iViewportWidth * 0.5f) - 1.f;
	vProjPos.y = vTargetPos.y / -(m_iViewportHeight * 0.5f) + 1.f;
	vProjPos.z = fProjZ;
	vProjPos.w = 1.f;

	/* 뷰공간상의 위치. */
	_vector	vViewPos = XMVector3TransformCoord(XMLoadFloat4(&vProjPos), XMMatrixInverse(nullptr, ProjMatrix));

	_vector vResultPos = XMVector3TransformCoord(vViewPos, XMMatrixInverse(nullptr, ViewMatrix));
	
	return vResultPos;
}

CPicking * CPicking::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, HWND hWnd)
{
	CPicking*		pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPicking::Free()
{
	__super::Free();

	Safe_Release(m_pPickDepthTexture);
	Safe_Release(m_pPickObjectTexture);
	Safe_Release(m_pPickObjectTextureMini);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
