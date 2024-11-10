#include "stdafx.h"
#include "..\Public\UIRender_Client.h"

#include "GameInstance.h"


CUIRender_Client::CUIRender_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
	
}

CUIRender_Client::CUIRender_Client(const CUIRender_Client& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIRender_Client::Initialize_Prototype()
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

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRender_Client::Initialize(void* pArg)
{
	return S_OK;
}

void CUIRender_Client::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIRender_Client::Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIRender_Client::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

}

HRESULT CUIRender_Client::Render()
{
	
	return S_OK;
}

HRESULT CUIRender_Client::Render_UI(vector<CUIPage*>& rPage)
{
	for (auto& iter : rPage)
	{
		if (!iter->GetRender())
			continue;

		for (auto& iterPart : iter->GetPartInfo())
		{
			if (iterPart->iTexture_Index != -1)
			{
				if (m_vecTextureInfo[iterPart->iTexture_Index]->Texture == nullptr)
					if (FAILED(Make_Texture(iterPart->iTexture_Index)))
						return E_FAIL;

				m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

				if (iterPart->iMoveType == _int(MOVETYPE::TYPE_BAR))
					m_pTransformCom->Set_Scaled(iterPart->GetBarSize().x, iterPart->GetBarSize().y, 1.f);
				else
					m_pTransformCom->Set_Scaled(iterPart->fSize.x, iterPart->fSize.y, 1.f);

				m_pTransformCom->Set_State(CTransform::STATE_POSITION,
					XMVectorSet(iterPart->fPosition.x - m_fViewWidth * 0.5f, -iterPart->fPosition.y + m_fViewHeight * 0.5f, 0.f, 1.f));

				if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
					return E_FAIL;

				if (FAILED(m_vecTextureInfo[iterPart->iTexture_Index]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &iterPart->fTextureColor, sizeof(_float4))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;

				if (FAILED(m_pVIBufferCom->Bind_Buffers()))
					return E_FAIL;

				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
			if ((iterPart->iFontIndex >= 0) && (iterPart->iFontIndex < _int(UI_FONT::FONT_END)))
			{
				if (iterPart->strText.size() == 0)
					continue;

				_vector vPosition = { iterPart->fPosition.x, iterPart->fPosition.y, 0.f,0.f };
				_vector vColor = { 1.f,1.f,1.f,1.f };

				if (iterPart->fTextColor.x > 0.f)
					vColor.m128_f32[0] = iterPart->fTextColor.x;

				if (iterPart->fTextColor.y > 0.f)
					vColor.m128_f32[1] = iterPart->fTextColor.y;

				if (iterPart->fTextColor.z > 0.f)
					vColor.m128_f32[2] = iterPart->fTextColor.z;

				if (iterPart->fTextColor.w > 0.f)
					vColor.m128_f32[3] = iterPart->fTextColor.w;

				_tchar* tText = new _tchar[iterPart->strText.size() + 1];
				for (_int i = 0; i <= iterPart->strText.size(); ++i)
					tText[i] = iterPart->strText[i];


				//memcpy(tText, &iterPart->strText, sizeof(_tchar) * (iterPart->strText.size() + 1));

				if (iterPart->bCenter)
					m_pGameInstance->Render_TextCenter(m_vecFont_tchar[iterPart->iFontIndex], tText, vPosition, vColor);
				else
					m_pGameInstance->Render_Text(m_vecFont_tchar[iterPart->iFontIndex], tText, vPosition, vColor);

				Safe_Delete_Array(tText);
			}
		}
	}



	return S_OK;
}

HRESULT CUIRender_Client::Make_Texture(_int iTextureIndex)
{
	if ((iTextureIndex < 0) || (iTextureIndex >= m_vecTextureInfo.size()))
	{
		MSG_BOX(TEXT("UIRender : 잘못된 텍스쳐 값"));
		return E_FAIL;
	}
		

	if (m_vecTextureInfo[iTextureIndex]->Texture != nullptr)
		return E_FAIL;

	m_vecTextureInfo[iTextureIndex]->Texture = CTexture::Create(m_pDevice, m_pContext, m_vecTextureInfo[iTextureIndex]->strTexturePath, 1);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, m_vecTextureInfo[iTextureIndex]->strTextureTag, m_vecTextureInfo[iTextureIndex]->Texture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRender_Client::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* FOR.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_UIRender_Client"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CUIRender_Client::Ready_Font()
{

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

HRESULT CUIRender_Client::Ready_Texture()
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

		m_vecTextureInfo.push_back(pNew);
	}

	return S_OK;
}

CUIRender_Client* CUIRender_Client::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIRender_Client* pInstance = new CUIRender_Client(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIRender_Client"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUIRender_Client::Clone(void* pArg)
{
	CUIRender_Client* pInstance = new CUIRender_Client(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIRender_Client"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIRender_Client::Free()
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

	m_vecFont_tchar.clear();

	Safe_Release(m_pVIBufferCom);
}
