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

			if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				return E_FAIL;

			if (FAILED(m_vecTextureInfo[rNow.iTexture_Index]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
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
	ifstream file("../Bin/Resources/textures/UI/UIList.csv");  // 읽을 CSV 파일 이름
	string line;

	if (!file.is_open())
		return E_FAIL;

	stack<_int> stackNum;
	_int iTextureNum = 0;
	getline(file, line);
	for (_int i = 0; i < line.size(); ++i)
		stackNum.push(line[i] - '0');

	_int iCount = 0;
	while (!stackNum.empty())
	{
		if ((stackNum.top() >= 0) && (stackNum.top() < 10))
		{
			iTextureNum += stackNum.top() * pow(10, iCount);
			++iCount;
		}
		stackNum.pop();
	}

	getline(file, line); // 헤더 넘기기

	m_vecTextureInfo.resize(iTextureNum);

	_int iTextureCount = 0;

	while (getline(file, line))
	{
		UTEXTURE* pNew = new UTEXTURE;
		
		pNew->strTexturePath = new _char[200];
		pNew->strTextureTag = new _char[200];

		_int iIndex = 0;
		_int iNow = 0;

		while (line[iIndex] != ',')
		{
			pNew->strTexturePath[iNow] = line[iIndex];
			++iIndex;
			++iNow;
		}

		pNew->strTexturePath[iNow] = '\0';

		iNow = 0;
		--iIndex;

		do
		{
			++iIndex;
			++iNow;
			pNew->strTextureTag[iNow] = line[iIndex];
		} while (line[iIndex] != '\0');

		_tchar tPath[200] = {};
		_tchar tTag[200] = {};

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNew->strTexturePath, strlen(pNew->strTexturePath), tPath, 200);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNew->strTextureTag, strlen(pNew->strTextureTag), tTag, 200);

		pNew->Texture = CTexture::Create(m_pDevice, m_pContext, tPath, 1);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, tTag, pNew->Texture)))
			return E_FAIL;

		m_vecTextureInfo[iTextureCount] = pNew;
		++iTextureCount;

	}

	file.close();

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

	for (auto& iter : m_UIRenderlist)
		Safe_Delete(iter);

	m_vecFont_char.clear();
	m_vecFont_tchar.clear();

	

	m_UIRenderlist.clear();

	Safe_Release(m_pVIBufferCom);
}
