#include "stdafx.h"
#include "..\Public\UIRender_Batching.h"

#include "GameInstance.h"
#include "UIPage_Ortho.h"

CUIRender_Batching::CUIRender_Batching(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CUIRender_Batching::CUIRender_Batching(const CUIRender_Batching& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIRender_Batching::Initialize_Prototype()
{
	// UI 렌더는 클론 안 함, 여기서 모든 세팅 끝내기 

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

	if (FAILED(Ready_Texture_UIPart()))
		return E_FAIL;

	if (FAILED(Ready_Texture_ItemIcon()))
		return E_FAIL;

	Ready_Scroll_Adjust_Data();

	return S_OK;
}

HRESULT CUIRender_Batching::Initialize(void* pArg)
{
	return S_OK;
}

void CUIRender_Batching::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIRender_Batching::Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIRender_Batching::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

}

HRESULT CUIRender_Batching::Render()
{
	

	_float fWidth = m_vecViewPort[_int(m_eNow_Area)]->Width;
	_float fHeight = m_vecViewPort[_int(m_eNow_Area)]->Height;
	_float fStartX = m_vecViewPort[_int(m_eNow_Area)]->TopLeftX;
	_float fStartY = m_vecViewPort[_int(m_eNow_Area)]->TopLeftY;
	_float fSizeX_Adjust = _float(g_iWinSizeX) / fWidth;
	_float fSizeY_Adjust = _float(g_iWinSizeY) / fHeight;

	while (!m_queueRender.empty())
	{
		URENDER* pNow = m_queueRender.front();
		m_queueRender.pop();

		if (m_eNow_Area != pNow->eArea)
		{
			m_eNow_Area = pNow->eArea;
			m_pContext->RSSetViewports(1, m_vecViewPort[_int(m_eNow_Area)]);
			 fWidth = m_vecViewPort[_int(m_eNow_Area)]->Width;
			 fHeight = m_vecViewPort[_int(m_eNow_Area)]->Height;
			 fStartX = m_vecViewPort[_int(m_eNow_Area)]->TopLeftX;
			 fStartY = m_vecViewPort[_int(m_eNow_Area)]->TopLeftY;
			 fSizeX_Adjust = _float(g_iWinSizeX) / fWidth;
			 fSizeY_Adjust = _float(g_iWinSizeY) / fHeight;
		}
			
		if (pNow->iTexture >= 0)
		{
			UI_SHADER eShader = UI_SHADER::SHADER_MASTER;
			UI_SHADER_PASS ePass = Select_Shader_Pass(*pNow);

			if (pNow->bIsMultiple)
			{
				if (pNow->vColor_Texture.x < 0) pNow->vColor_Texture.x = 1.f;
				if (pNow->vColor_Texture.y < 0) pNow->vColor_Texture.y = 1.f;
				if (pNow->vColor_Texture.z < 0) pNow->vColor_Texture.z = 1.f;
				if (pNow->vColor_Texture.w < 0) pNow->vColor_Texture.w = 1.f;
			}

			m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

			m_pTransformCom->Set_Scaled(pNow->vSize.x * fSizeX_Adjust, pNow->vSize.y * fSizeY_Adjust, 1.f);

			if (m_eNow_Area == SCROLL_AREA::SCROLL_NONE)
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION,
					XMVectorSet((pNow->vPosition.x) - fWidth * 0.5f, -(pNow->vPosition.y) + fHeight * 0.5f, 0.f, 1.f));
			}
			else 
			{
				pNow->vPosition.x *= fSizeX_Adjust;
				pNow->vPosition.y *= fSizeY_Adjust;

				pNow->vPosition.x -= fStartX;
				pNow->vPosition.y -= fStartY;

				if (m_eNow_Area == SCROLL_AREA::SCROLL_TELEPOT)
				if (KEY_HOLD(KEY::ALT))
				{
					if (KEY_HOLD(KEY::NUM1))
					{
						m_vecViewPort_Adjust[_int(m_eNow_Area)].x += 0.01f;
					}
					if (KEY_HOLD(KEY::NUM2))
					{
						m_vecViewPort_Adjust[_int(m_eNow_Area)].x -= 0.01f;
					}

					if (KEY_HOLD(KEY::NUM3))
					{
						m_vecViewPort_Adjust[_int(m_eNow_Area)].y += 0.01f;
					}
					if (KEY_HOLD(KEY::NUM4))
					{
						m_vecViewPort_Adjust[_int(m_eNow_Area)].y -= 0.01f;
					}

				}

				pNow->vPosition.x += m_vecViewPort_Adjust[_int(m_eNow_Area)].x;
				pNow->vPosition.y += m_vecViewPort_Adjust[_int(m_eNow_Area)].y;
				
					m_pTransformCom->Set_State(CTransform::STATE_POSITION,
						XMVectorSet((pNow->vPosition.x) - fWidth * 0.5f, -(pNow->vPosition.y) + fHeight * 0.5f, 0.f, 1.f));
			}
			
			

			if (pNow->fTurn != 0.f)
				m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(pNow->fTurn));

			if (FAILED(m_pTransformCom->Bind_ShaderResource(m_vecShader_UI[_int(eShader)], "g_WorldMatrix")))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				return E_FAIL;

			if (!pNow->bIsItem)
			{
				if (m_vecTextureInfo_UIPart[pNow->iTexture]->Texture == nullptr)
					if (FAILED(Make_Texture(pNow->iTexture)))
						return E_FAIL;

				if (FAILED(m_vecTextureInfo_UIPart[pNow->iTexture]->Texture->Bind_ShadeResource(m_vecShader_UI[_int(eShader)], "g_Texture", 0)))
					return E_FAIL;
			}
			else
			{
				if (m_vecTextureInfo_ItemIcon[pNow->iTexture]->Texture == nullptr)
					if (FAILED(Make_Texture_Item(pNow->iTexture)))
						return E_FAIL;

				if (FAILED(m_vecTextureInfo_ItemIcon[pNow->iTexture]->Texture->Bind_ShadeResource(m_vecShader_UI[_int(eShader)], "g_Texture", 0)))
					return E_FAIL;
			}

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Multiple", &pNow->bIsMultiple, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Alpha_Strash", &pNow->fAlpha_Strash, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Color", &pNow->vColor_Texture, sizeof(_Vec4))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Render_Area", &pNow->vRange, sizeof(_Vec4))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Render_Angle", &pNow->vAngle, sizeof(_Vec2))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Begin(_int(ePass))))
				return E_FAIL;

			if (pNow->bIs_TwoDPolygon)
			{
				pNow->iTwoPolygon_Buffer_Num = (_int)max(pNow->iTwoPolygon_Buffer_Num, 0);
				pNow->iTwoPolygon_Buffer_Num = (_int)min(pNow->iTwoPolygon_Buffer_Num, m_vecVIBuffer_2DPolygon_Com.size() - 1);

				for (_int i = 0; i < 7; ++i)
				{
					m_vecVIBuffer_2DPolygon_Com[pNow->iTwoPolygon_Buffer_Num]->Set_Point_Ratio(pNow->fRatio_TwoDPolygon[i], i);
				}
				if (FAILED(m_vecVIBuffer_2DPolygon_Com[pNow->iTwoPolygon_Buffer_Num]->Bind_Buffers()))
					return E_FAIL;

				if (FAILED(m_vecVIBuffer_2DPolygon_Com[pNow->iTwoPolygon_Buffer_Num]->Render()))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pVIBufferCom->Bind_Buffers()))
					return E_FAIL;

				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}

		}
		else
		{
			if (pNow->strText.size() == 0)
			{
				Safe_Delete(pNow);
				continue;
			}
				

			_Vec4 vPosition = { pNow->vPosition.x - fStartX, pNow->vPosition.y - fStartY, 0.f,0.f };
			_Vec4 vColor = { 1.f,1.f,1.f,1.f };

			if (pNow->vColor_Text.x >= 0.f)
				vColor.x = pNow->vColor_Text.x;

			if (pNow->vColor_Text.y >= 0.f)
				vColor.y = pNow->vColor_Text.y;

			if (pNow->vColor_Text.z >= 0.f)
				vColor.z = pNow->vColor_Text.z;

			if (pNow->vColor_Text.w >= 0.f)
				vColor.w = pNow->vColor_Text.w;

			_tchar* tText = new _tchar[pNow->strText.size() + 1];
			for (_int i = 0; i <= pNow->strText.size(); ++i)
				tText[i] = pNow->strText[i];

			if (pNow->eText_Type == TEXT_TYPE::TEXT_LEFT)
				m_pGameInstance->Render_Text(m_vecFont_tchar[pNow->iFont], tText, vPosition, vColor);
			else if (pNow->eText_Type == TEXT_TYPE::TEXT_CENTER)
				m_pGameInstance->Render_TextCenter(m_vecFont_tchar[pNow->iFont], tText, vPosition, vColor);
			else 
				m_pGameInstance->Render_TextRight(m_vecFont_tchar[pNow->iFont], tText, vPosition, vColor);

			Safe_Delete_Array(tText);
		}

		Safe_Delete(pNow);
	}


	//m_eNow_Area = SCROLL_AREA::SCROLL_NONE;

	return S_OK;
}


