#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Telepot : public CUIPage
{
public:
	enum class PART_GROUP
	{
		TELEPOT_Back,
		TELEPOT_Fx_Cloud,
		TELEPOT_Fx_Grid,
		TELEPOT_List_Area,
		TELEPOT_Scroll_Line,
		TELEPOT_Scroll_Bar,
		TELEPOT_Dest_Frame,
		TELEPOT_Dest_Deco_LT,
		TELEPOT_Dest_Deco_Middle,
		TELEPOT_Dest_Deco_RB,
		TELEPOT_Dest_Picture_Start,
		TELEPOT_Dest_Picture_Rax,
		TELEPOT_Dest_Picture_Stair,
		TELEPOT_Dest_Picture_Manus,
		TELEPOT_Dest_Fx_Select,
		TELEPOT_Dest_Text_Now,
		TELEPOT_Dest_Text_Inactive,
		TELEPOT_Dest_Text_Name,
		GROUP_END
	};




protected:
	CUIPage_Telepot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Telepot(const CUIPage_Telepot& Prototype);
	virtual ~CUIPage_Telepot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OpenAction() override;
	virtual void CloseAction() override;

	virtual CHECK_MOUSE Check_Page_Action(_float fTimeDelta) override;

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

protected:
	_Vec4 m_vDestination_Now = { -1.f,-1.f,-1.f,-1.f };

	_Vec4 m_vDestination_Origin[4] =
	{
		{ -1.f,-1.f,-1.f,-1.f }, // 모나스트리 입구
		{ -1.f,-1.f,-1.f,-1.f }, // 락사시아 보스전 
		{ -1.f,-1.f,-1.f,-1.f }, // 원형계단
		{ -1.f,-1.f,-1.f,-1.f } // 마누스 보스전
	};

	_wstring m_strDest_Name[4] =
	{
		TEXT("아르케 대수도원 입구"),
		TEXT("전투 : 락사시아"),
		TEXT("대수도원 원형계단"),
		TEXT("전투 : 마누스")
	};

	SCROLL* m_pScroll_Telepot = { nullptr };






public:
	static CUIPage_Telepot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END