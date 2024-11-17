#include "UIRender.h"
#include "UIRender.h"
#include "stdafx.h"
#include "..\Public\UIRender.h"

#include "GameInstance.h"
#include "Controller_UITool.h"
#include <fstream>
#include <sstream>
#include <stack>

CUIRender::CUIRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
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

	if (FAILED(Ready_Texture_ItemIcon()))
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
	_int iBack = CController_UITool::Get_Instance()->GetBackNum();

	if ((iBack >= 478) && (iBack <= 507))
		if(FAILED(BackRender(iBack)))
			return E_FAIL;

	_int iMax = CController_UITool::Get_Instance()->GetPartCount();

	for (_int i = 0; i < iMax; ++i)
	{
		CController_UITool::UPART& rNow = CController_UITool::Get_Instance()->Get_PartRenderInfo(i);

		if (rNow.iTexture_Index != -1)
		{
			m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

			if (rNow.iMoveType == _int(CController_UITool::MOVETYPE::TYPE_BAR))
				m_pTransformCom->Set_Scaled(rNow.GetBarSize().x, rNow.GetBarSize().y, 1.f);
			else 
				m_pTransformCom->Set_Scaled(rNow.fSize.x, rNow.fSize.y, 1.f);

			

			m_pTransformCom->Set_State(CTransform::STATE_POSITION,
				XMVectorSet(rNow.fPosition.x - m_fViewWidth * 0.5f, -rNow.fPosition.y + m_fViewHeight * 0.5f, 0.f, 1.f));

			if (rNow.bTurn)
				m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(rNow.fTurn_Degree));

			if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				return E_FAIL;

			if (!rNow.bIsItem)
			{
				if (rNow.iTexture_Index >= m_vecTextureInfo.size())
					return S_OK;

				if (FAILED(m_vecTextureInfo[rNow.iTexture_Index]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
					return E_FAIL;
			}
			else
			{
				if (rNow.iTexture_Index >= m_vecTextureInfo_ItemIcon.size())
					return S_OK;

				if (FAILED(m_vecTextureInfo_ItemIcon[rNow.iTexture_Index]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
					return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &rNow.fTextureColor, sizeof(_float4))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Bind_Buffers()))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
		if ((rNow.iFontIndex >= 0) && (rNow.iFontIndex < _int(UI_FONT::FONT_END)))
		{
			if (rNow.szText == nullptr)
				continue;

			_vector vPosition = { rNow.fPosition.x, rNow.fPosition.y, 0.f,0.f };
			_vector vColor = { 1.f,1.f,1.f,1.f };

			if (rNow.fTextColor.x > 0.f)
				vColor.m128_f32[0] = rNow.fTextColor.x;

			if (rNow.fTextColor.y > 0.f)
				vColor.m128_f32[1] = rNow.fTextColor.y;

			if (rNow.fTextColor.z > 0.f)
				vColor.m128_f32[2] = rNow.fTextColor.z;

			if (rNow.fTextColor.w > 0.f)
				vColor.m128_f32[3] = rNow.fTextColor.w;


			if (rNow.bCenter)
				m_pGameInstance->Render_TextCenter(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor);
			else
				m_pGameInstance->Render_Text(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor);
		}
	}
	
	return S_OK;
}

void CUIRender::AddRenderUIObject(_int iTextureIndex, _float2 fPosition, _float2 fSize, _float3 fRGB, _float fAlpha)
{
	URCOM* pNew = new URCOM;

	pNew->iTextureIndex = iTextureIndex;
	pNew->fPosition = fPosition;
	pNew->fSize = fSize;
	pNew->fRGB = fRGB;
	pNew->fAlpah = fAlpha;

	m_UIRenderlist.push_back(pNew);
}

void CUIRender::AddRenderText(UI_FONT eFont, _bool bIsCenter, _tchar* szText, _float2 fPosition, _float3 fColor, _float fAlpha)
{
	URCOM* pNew = new URCOM;

	pNew->fPosition = fPosition;
	pNew->fRGB = fColor;
	pNew->fAlpah = fAlpha;
	pNew->eType = eFont;
	pNew->szText = szText;
	pNew->bIsCenter = bIsCenter;

	m_UIRenderlist.push_back(pNew);
}

HRESULT CUIRender::BackRender(_int iIndex)
{
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	m_pTransformCom->Set_Scaled(g_iWinSizeX, g_iWinSizeY, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet((1280.f * 0.5f) - m_fViewWidth * 0.5f, -(720.f * 0.5f) + m_fViewHeight * 0.5f, 0.f, 1.f));

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_vecTextureInfo[iIndex]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_float4 fColor = { -1,-1,-1,-1 };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &fColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


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
	
	m_pGameInstance->Add_Font(TEXT("Font_Normal_8"), TEXT("../Bin/Resources/Fonts/Font_Normal_8.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Normal_12"), TEXT("../Bin/Resources/Fonts/Font_Normal_12.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Normal_16"), TEXT("../Bin/Resources/Fonts/Font_Normal_16.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Normal_24"), TEXT("../Bin/Resources/Fonts/Font_Normal_24.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Normal_32"), TEXT("../Bin/Resources/Fonts/Font_Normal_32.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Normal_40"), TEXT("../Bin/Resources/Fonts/Font_Normal_40.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Normal_48"), TEXT("../Bin/Resources/Fonts/Font_Normal_48.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Bold_8"), TEXT("../Bin/Resources/Fonts/Font_Bold_8.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Bold_12"), TEXT("../Bin/Resources/Fonts/Font_Bold_12.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Bold_16"), TEXT("../Bin/Resources/Fonts/Font_Bold_16.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Bold_24"), TEXT("../Bin/Resources/Fonts/Font_Bold_24.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Bold_32"), TEXT("../Bin/Resources/Fonts/Font_Bold_32.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Bold_40"), TEXT("../Bin/Resources/Fonts/Font_Bold_40.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Font_Bold_48"), TEXT("../Bin/Resources/Fonts/Font_Bold_48.spritefont"));

	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_8)] ="Font_Normal_8";
	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_12)] = "Font_Normal_12";
	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_16)] = "Font_Normal_16";
	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_24)] = "Font_Normal_24";
	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_32)] = "Font_Normal_32";
	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_40)] = "Font_Normal_40";
	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_48)] = "Font_Normal_48";
	m_vecFont_char[_int(UI_FONT::FONT_BOLD_8)] = "Font_Bold_8";
	m_vecFont_char[_int(UI_FONT::FONT_BOLD_12)] = "Font_Bold_12";
	m_vecFont_char[_int(UI_FONT::FONT_BOLD_16)] = "Font_Bold_16";
	m_vecFont_char[_int(UI_FONT::FONT_BOLD_24)] = "Font_Bold_24";
	m_vecFont_char[_int(UI_FONT::FONT_BOLD_32)] = "Font_Bold_32";
	m_vecFont_char[_int(UI_FONT::FONT_BOLD_40)] = "Font_Bold_40";
	m_vecFont_char[_int(UI_FONT::FONT_BOLD_48)] = "Font_Bold_48";

	m_vecFont_tchar[_int(UI_FONT::FONT_NORMAL_8)] = TEXT("Font_Normal_8");
	m_vecFont_tchar[_int(UI_FONT::FONT_NORMAL_12)] = TEXT("Font_Normal_12");
	m_vecFont_tchar[_int(UI_FONT::FONT_NORMAL_16)] = TEXT("Font_Normal_16");
	m_vecFont_tchar[_int(UI_FONT::FONT_NORMAL_24)] = TEXT("Font_Normal_24");
	m_vecFont_tchar[_int(UI_FONT::FONT_NORMAL_32)] = TEXT("Font_Normal_32");
	m_vecFont_tchar[_int(UI_FONT::FONT_NORMAL_40)] = TEXT("Font_Normal_40");
	m_vecFont_tchar[_int(UI_FONT::FONT_NORMAL_48)] = TEXT("Font_Normal_48");
	m_vecFont_tchar[_int(UI_FONT::FONT_BOLD_8)] = TEXT("Font_Bold_8");
	m_vecFont_tchar[_int(UI_FONT::FONT_BOLD_12)] = TEXT("Font_Bold_12");
	m_vecFont_tchar[_int(UI_FONT::FONT_BOLD_16)] = TEXT("Font_Bold_16");
	m_vecFont_tchar[_int(UI_FONT::FONT_BOLD_24)] = TEXT("Font_Bold_24");
	m_vecFont_tchar[_int(UI_FONT::FONT_BOLD_32)] = TEXT("Font_Bold_32");
	m_vecFont_tchar[_int(UI_FONT::FONT_BOLD_40)] = TEXT("Font_Bold_40");
	m_vecFont_tchar[_int(UI_FONT::FONT_BOLD_48)] = TEXT("Font_Bold_48");
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

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, tTag, pNew->Texture)))
			return E_FAIL;

		WideCharToMultiByte(CP_UTF8, 0, tPath, -1, pNew->strTexturePath, (_int)vecBuffer[i][0].size() + 1, nullptr, nullptr);
		WideCharToMultiByte(CP_UTF8, 0, tTag, -1, pNew->strTextureTag, (_int)vecBuffer[i][1].size() + 1, nullptr, nullptr);

		m_vecTextureInfo.push_back(pNew);

		Safe_Delete_Array(tPath);
		Safe_Delete_Array(tTag);
	}

	return S_OK;
}