HRESULT CUIRender_Batching::Make_Texture(_int iTextureIndex)
{
	if (iTextureIndex == -1)
		return S_OK;

	if ((iTextureIndex < 0) || (iTextureIndex >= m_vecTextureInfo_UIPart.size()))
	{
		MSG_BOX(TEXT("UIRender : 잘못된 텍스쳐 값"));
		return E_FAIL;
	}


	if (m_vecTextureInfo_UIPart[iTextureIndex]->Texture != nullptr)
		return E_FAIL;

	m_vecTextureInfo_UIPart[iTextureIndex]->Texture = CTexture::Create(m_pDevice, m_pContext, m_vecTextureInfo_UIPart[iTextureIndex]->strTexturePath, 1);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, m_vecTextureInfo_UIPart[iTextureIndex]->strTextureTag, m_vecTextureInfo_UIPart[iTextureIndex]->Texture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRender_Batching::Make_Texture_Item(_int iTextureIndex)
{
	if (iTextureIndex == -1)
		return S_OK;

	if ((iTextureIndex < 0) || (iTextureIndex >= m_vecTextureInfo_ItemIcon.size()))
	{
		MSG_BOX(TEXT("UIRender : 잘못된 텍스쳐 값"));
		return E_FAIL;
	}


	if (m_vecTextureInfo_ItemIcon[iTextureIndex]->Texture != nullptr)
		return E_FAIL;

	m_vecTextureInfo_ItemIcon[iTextureIndex]->Texture = CTexture::Create(m_pDevice, m_pContext, m_vecTextureInfo_ItemIcon[iTextureIndex]->strTexturePath, 1);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, m_vecTextureInfo_ItemIcon[iTextureIndex]->strTextureTag, m_vecTextureInfo_ItemIcon[iTextureIndex]->Texture)))
		return E_FAIL;

	return S_OK;
}

