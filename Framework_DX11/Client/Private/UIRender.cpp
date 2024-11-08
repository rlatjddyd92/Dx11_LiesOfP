#include "stdafx.h"
#include "UIRender.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIRender::CUIRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<CUIPage*>* pVecUIPage)
	: CUIObject{ pDevice, pContext }
{
	m_vecUIPage = pVecUIPage;
}

CUIRender::CUIRender(const CUIRender& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIRender::Initialize_Prototype()
{
	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Ready_Font();

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRender::Initialize(void* pArg)
{
	return S_OK;
}

void CUIRender::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIRender::Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIRender::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIRender::Render()
{
	
	return S_OK;
}

HRESULT CUIRender::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* FOR.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_UIRender"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CUIRender::Ready_Font()
{
	m_vecFont_char.resize(_int(UI_FONT::FONT_END));
	m_vecFont_tchar.resize(_int(UI_FONT::FONT_END));



	m_pGameInstance->Add_Font(TEXT("FONT_INFO_12"), TEXT("../Bin/Resources/Fonts/Font_Info_12.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_18"), TEXT("../Bin/Resources/Fonts/Font_Info_18.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_24"), TEXT("../Bin/Resources/Fonts/Font_Info_24.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_36"), TEXT("../Bin/Resources/Fonts/Font_Info_36.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_48"), TEXT("../Bin/Resources/Fonts/Font_Info_48.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_60"), TEXT("../Bin/Resources/Fonts/Font_Info_60.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_72"), TEXT("../Bin/Resources/Fonts/Font_Info_72.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_12"), TEXT("../Bin/Resources/Fonts/Font_Title_12.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_18"), TEXT("../Bin/Resources/Fonts/Font_Title_18.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_24"), TEXT("../Bin/Resources/Fonts/Font_Title_24.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_36"), TEXT("../Bin/Resources/Fonts/Font_Title_36.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_48"), TEXT("../Bin/Resources/Fonts/Font_Title_48.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_60"), TEXT("../Bin/Resources/Fonts/Font_Title_60.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_72"), TEXT("../Bin/Resources/Fonts/Font_Title_72.spritefont"));

	m_vecFont_char[_int(UI_FONT::FONT_INFO_12)] = "FONT_INFO_12";
	m_vecFont_char[_int(UI_FONT::FONT_INFO_18)] = "FONT_INFO_18";
	m_vecFont_char[_int(UI_FONT::FONT_INFO_24)] = "FONT_INFO_24";
	m_vecFont_char[_int(UI_FONT::FONT_INFO_36)] = "FONT_INFO_36";
	m_vecFont_char[_int(UI_FONT::FONT_INFO_48)] = "FONT_INFO_48";
	m_vecFont_char[_int(UI_FONT::FONT_INFO_60)] = "FONT_INFO_60";
	m_vecFont_char[_int(UI_FONT::FONT_INFO_72)] = "FONT_INFO_72";
	m_vecFont_char[_int(UI_FONT::FONT_TITLE_12)] = "FONT_TITLE_12";
	m_vecFont_char[_int(UI_FONT::FONT_TITLE_18)] = "FONT_TITLE_18";
	m_vecFont_char[_int(UI_FONT::FONT_TITLE_24)] = "FONT_TITLE_24";
	m_vecFont_char[_int(UI_FONT::FONT_TITLE_36)] = "FONT_TITLE_36";
	m_vecFont_char[_int(UI_FONT::FONT_TITLE_48)] = "FONT_TITLE_48";
	m_vecFont_char[_int(UI_FONT::FONT_TITLE_60)] = "FONT_TITLE_60";
	m_vecFont_char[_int(UI_FONT::FONT_TITLE_72)] = "FONT_TITLE_72";

	m_vecFont_tchar[_int(UI_FONT::FONT_INFO_12)] = TEXT("FONT_INFO_12");
	m_vecFont_tchar[_int(UI_FONT::FONT_INFO_18)] = TEXT("FONT_INFO_18");
	m_vecFont_tchar[_int(UI_FONT::FONT_INFO_24)] = TEXT("FONT_INFO_24");
	m_vecFont_tchar[_int(UI_FONT::FONT_INFO_36)] = TEXT("FONT_INFO_36");
	m_vecFont_tchar[_int(UI_FONT::FONT_INFO_48)] = TEXT("FONT_INFO_48");
	m_vecFont_tchar[_int(UI_FONT::FONT_INFO_60)] = TEXT("FONT_INFO_60");
	m_vecFont_tchar[_int(UI_FONT::FONT_INFO_72)] = TEXT("FONT_INFO_72");
	m_vecFont_tchar[_int(UI_FONT::FONT_TITLE_12)] = TEXT("FONT_TITLE_12");
	m_vecFont_tchar[_int(UI_FONT::FONT_TITLE_18)] = TEXT("FONT_TITLE_18");
	m_vecFont_tchar[_int(UI_FONT::FONT_TITLE_24)] = TEXT("FONT_TITLE_24");
	m_vecFont_tchar[_int(UI_FONT::FONT_TITLE_36)] = TEXT("FONT_TITLE_36");
	m_vecFont_tchar[_int(UI_FONT::FONT_TITLE_48)] = TEXT("FONT_TITLE_48");
	m_vecFont_tchar[_int(UI_FONT::FONT_TITLE_60)] = TEXT("FONT_TITLE_60");
	m_vecFont_tchar[_int(UI_FONT::FONT_TITLE_72)] = TEXT("FONT_TITLE_72");
}

HRESULT CUIRender::Ready_Texture()
{
	vector<vector<_wstring>> vecBuffer;
	m_pGameInstance->LoadDataByFile("../Bin/Resources/textures/UI/UIList.csv", &vecBuffer);

	for (_int i = 2; i < vecBuffer.size(); ++i)
	{
		_tchar* tPath = new _tchar[vecBuffer[i][0].size() + 1];
		_tchar* tTag = new _tchar[vecBuffer[i][1].size() + 1];

		memcpy(tPath, &vecBuffer[i][0][0], sizeof(_tchar) * (vecBuffer[i][0].size() + 1));
		memcpy(tTag, &vecBuffer[i][1][0], sizeof(_tchar) * (vecBuffer[i][1].size() + 1));

		UTEXTURE* pNew = new UTEXTURE;

		pNew->Texture = CTexture::Create(m_pDevice, m_pContext, tPath, 1);
		pNew->strTexturePath = new _char[vecBuffer[i][0].size() + 1];
		pNew->strTextureTag = new _char[vecBuffer[i][1].size() + 1];

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, tTag, pNew->Texture)))
			return E_FAIL;

		WideCharToMultiByte(CP_UTF8, 0, tPath, -1, pNew->strTexturePath, vecBuffer[i][0].size() + 1, nullptr, nullptr);
		WideCharToMultiByte(CP_UTF8, 0, tTag, -1, pNew->strTextureTag, vecBuffer[i][1].size() + 1, nullptr, nullptr);

		m_vecTextureInfo.push_back(pNew);

		Safe_Delete_Array(tPath);
		Safe_Delete_Array(tTag);
	}

	return S_OK;
}

CUIRender* CUIRender::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<CUIPage*>* pVecUIPage)
{
	CUIRender* pInstance = new CUIRender(pDevice, pContext, pVecUIPage);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIRender"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUIRender::Clone(void* pArg)
{
	CUIRender* pInstance = new CUIRender(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIRender"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIRender::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	for (auto& iter : m_vecTextureInfo)
	{
		//		Safe_Release(iter->Texture);
		Safe_Delete_Array(iter->strTexturePath);
		Safe_Delete_Array(iter->strTextureTag);
		Safe_Delete(iter);
	}

	m_vecTextureInfo.clear();

	

	m_vecFont_char.clear();
	m_vecFont_tchar.clear();



	Safe_Release(m_pVIBufferCom);
}
