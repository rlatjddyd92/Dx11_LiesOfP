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
		if (FAILED(BackRender(iBack)))
			return E_FAIL;

	_int iMax = CController_UITool::Get_Instance()->GetPartCount();

	for (_int i = 0; i < iMax; ++i)
	{
		CController_UITool::UPART& rNow = CController_UITool::Get_Instance()->Get_PartRenderInfo(i);

		if (rNow.iTexture_Index != -1)
		{
			UI_SHADER eShader = UI_SHADER::SHADER_MASTER;
			UI_SHADER_PASS ePass = UI_SHADER_PASS::PASS_BASIC;

			if (rNow.bTexture_Color_Multiple)
			{
				ePass = UI_SHADER_PASS::PASS_COLOR_MULTI;
				if (rNow.fTextureColor.x < 0) rNow.fTextureColor.x = 1.f;
				if (rNow.fTextureColor.y < 0) rNow.fTextureColor.y = 1.f;
				if (rNow.fTextureColor.z < 0) rNow.fTextureColor.z = 1.f;
				if (rNow.fTextureColor.w < 0) rNow.fTextureColor.w = 1.f;

				if ((abs(rNow.vTexture_Angle.x) <= 360.f) && (abs(rNow.vTexture_Angle.y) <= 360.f))
					ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_ANGLE;
				else if ((rNow.vTexture_Range.x >= 0.f) && (rNow.vTexture_Range.y >= 0.f))
					if ((rNow.vTexture_Range.z >= 0.f) && (rNow.vTexture_Range.w >= 0.f))
						ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_RANGE;
			}
			else
			{
				if (rNow.fTextureColor.x >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;
				else if (rNow.fTextureColor.y >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;
				else if (rNow.fTextureColor.z >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;
				else if (rNow.fTextureColor.w >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;

				if (ePass == UI_SHADER_PASS::PASS_COLOR_INPUT)
				{
					if ((abs(rNow.vTexture_Angle.x) <= 360.f) && (abs(rNow.vTexture_Angle.y) <= 360.f))
						ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_ANGLE;
					else if ((rNow.vTexture_Range.x >= 0.f) && (rNow.vTexture_Range.y >= 0.f))
						if ((rNow.vTexture_Range.z >= 0.f) && (rNow.vTexture_Range.w >= 0.f))
							ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_RANGE;
				}
				else
				{
					if ((abs(rNow.vTexture_Angle.x) <= 360.f) && (abs(rNow.vTexture_Angle.y) <= 360.f))
						ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_ANGLE;
					else if ((rNow.vTexture_Range.x >= 0.f) && (rNow.vTexture_Range.y >= 0.f))
						if ((rNow.vTexture_Range.z >= 0.f) && (rNow.vTexture_Range.w >= 0.f))
							ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_RANGE;
				}
			}

			

			m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

			if (rNow.iMoveType == _int(CController_UITool::MOVETYPE::TYPE_BAR))
				m_pTransformCom->Set_Scaled(rNow.GetBarSize().x, rNow.GetBarSize().y, 1.f);
			else
				m_pTransformCom->Set_Scaled(rNow.fSize.x, rNow.fSize.y, 1.f);



			m_pTransformCom->Set_State(CTransform::STATE_POSITION,
				XMVectorSet(rNow.fPosition.x - m_fViewWidth * 0.5f, -rNow.fPosition.y + m_fViewHeight * 0.5f, 0.f, 1.f));

			if (rNow.bTurn)
				m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(rNow.fTurn_Degree));

			if (FAILED(m_pTransformCom->Bind_ShaderResource(m_vecShader_UI[_int(eShader)], "g_WorldMatrix")))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				return E_FAIL;

			if (!rNow.bIsItem)
			{
				if (rNow.iTexture_Index >= m_vecTextureInfo.size())
					return S_OK;

				if (FAILED(m_vecTextureInfo[rNow.iTexture_Index]->Texture->Bind_ShadeResource(m_vecShader_UI[_int(eShader)], "g_Texture", 0)))
					return E_FAIL;
			}
			else
			{
				if (rNow.iTexture_Index >= m_vecTextureInfo_ItemIcon.size())
					return S_OK;

				if (FAILED(m_vecTextureInfo_ItemIcon[rNow.iTexture_Index]->Texture->Bind_ShadeResource(m_vecShader_UI[_int(eShader)], "g_Texture", 0)))
					return E_FAIL;
			}

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Multiple", &rNow.bTexture_Color_Multiple, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Alpha_Strash", &rNow.fStrash_Alpha, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Color", &rNow.fTextureColor, sizeof(_float4))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Render_Area", &rNow.vTexture_Range, sizeof(_Vec4))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Render_Angle", &rNow.vTexture_Angle, sizeof(_Vec2))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Begin(_int(ePass))))
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

			if (KEY_HOLD(KEY::ALT))
			{
				if (KEY_HOLD(KEY::NUM1))
					m_fSize -= 0.01f;
				else if (KEY_HOLD(KEY::NUM2))
					m_fSize += 0.01f;

				m_fSize = max(m_fSize, 0.f);
				m_fSize = min(m_fSize, 1.f);
			}


			if (rNow.bCenter)
				m_pGameInstance->Render_TextCenter(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor, 0.f, {0.f,0.f,0.f,1.f}, m_fSize);
			else if (rNow.bText_Right)
				m_pGameInstance->Render_TextRight(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor, 0.f, { 0.f,0.f,0.f,1.f }, m_fSize);
			else
				m_pGameInstance->Render_Text(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor, 0.f, { 0.f,0.f,0.f,1.f }, m_fSize);
		}
	}

	return S_OK;
}

void CUIRender::AddRenderUIObject(_int iTextureIndex, _float2 fPosition, _float2 fSize, _float3 fRGB, _float fAlpha, _float4 vRange, _float2 vAngle)
{
	URCOM* pNew = new URCOM;

	pNew->iTextureIndex = iTextureIndex;
	pNew->fPosition = fPosition;
	pNew->fSize = fSize;
	pNew->fRGB = fRGB;
	pNew->fAlpah = fAlpha;
	pNew->vRange = vRange;
	pNew->vAngle = vAngle;

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

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_vecShader_UI[_int(UI_SHADER::SHADER_NORMAL)], "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_vecShader_UI[_int(UI_SHADER::SHADER_NORMAL)]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_vecShader_UI[_int(UI_SHADER::SHADER_NORMAL)]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_vecTextureInfo[iIndex]->Texture->Bind_ShadeResource(m_vecShader_UI[_int(UI_SHADER::SHADER_NORMAL)], "g_Texture", 0)))
		return E_FAIL;

	_float4 fColor = { -1,-1,-1,-1 };

	if (FAILED(m_vecShader_UI[_int(UI_SHADER::SHADER_NORMAL)]->Bind_RawValue("g_Color", &fColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_vecShader_UI[_int(UI_SHADER::SHADER_NORMAL)]->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUIRender::Render_Effect_Tool()
{
	_int iIndex = -1;

	while (1)
	{
		++iIndex;

		CController_UITool::UPART* pNow = CController_UITool::Get_Instance()->Get_PartRenderInfo_Effect(iIndex);

		if (pNow == nullptr)
			return S_OK;

		if (pNow->bRender = false)
			continue;

		CController_UITool::UPART& rNow = *pNow;
		
		if (rNow.iTexture_Index != -1)
		{
			UI_SHADER eShader = UI_SHADER::SHADER_MASTER;
			UI_SHADER_PASS ePass = UI_SHADER_PASS::PASS_BASIC;

			if (rNow.bTexture_Color_Multiple)
			{
				ePass = UI_SHADER_PASS::PASS_COLOR_MULTI;
				if (rNow.fTextureColor.x < 0) rNow.fTextureColor.x = 1.f;
				if (rNow.fTextureColor.y < 0) rNow.fTextureColor.y = 1.f;
				if (rNow.fTextureColor.z < 0) rNow.fTextureColor.z = 1.f;
				if (rNow.fTextureColor.w < 0) rNow.fTextureColor.w = 1.f;

				if ((abs(rNow.vTexture_Angle.x) <= 180.f) && (abs(rNow.vTexture_Angle.y) > 180.f))
					ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_ANGLE;
				else if ((rNow.vTexture_Range.x >= 0.f) && (rNow.vTexture_Range.y >= 0.f))
					if ((rNow.vTexture_Range.z >= 0.f) && (rNow.vTexture_Range.w >= 0.f))
						ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_RANGE;
			}
			else
			{
				if (rNow.fTextureColor.x >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;
				else if (rNow.fTextureColor.y >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;
				else if (rNow.fTextureColor.z >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;
				else if (rNow.fTextureColor.w >= 0) ePass = UI_SHADER_PASS::PASS_COLOR_INPUT;

				if (ePass == UI_SHADER_PASS::PASS_COLOR_INPUT)
				{
					if ((abs(rNow.vTexture_Angle.x) <= 180.f) && (abs(rNow.vTexture_Angle.y) > 180.f))
						ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_ANGLE;
					else if ((rNow.vTexture_Range.x >= 0.f) && (rNow.vTexture_Range.y >= 0.f))
						if ((rNow.vTexture_Range.z >= 0.f) && (rNow.vTexture_Range.w >= 0.f))
							ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_RANGE;
				}
				else
				{
					if ((abs(rNow.vTexture_Angle.x) <= 180.f) && (abs(rNow.vTexture_Angle.y) > 180.f))
						ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_ANGLE;
					else if ((rNow.vTexture_Range.x >= 0.f) && (rNow.vTexture_Range.y >= 0.f))
						if ((rNow.vTexture_Range.z >= 0.f) && (rNow.vTexture_Range.w >= 0.f))
							ePass = UI_SHADER_PASS::PASS_COLOR_MULTI_RANGE;
				}
			}



			m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

			if (rNow.iMoveType == _int(CController_UITool::MOVETYPE::TYPE_BAR))
				m_pTransformCom->Set_Scaled(rNow.GetBarSize().x, rNow.GetBarSize().y, 1.f);
			else
				m_pTransformCom->Set_Scaled(rNow.fSize.x, rNow.fSize.y, 1.f);



			m_pTransformCom->Set_State(CTransform::STATE_POSITION,
				XMVectorSet(rNow.fPosition.x - m_fViewWidth * 0.5f, -rNow.fPosition.y + m_fViewHeight * 0.5f, 0.f, 1.f));

			if (rNow.bTurn)
				m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(rNow.fTurn_Degree));

			if (FAILED(m_pTransformCom->Bind_ShaderResource(m_vecShader_UI[_int(eShader)], "g_WorldMatrix")))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
				return E_FAIL;

			if (!rNow.bIsItem)
			{
				if (rNow.iTexture_Index >= m_vecTextureInfo.size())
					return S_OK;

				if (FAILED(m_vecTextureInfo[rNow.iTexture_Index]->Texture->Bind_ShadeResource(m_vecShader_UI[_int(eShader)], "g_Texture", 0)))
					return E_FAIL;
			}
			else
			{
				if (rNow.iTexture_Index >= m_vecTextureInfo_ItemIcon.size())
					return S_OK;

				if (FAILED(m_vecTextureInfo_ItemIcon[rNow.iTexture_Index]->Texture->Bind_ShadeResource(m_vecShader_UI[_int(eShader)], "g_Texture", 0)))
					return E_FAIL;
			}

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Multiple", &rNow.bTexture_Color_Multiple, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Alpha_Strash", &rNow.fStrash_Alpha, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Color", &rNow.fTextureColor, sizeof(_float4))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Render_Area", &rNow.vTexture_Range, sizeof(_Vec4))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Bind_RawValue("g_Render_Angle", &rNow.vTexture_Angle, sizeof(_Vec2))))
				return E_FAIL;

			if (FAILED(m_vecShader_UI[_int(eShader)]->Begin(_int(ePass))))
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

			if (KEY_HOLD(KEY::ALT))
			{
				if (KEY_HOLD(KEY::NUM1))
					m_fSize -= 0.01f;
				else if (KEY_HOLD(KEY::NUM2))
					m_fSize += 0.01f;

				m_fSize = max(m_fSize, 0.f);
				m_fSize = min(m_fSize, 1.f);
			}


			if (rNow.bCenter)
				m_pGameInstance->Render_TextCenter(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor, 0.f, { 0.f,0.f,0.f,1.f }, m_fSize);
			else if (rNow.bText_Right)
				m_pGameInstance->Render_TextRight(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor, 0.f, { 0.f,0.f,0.f,1.f }, m_fSize);
			else
				m_pGameInstance->Render_Text(m_vecFont_tchar[rNow.iFontIndex], rNow.szText, vPosition, vColor, 0.f, { 0.f,0.f,0.f,1.f }, m_fSize);
		}
	} 
	



	return S_OK;
}


HRESULT CUIRender::Ready_Components()
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

	m_vecFont_char[_int(UI_FONT::FONT_NORMAL_8)] = "Font_Normal_8";
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


	m_pGameInstance->Add_Font(TEXT("Noto_Normal_10"), TEXT("../Bin/Resources/Fonts/Noto_Nomal_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Medium_10"), TEXT("../Bin/Resources/Fonts/Noto_Medium_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Black_10"), TEXT("../Bin/Resources/Fonts/Noto_Black_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_ExtraLight_10"), TEXT("../Bin/Resources/Fonts/Noto_ExtraLight_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Light_10"), TEXT("../Bin/Resources/Fonts/Noto_Light_10.spritefont"));
	m_pGameInstance->Add_Font(TEXT("Noto_Semiblod_10"), TEXT("../Bin/Resources/Fonts/Noto_SemiBlod_10.spritefont"));

	m_vecFont_char[_int(UI_FONT::NOTO_NORMAL_10)] = "Noto_Normal_10";
	m_vecFont_char[_int(UI_FONT::NOTO_MEDIUM_10)] = "Noto_Medium_10";
	m_vecFont_char[_int(UI_FONT::NOTO_BLACK_10)] = "Noto_Black_10";
	m_vecFont_char[_int(UI_FONT::NOTO_EXTRALIGHT_10)] = "Noto_ExtraLight_10";
	m_vecFont_char[_int(UI_FONT::NOTO_LIGHT_10)] = "Noto_Light_10";
	m_vecFont_char[_int(UI_FONT::NOTO_SEMIBLOD_10)] = "Noto_Semiblod_10";

	m_vecFont_tchar[_int(UI_FONT::NOTO_NORMAL_10)] = TEXT("Noto_Normal_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_MEDIUM_10)] = TEXT("Noto_Medium_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_BLACK_10)] = TEXT("Noto_Black_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_EXTRALIGHT_10)] = TEXT("Noto_ExtraLight_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_LIGHT_10)] = TEXT("Noto_Light_10");
	m_vecFont_tchar[_int(UI_FONT::NOTO_SEMIBLOD_10)] = TEXT("Noto_Semiblod_10");
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

CUIRender::UI_SHADER_PASS CUIRender::Select_Shader(URCOM& Command)
{
	UI_SHADER_PASS eReturn = UI_SHADER_PASS::PASS_BASIC;

	


	





	return eReturn;
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

	for (auto& iter : m_vecShader_UI)
		Safe_Release(iter);

	m_vecShader_UI.clear();


	m_UIRenderlist.clear();

	Safe_Release(m_pVIBufferCom);
}