void CUIRender_Batching::Set_Scroll_Area(SCROLL_AREA eArea, _Vec2 vPos, _Vec2 vSize)
{
	m_vecViewPort[_int(eArea)]->Width = vSize.x;
	m_vecViewPort[_int(eArea)]->Height = vSize.y;

	m_vecViewPort[_int(eArea)]->TopLeftX = vPos.x - (vSize.x * 0.5f);
	m_vecViewPort[_int(eArea)]->TopLeftY = vPos.y - (vSize.y * 0.5f);

	m_vecViewPort[_int(eArea)]->MinDepth = 0.f;
	m_vecViewPort[_int(eArea)]->MaxDepth = 1.f;

	m_vecfScrollY_Offset_Max[_int(eArea)] = m_vecViewPort[_int(eArea)]->Height;
}


HRESULT CUIRender_Batching::Ready_Components()
{
	m_vecShader_UI.resize(_int(UI_SHADER::SHADER_END));

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Normal"),
		TEXT("Com_Shader_Normal"), reinterpret_cast<CComponent**>(&m_vecShader_UI[_int(UI_SHADER::SHADER_NORMAL)]))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Change_Color"),
		TEXT("Com_Shader_Change_Color"), reinterpret_cast<CComponent**>(&m_vecShader_UI[_int(UI_SHADER::SHADER_CHANGE_COLOR)]))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Multiple_Color"),
		TEXT("Com_Shader_Multiple_Color"), reinterpret_cast<CComponent**>(&m_vecShader_UI[_int(UI_SHADER::SHADER_MULTIPLE_COLOR)]))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Master"),
		TEXT("Com_Shader_Master"), reinterpret_cast<CComponent**>(&m_vecShader_UI[_int(UI_SHADER::SHADER_MASTER)]))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	m_vecVIBuffer_2DPolygon_Com.resize(3);

	CVIBuffer_2DPolygon::UIPOLIGON_DESC tDesc{};
	tDesc.iPoint = 8;
	tDesc.fAngle = 0.f;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_2DPolygon"),
		TEXT("Com_VIBuffer_2DPolygon"), reinterpret_cast<CComponent**>(&m_vecVIBuffer_2DPolygon_Com[0]), &tDesc)))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_2DPolygon"),
		TEXT("Com_VIBuffer_2DPolygon_LevelUp"), reinterpret_cast<CComponent**>(&m_vecVIBuffer_2DPolygon_Com[1]), &tDesc)))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_2DPolygon"),
		TEXT("Com_VIBuffer_2DPolygon_Origin"), reinterpret_cast<CComponent**>(&m_vecVIBuffer_2DPolygon_Com[2]), &tDesc)))
		return E_FAIL;

	m_vecViewPort.resize(_int(SCROLL_AREA::SCROLL_END));
	for (auto& iter : m_vecViewPort)
	{
		D3D11_VIEWPORT* pNew = new D3D11_VIEWPORT;
		iter = pNew;
	}
	m_vecfScrollY_Offset_Max.resize(_int(SCROLL_AREA::SCROLL_END));
	m_vecViewPort_Adjust.resize(_int(SCROLL_AREA::SCROLL_END));
	
	return S_OK;
}

