#include "stdafx.h"
#include "..\Public\UIRender_Client.h"

#include "GameInstance.h"
#include "UIPage_Ortho.h"

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

	if (FAILED(Ready_Texture_UIPart()))
		return E_FAIL;

	if (FAILED(Ready_Texture_ItemIcon()))
		return E_FAIL;

	m_vecTestPageInfo.resize(_int(TEST_PAGE_NAME::NAME_END));
	m_vecTestPage_Pos.resize(_int(TEST_PAGE_NAME::NAME_END));
	m_vecTestPage_Size.resize(_int(TEST_PAGE_NAME::NAME_END));

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

HRESULT CUIRender_Client::Render_Ortho(CUIPage_Ortho* pPage_Ortho)
{
	

	return S_OK;
}

HRESULT CUIRender_Client::Render_UI(vector<CUIPage*>& rPage)
{
	for (auto& iter : rPage)
	{
		if (!iter->GetRender())
			continue;

		_bool bTopMove = false;

		if ((iter->GetTopPartMove() > 0.f) && (iter->GetTopPartMove() < 1.f))
			bTopMove = true;

		for (auto& iterPart : iter->GetPartInfo())
		{
			if (!iterPart->bRender)
				continue;

			if (FAILED(Render_Part(*iterPart, *iter, bTopMove)))
				return E_FAIL;
		}
	}


	Render_TestPage_Info();

	return S_OK;
}

HRESULT CUIRender_Client::Make_Texture(_int iTextureIndex)
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

HRESULT CUIRender_Client::Make_Texture_Item(_int iTextureIndex)
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

HRESULT CUIRender_Client::Render_TestFont(_bool bIsKorean)
{
	if (!bIsKorean)
	{
		for (_int i = 0; i < _int(UI_FONT::FONT_END); ++i)
			m_pGameInstance->Render_Text(m_vecFont_tchar[i], TEXT("~1234567890abcdefghijklmnopqrstuvwxyz_ -=.,;:?!@()"), { 10.f,50.f * (i + 1),0.f,0.f }, { 1.f,1.f,1.f,1.f });
	}
	else
	{
		for (_int i = 0; i < _int(UI_FONT::FONT_END); ++i)
			m_pGameInstance->Render_Text(m_vecFont_tchar[i], TEXT("가나다라마바사아자차카타파하값낢밟삯핳쁐뀰팖갊삶"), { 10.f,50.f * (i + 1),0.f,0.f }, { 1.f,1.f,1.f,1.f });
	}
	
	return S_OK;
}

void CUIRender_Client::Input_TestPageInfo(
	TEST_PAGE_NAME eName,
	_float2 fPosition,
	_float2 fSize,
	vector<_wstring>& vecName,
	vector<_wstring>& vecValue)
{
	_int iFunc = _int(eName);

	m_vecTestPage_Pos[iFunc] = fPosition;
	m_vecTestPage_Size[iFunc] = fSize;

	for (_int i = 0; i < vecName.size(); ++i)
	{
		_tchar* pName = new _tchar[vecName[i].size() + 1];
		memcpy(pName, vecName[i].c_str(), sizeof(_tchar) * (vecName[i].size() + 1));

		_tchar* pValue = new _tchar[vecValue[i].size() + 1];
		memcpy(pValue, vecValue[i].c_str(), sizeof(_tchar) * (vecValue[i].size() + 1));
		
		
		m_vecTestPageInfo[iFunc].push_back({ pName, pValue });
	}
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

HRESULT CUIRender_Client::Render_Part(CUIPage::UPART& pPart, CUIPage& pPage, _bool bTopMove)
{
	if (pPart.iTexture_Index != -1)
	{
		if (!pPart.bIsItem)
		{
			if (m_vecTextureInfo_UIPart[pPart.iTexture_Index]->Texture == nullptr)
				if (FAILED(Make_Texture(pPart.iTexture_Index)))
					return E_FAIL;
		}
		else
		{
			if (m_vecTextureInfo_ItemIcon[pPart.iTexture_Index]->Texture == nullptr)
				if (FAILED(Make_Texture_Item(pPart.iTexture_Index)))
					return E_FAIL;
		}

		m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

		if (pPart.iMoveType == _int(MOVETYPE::TYPE_BAR))
			m_pTransformCom->Set_Scaled(pPart.GetBarSize().x, pPart.GetBarSize().y, 1.f);
		else
			m_pTransformCom->Set_Scaled(pPart.fSize.x, pPart.fSize.y, 1.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(pPart.fPosition.x - m_fViewWidth * 0.5f, -pPart.fPosition.y + m_fViewHeight * 0.5f, 0.f, 1.f));

		if (pPart.bTurn)
			m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(pPart.fTurn_Degree));

		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (!pPart.bIsItem)
		{
			if (FAILED(m_vecTextureInfo_UIPart[pPart.iTexture_Index]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_vecTextureInfo_ItemIcon[pPart.iTexture_Index]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;
		}

		if ((bTopMove) && (pPart.iParentPart_Index == -1))
		{
			_float4 fColor = pPart.fTextureColor;
			fColor.w = pPage.GetTopPartMove();

			if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &fColor, sizeof(_float4))))
				return E_FAIL;
		}
		else if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &pPart.fTextureColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	if ((pPart.iFontIndex >= 0) && (pPart.iFontIndex < _int(UI_FONT::FONT_END)))
	{
		if (pPart.strText.size() == 0)
			return S_OK;

		_vector vPosition = { pPart.fPosition.x, pPart.fPosition.y, 0.f,0.f };
		_vector vColor = { 1.f,1.f,1.f,1.f };

		if (pPart.fTextColor.x > 0.f)
			vColor.m128_f32[0] = pPart.fTextColor.x;

		if (pPart.fTextColor.y > 0.f)
			vColor.m128_f32[1] = pPart.fTextColor.y;

		if (pPart.fTextColor.z > 0.f)
			vColor.m128_f32[2] = pPart.fTextColor.z;

		if (pPart.fTextColor.w > 0.f)
			vColor.m128_f32[3] = pPart.fTextColor.w;

		_tchar* tText = new _tchar[pPart.strText.size() + 1];
		for (_int i = 0; i <= pPart.strText.size(); ++i)
			tText[i] = pPart.strText[i];


		//memcpy(tText, &pPage.strText, sizeof(_tchar) * (pPage.strText.size() + 1));

		if (pPart.bCenter)
			m_pGameInstance->Render_TextCenter(m_vecFont_tchar[pPart.iFontIndex], tText, vPosition, vColor);
		else
			m_pGameInstance->Render_Text(m_vecFont_tchar[pPart.iFontIndex], tText, vPosition, vColor);

		Safe_Delete_Array(tText);
	}

	return S_OK;
}

