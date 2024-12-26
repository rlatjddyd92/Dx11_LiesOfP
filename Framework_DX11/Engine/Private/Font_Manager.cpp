#include "..\Public\Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Initialize()
{
	return S_OK;
}

HRESULT CFont_Manager::Add_Font(const _wstring & strFontTag, const _tchar * pFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	CCustomFont*		pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render(const _wstring & strFontTag, const _tchar * pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	CCustomFont*		pFont = Find_Font(strFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	return pFont->Render(pText, vPosition, vColor, fRadian, vPivot, fScale);	
}

HRESULT CFont_Manager::Render_Center(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	CCustomFont* pFont = Find_Font(strFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	return pFont->Render_Center(pText, vPosition, vColor, fRadian, vPivot, fScale);
}

// 24-11-22 김성용 
// 오른쪽 정렬로 텍스트 그리기
HRESULT CFont_Manager::Render_Right(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	CCustomFont* pFont = Find_Font(strFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	return pFont->Render_Right(pText, vPosition, vColor, fRadian, vPivot, fScale);
}

_Vec4 CFont_Manager::Measure_Font(const _wstring& strFontTag, _wstring strText)
{
	CCustomFont* pFont = Find_Font(strFontTag);

	if (nullptr == pFont)
		return { -1.f,-1.f ,-1.f ,-1.f };

	return pFont->Measure_Font(strText);
}

CCustomFont * CFont_Manager::Find_Font(const _wstring & strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;	
}

CFont_Manager * CFont_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFont_Manager*		pInstance = new CFont_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CFont_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