void CUIRender_Batching::Ready_Font()
{
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

	m_pGameInstance->Add_Font(TEXT("Noto_Normal_10"), TEXT("../Bin/Resources/Fonts/Noto_Nomal_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Medium_10"), TEXT("../Bin/Resources/Fonts/Noto_Medium_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Black_10"), TEXT("../Bin/Resources/Fonts/Noto_Black_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_ExtraLight_10"), TEXT("../Bin/Resources/Fonts/Noto_ExtraLight_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Light_10"), TEXT("../Bin/Resources/Fonts/Noto_Light_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Semiblod_10"), TEXT("../Bin/Resources/Fonts/Noto_SemiBlod_10.spritefont"));

	m_vecFont_tchar[_int(UI_FONT::NOTO_NORMAL_10)] = TEXT("Noto_Normal_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_MEDIUM_10)] = TEXT("Noto_Medium_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_BLACK_10)] = TEXT("Noto_Black_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_EXTRALIGHT_10)] = TEXT("Noto_ExtraLight_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_LIGHT_10)] = TEXT("Noto_Light_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_SEMIBLOD_10)] = TEXT("Noto_Semiblod_10");
}

HRESULT CUIRender_Batching::Ready_Texture_UIPart()
{
	vector<vector<_wstring>> vecBuffer;
	m_pGameInstance->LoadDataByFile("../Bin/Resources/textures/UI/UIList.csv", &vecBuffer);

	for (_int i = 2; i < vecBuffer.size(); ++i)
	{
		UTEXTURE* pNew = new UTEXTURE;

		pNew->strTexturePath = new _tchar[vecBuffer[i][0].size() + 1];
		pNew->strTextureTag = new _tchar[vecBuffer[i][1].size() + 1];

		memcpy(pNew->strTexturePath, &vecBuffer[i][0][0], sizeof(_tchar) * (vecBuffer[i][0].size() + 1));
		memcpy(pNew->strTextureTag, &vecBuffer[i][1][0], sizeof(_tchar) * (vecBuffer[i][1].size() + 1));

		m_vecTextureInfo_UIPart.push_back(pNew);
	}

	return S_OK;
}

HRESULT CUIRender_Batching::Ready_Texture_ItemIcon()
{


	vector<vector<_wstring>> vecBuffer;
	m_pGameInstance->LoadDataByFile("../Bin/Resources/textures/Item/ItemList.csv", &vecBuffer);

	for (_int i = 2; i < vecBuffer.size(); ++i)
	{
		UTEXTURE* pNew = new UTEXTURE;

		pNew->strTexturePath = new _tchar[vecBuffer[i][0].size() + 1];
		pNew->strTextureTag = new _tchar[vecBuffer[i][1].size() + 1];

		memcpy(pNew->strTexturePath, &vecBuffer[i][0][0], sizeof(_tchar) * (vecBuffer[i][0].size() + 1));
		memcpy(pNew->strTextureTag, &vecBuffer[i][1][0], sizeof(_tchar) * (vecBuffer[i][1].size() + 1));

		m_vecTextureInfo_ItemIcon.push_back(pNew);
	}

	return S_OK;
}