HRESULT CUIRender::Ready_Texture_ItemIcon()
{
	vector<vector<_wstring>> vecBuffer;
	m_pGameInstance->LoadDataByFile("../Bin/Resources/textures/Item/ItemList.csv", &vecBuffer);

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

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, tTag, pNew->Texture)))
			return E_FAIL;

		WideCharToMultiByte(CP_UTF8, 0, tPath, -1, pNew->strTexturePath, (_int)vecBuffer[i][0].size() + 1, nullptr, nullptr);
		WideCharToMultiByte(CP_UTF8, 0, tTag, -1, pNew->strTextureTag, (_int)vecBuffer[i][1].size() + 1, nullptr, nullptr);

		m_vecTextureInfo_ItemIcon.push_back(pNew);

		Safe_Delete_Array(tPath);
		Safe_Delete_Array(tTag);
	}

	return S_OK;
}

CUIRender* CUIRender::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIRender* pInstance = new CUIRender(pDevice, pContext);

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

	for (auto& iter : m_vecTextureInfo_ItemIcon)
	{
		//		Safe_Release(iter->Texture);
		Safe_Delete_Array(iter->strTexturePath);
		Safe_Delete_Array(iter->strTextureTag);
		Safe_Delete(iter);
	}

	m_vecTextureInfo_ItemIcon.clear();

	for (auto& iter : m_UIRenderlist)
		Safe_Delete(iter);

	m_vecFont_char.clear();
	m_vecFont_tchar.clear();

	

	m_UIRenderlist.clear();

	Safe_Release(m_pVIBufferCom);
}
