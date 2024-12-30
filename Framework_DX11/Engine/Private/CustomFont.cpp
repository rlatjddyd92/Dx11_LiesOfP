#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	// 알파 블렌딩 활성화
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 텍스처 샘플러 설정
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 선형 필터링
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;


	m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);

	m_pDevice->CreateBlendState(&blendDesc, &m_pBlendState);

	m_pBatch = new SpriteBatch(m_pContext);

	return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	if (vColor.m128_f32[3] != 1.f)
		m_pBatch->Begin(SpriteSortMode_Deferred, m_pBlendState, m_pSamplerState);
	else
		m_pBatch->Begin();

	_float fRound = round(vPosition.m128_f32[0]);
	fRound = round(vPosition.m128_f32[1]);

	m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRadian, vPivot, fScale);

	m_pBatch->End();

	return S_OK;
}

HRESULT CCustomFont::Render_Center(const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	XMVECTOR vSize = m_pFont->MeasureString(pText);
	XMVECTOR vAdjustedPosition = vPosition - (vSize * 0.5f * fScale);

	if (vColor.m128_f32[3] != 1.f)
		m_pBatch->Begin(SpriteSortMode_Deferred, m_pBlendState, m_pSamplerState);
	else
		m_pBatch->Begin();

	_float fRound = round(vAdjustedPosition.m128_f32[0]);
	fRound = round(vAdjustedPosition.m128_f32[1]);

	m_pFont->DrawString(m_pBatch, pText, vAdjustedPosition, vColor, fRadian, vPivot, fScale);

	m_pBatch->End();

	return S_OK;
}
// 24-11-22 김성용 
// 오른쪽 정렬로 텍스트 그리기
HRESULT CCustomFont::Render_Right(const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	XMVECTOR vSize = { 0.f,0.f,0.f,0.f };
	vSize.m128_f32[0] = m_pFont->MeasureString(pText).m128_f32[0];
	XMVECTOR vAdjustedPosition = vPosition - (vSize * fScale);

	if (vColor.m128_f32[3] != 1.f)
		m_pBatch->Begin(SpriteSortMode_Deferred, m_pBlendState, m_pSamplerState);
	else
		m_pBatch->Begin();

	_float fRound = round(vAdjustedPosition.m128_f32[0]);
	fRound = round(vAdjustedPosition.m128_f32[1]);

	m_pFont->DrawString(m_pBatch, pText, vAdjustedPosition, vColor, fRadian, vPivot, fScale);

	m_pBatch->End();

	return S_OK;
}

_Vec4 CCustomFont::Measure_Font(_wstring strText)
{
	_tchar szText[100] = {};

	for (_int i = 0; i <= strText.size(); ++i)
		szText[i] = strText[i];

	return m_pFont->MeasureString(szText);
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CCustomFont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	__super::Free();

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pBlendState->Release();
	m_pSamplerState->Release();
}