void CUIRender_Client::Ready_Font()
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


	

}

HRESULT CUIRender_Client::Ready_Texture_UIPart()
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

HRESULT CUIRender_Client::Ready_Texture_ItemIcon()
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

HRESULT CUIRender_Client::Render_TestPage_Info()
{
	for (_int i = 0; i < _int(TEST_PAGE_NAME::NAME_END); ++i)
	{
		if (m_vecTestPageInfo[i].empty())
			continue;

		_float fX = m_vecTestPage_Pos[i].x - (m_vecTestPage_Size[i].x * 0.5f) + 10.f;
		_float fY = m_vecTestPage_Pos[i].y - (m_vecTestPage_Size[i].y * 0.5f) + 10.f;
		_float fCol_Interval = 200.f;
		_float fRow_Interval = 25.f;

		if (m_vecTextureInfo_UIPart[69]->Texture == nullptr)
			if (FAILED(Make_Texture(69)))
				return E_FAIL;

		m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

		m_pTransformCom->Set_Scaled(m_vecTestPage_Size[i].x, m_vecTestPage_Size[i].y, 1.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(m_vecTestPage_Pos[i].x - m_fViewWidth * 0.5f, -m_vecTestPage_Pos[i].y + m_fViewHeight * 0.5f, 0.f, 1.f));

		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_vecTextureInfo_UIPart[69]->Texture->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		_float4 fColor = { -1.f,-1.f,-1.f,-1.f };

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &fColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		switch (i)
		{
		case _int(TEST_PAGE_NAME::NAME_TEAMJANG):
			m_pGameInstance->Render_Text(m_vecFont_tchar[7], TEXT("[팀장용 페이지] 열기/닫기 : LShift + F3, 창의 아무데나 눌러서 드래그하면 창 이동"), { fX,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			break;
		case _int(TEST_PAGE_NAME::NAME_ANIM):
			m_pGameInstance->Render_Text(m_vecFont_tchar[7], TEXT("[애니메이션 페이지] 열기/닫기 : LShift + F4, 창의 아무데나 눌러서 드래그하면 창 이동"), { fX,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			break;
		case _int(TEST_PAGE_NAME::NAME_EFFECT):
			m_pGameInstance->Render_Text(m_vecFont_tchar[7], TEXT("[이펙트 페이지] 열기/닫기 : LShift + F5, 창의 아무데나 눌러서 드래그하면 창 이동"), { fX,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			break;
		case _int(TEST_PAGE_NAME::NAME_MAP):
			m_pGameInstance->Render_Text(m_vecFont_tchar[7], TEXT("[맵 페이지] 열기/닫기 : LShift + F6, 창의 아무데나 눌러서 드래그하면 창 이동"), { fX,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			break;
		case _int(TEST_PAGE_NAME::NAME_PLAYER_STAT):
			m_pGameInstance->Render_Text(m_vecFont_tchar[7], TEXT("[스탯 페이지] 열기/닫기 : LShift + F7, 창의 아무데나 눌러서 드래그하면 창 이동"), { fX,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			break;
		case _int(TEST_PAGE_NAME::NAME_ITEM):
			m_pGameInstance->Render_Text(m_vecFont_tchar[7], TEXT("[아이템 페이지] 열기/닫기 : LShift + F8, 창의 아무데나 눌러서 드래그하면 창 이동"), { fX,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			break;
		default:
			break;
		}




		
		fY += fRow_Interval;

		for (auto& iter : m_vecTestPageInfo[i])
		{
			m_pGameInstance->Render_Text(m_vecFont_tchar[0], iter[0], { fX,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			m_pGameInstance->Render_Text(m_vecFont_tchar[0], iter[1], { fX + fCol_Interval,fY,0.f,0.f }, { 1.f,1.f,1.f,1.f });
			fY += fRow_Interval;
		}

		RemoveTestPageInfo(TEST_PAGE_NAME(i));
	}

	

	return S_OK;
}

void CUIRender_Client::RemoveTestPageInfo(TEST_PAGE_NAME eFunction)
{
	for (auto& iter : m_vecTestPageInfo[_int(eFunction)])
	{
		for (auto& iterText : iter)
			Safe_Delete_Array(iterText);

		iter.clear();
	}

	m_vecTestPageInfo[_int(eFunction)].clear();
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

	m_vecTextureInfo_ItemIcon.clear();

	m_vecFont_tchar.clear();

	for (_int i = 0; i < _int(TEST_PAGE_NAME::NAME_END); ++i)
		RemoveTestPageInfo(TEST_PAGE_NAME(i));

	m_vecTestPageInfo.clear();
	m_vecTestPage_Pos.clear();
	m_vecTestPage_Size.clear();
	Safe_Release(m_pVIBufferCom);
}