void CUIRender_Batching::Ready_Scroll_Adjust_Data()
{
	vector<vector<_wstring>> vecBuffer;
	m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Scroll_Adjust_Data.csv", &vecBuffer);

	for (_int i = 1; i < vecBuffer.size(); ++i)
	{
		m_vecViewPort_Adjust[i - 1] = { stof(vecBuffer[i][1]), stof(vecBuffer[i][2]) };
	}

}

CUIRender_Batching::UI_SHADER_PASS CUIRender_Batching::Select_Shader_Pass(URENDER& Info)
{
	UI_SHADER_PASS eResult = UI_SHADER_PASS::PASS_END;

	_bool bIsMulti = false;
	_bool bIsChange = false;
	_bool bIsAngle = false;
	_bool bIsRange = false;

	if (Info.bIsMultiple)
	{
		bIsMulti = true;
	}
	else
	{
		if (Info.vColor_Texture.x >= 0) bIsChange = true;
		else if (Info.vColor_Texture.y >= 0) bIsChange = true;
		else if (Info.vColor_Texture.z >= 0) bIsChange = true;
		else if (Info.vColor_Texture.w >= 0) bIsChange = true;
	}

	if ((abs(Info.vAngle.x) <= 360.f) && (abs(Info.vAngle.y) <= 360.f))
		bIsAngle = true;

	if ((Info.vRange.x >= 0.f) && (Info.vRange.y >= 0.f))
		if ((Info.vRange.z >= 0.f) && (Info.vRange.w >= 0.f))
			bIsRange = true;

	if (bIsMulti)
	{
		if (bIsAngle)
			eResult = UI_SHADER_PASS::PASS_COLOR_MULTI_ANGLE;
		else if (bIsRange)
			eResult = UI_SHADER_PASS::PASS_COLOR_MULTI_RANGE;
		else 
			eResult = UI_SHADER_PASS::PASS_COLOR_MULTI;
	}
	else if (bIsChange)
	{
		if (bIsAngle)
			eResult = UI_SHADER_PASS::PASS_COLOR_INPUT_ANGLE;
		else if (bIsRange)
			eResult = UI_SHADER_PASS::PASS_COLOR_INPUT_RANGE;
		else
			eResult = UI_SHADER_PASS::PASS_COLOR_INPUT;
	}
	else if (bIsAngle)
		eResult = UI_SHADER_PASS::PASS_ANGLE;
	else if (bIsRange)
		eResult = UI_SHADER_PASS::PASS_RANGE;

	if (Info.bIsAlpha_Adjust == true)
		eResult = UI_SHADER_PASS::PASS_COLOR_MULTI_ALPHA_ADJUST;
	
	return eResult;
}


CUIRender_Batching* CUIRender_Batching::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIRender_Batching* pInstance = new CUIRender_Batching(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIRender_Batching"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUIRender_Batching::Clone(void* pArg)
{
	CUIRender_Batching* pInstance = new CUIRender_Batching(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIRender_Batching"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIRender_Batching::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pVIBufferCom_FullScreen);
	//Safe_Release(m_pVIBufferCom_Scroll);

	for (auto& iter : m_vecTextureInfo_UIPart)
	{
		//		Safe_Release(iter->Texture);
		Safe_Delete_Array(iter->strTexturePath);
		Safe_Delete_Array(iter->strTextureTag);
		Safe_Delete(iter);
	}

	m_vecTextureInfo_UIPart.clear();

	for (auto& iter : m_vecTextureInfo_ItemIcon)
	{
		//		Safe_Release(iter->Texture);
		Safe_Delete_Array(iter->strTexturePath);
		Safe_Delete_Array(iter->strTextureTag);
		Safe_Delete(iter);
	}

	for (auto& iter : m_vecShader_UI)
		Safe_Release(iter);

	m_vecShader_UI.clear();

	m_vecTextureInfo_ItemIcon.clear();

	m_vecFont_tchar.clear();

	while (!m_queueRender.empty())
	{
		Safe_Delete(m_queueRender.front());
		m_queueRender.pop();
	}

	for (auto& iter : m_vecViewPort)
		Safe_Delete(iter);

	m_vecViewPort.clear();

	m_vecfScrollY_Offset_Max.clear();

	for (auto& iter : m_vecVIBuffer_2DPolygon_Com)
		Safe_Release(iter);

	m_vecVIBuffer_2DPolygon_Com.clear();

	m_vecViewPort_Adjust.clear();
}
