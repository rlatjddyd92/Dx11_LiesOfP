#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Play : public CUIPage
{
public:
	enum class PART_GROUP
	{
		// 좌상단 게이지 
		GROUP_HP_STATIC,
		GROUP_HP_FRAME,
		GROUP_HP_FILL,
		GROUP_ST_STATIC,
		GROUP_ST_FRAME,
		GROUP_ST_FILL,
		GROUP_SP0_FRAME,
		GROUP_SP1_FRAME,
		GROUP_SP2_FRAME,
		GROUP_SP3_FRAME,
		GROUP_SP4_FRAME,
		GROUP_SP0_FILL,
		GROUP_SP1_FILL,
		GROUP_SP2_FILL,
		GROUP_SP3_FILL,
		GROUP_SP4_FILL,

		// 좌하단 아이템
		GROUP_POTION_ITEM, // <- 아이템 텍스쳐
		GROUP_POTION_TOOL_RKEY, // <- R키 (사용 가능할 때 표시)
		GROUP_POTION_FILL, // <- 포션 게이지 (사용 가능 포션이 0일 때 표시)
		GROUP_POTION_COUNT,
		GROUP_POTION_QUEUE_BACK,
		GROUP_POTION_QUEUE_ITEM,
		
		GROUP_TOOL_ITEM,
		GROUP_TOOL_COUNT,
		GROUP_TOOL_QUEUE_BACK,
		GROUP_TOOL_QUEUE_ITEM,

		GROUP_SELECT_CELL, // 선택 표시

		// 좌하단 보조가방
		GROUP_BAG_FRAMELINE,
		GROUP_BAG_BACK,
		GROUP_BAG_ITEM,
		GROUP_BAG_COUNT,
		GROUP_BAG_NUM,

		// 좌하단 암
		GROUP_ARM_BACK,
		GROUP_ARM_TEX,
		GROUP_ARM_CTRL,
		GROUP_ARM_NAME,
		GROUP_ARM_GAUGE_FRAME,
		GROUP_ARM_GAUGE_FILL,

		// 우상단 코인 
		GROUP_COIN_BACK,
		GROUP_COIN_COUNT,

		// 우하단 무기
		GROUP_WEAPON_CENTER, // 무기창 전체의 중심
		GROUP_WEAPON_DURABLE_FRAME,
		GROUP_WEAPON_DURABLE_FILL,
		GROUP_WEAPON_NORMAL_BACK, // 일반 무기 (날 + 자루) 표시
		GROUP_WEAPON_NORMAL_BACK_FX, // 일반 무기 (날 + 자루) 표시 (특수효과)
		GROUP_WEAPON_NORMAL_BLADE,
		GROUP_WEAPON_NORMAL_HANDLE,
		GROUP_WEAPON_SPECIAL_BACK, // 스페셜 무기 표시
		GROUP_WEAPON_SPECIAL_TEX,
		GROUP_WEAPON_EQUIP_NUM, // 1,2번 장착 표시


		// 특수 공격 게이지 
		// 사이드는 3칸짜리만 사용한다 
		GROUP_WEAPON_GAUGE_LEFT_SYMBOL, // 날 특수공격 아이콘
		GROUP_WEAPON_GAUGE_RIGHT_SYMBOL, // 자루 특수공격 아이콘

		GROUP_WEAPON_GAUGE_LEFT_SIDE_FRAME, // 날 특수 공격 게이지 프레임 (사이드) 
		GROUP_WEAPON_GAUGE_LEFT_SIDE_WHITE, // 날 특수 공격 게이지 바탕 (사이드)
		GROUP_WEAPON_GAUGE_LEFT_SIDE_FILL, // 날 특수 공격 게이지 활성화 표시 (사이드)

		GROUP_WEAPON_GAUGE_LEFT_CENTER_FRAME, // 날 특수 공격 게이지 표시 (중심)
		GROUP_WEAPON_GAUGE_LEFT_CENTER_WHITE, // 날 특수 공격 게이지 바탕 (중심)
		GROUP_WEAPON_GAUGE_LEFT_CENTER_FILL,  // 날 특수 공격 게이지 활성화 표시 (중심)

		GROUP_WEAPON_GAUGE_RIGHT_SIDE_FRAME, // 자루 특수 공격 게이지 프레임 (사이드) 
		GROUP_WEAPON_GAUGE_RIGHT_SIDE_WHITE, // 자루 특수 공격 게이지 바탕 (사이드)
		GROUP_WEAPON_GAUGE_RIGHT_SIDE_FILL, // 자루 특수 공격 게이지 활성화 표시 (사이드)

		GROUP_WEAPON_GAUGE_RIGHT_CENTER_FRAME, // 자루 특수 공격 게이지 표시 (중심)
		GROUP_WEAPON_GAUGE_RIGHT_CENTER_WHITE, // 자루 특수 공격 게이지 바탕 (중심)
		GROUP_WEAPON_GAUGE_RIGHT_CENTER_FILL, // 자루 특수 공격 게이지 활성화 표시 (중심)

		GROUP_WEAPON_GAUGE_LEFT_KEYSET_A, // 날 특수 공격 키 (F)
		GROUP_WEAPON_GAUGE_LEFT_KEYSET_B, // 날 특수 공격 키 (Shift+F)

		GROUP_WEAPON_GAUGE_RIGHT_KEYSET_A, // 자루 특수 공격 키 (F)
		GROUP_WEAPON_GAUGE_RIGHT_KEYSET_B, // 자루 특수 공격 키 (Shift+F)

		GROUP_END
	};

protected:
	CUIPage_Play(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Play(const CUIPage_Play& Prototype);
	virtual ~CUIPage_Play() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OpenAction() override;
	virtual void CloseAction() override;

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

	

	// 좌상단 게이지는 스탯 매니저의 내용을 참조하여 변경 
	// 외부 -> 스탯 매니저 -> UI 매니저(Page_Play)

	// 좌하단 포션/도구 
	void Move_SelectCtrl(_bool bIsUp);
	void Swich_ToolItem();

private:
	void LU_Gauge_Update(_float fTimeDelta);
	void LD_Potion_Tool_Update(_float fTimeDelta);
	void LD_Arm_Update(_float fTimeDelta);
	void RU_Coin_Update(_float fTimeDelta);
	void RD_Weapon_Update(_float fTimeDelta);
	
private:
	

private:
	

	// 아래 있는 내용은 Page에 있는 UG_CTRL 리스트의 내용을 얕은 복사하여 사용한다 
	// 따라서 아래 내용 이외에 절대 추가 복사를 진행하면 안되며 릴리즈 시에도 따로 Delete 처리하지 않는다 

	// 체력바 조정
	UG_CTRL* m_HP_Gauge_Frame = { nullptr };
	UG_CTRL* m_HP_Gauge_Fill = { nullptr };

	// 스태미나 조정
	UG_CTRL* m_Stamina_Gauge_Frame = { nullptr };
	UG_CTRL* m_Stamina_Gauge_Fill = { nullptr };

	// 특수 스킬 바 조정
	vector<UG_CTRL*> m_vecSpecial_Gauge_Frame;
	vector<UG_CTRL*> m_vecSpecial_Gauge_Fill;

	// 좌하단 포션/도구 조정
	UG_CTRL* m_Potion_Tex = { nullptr };
	UG_CTRL* m_Potion_Tool_RKey = { nullptr };
	UG_CTRL* m_Potion_Queue = { nullptr };
	UG_CTRL* m_Potion_Gauge = { nullptr };
	UG_CTRL* m_Potion_Count = { nullptr };
	UG_CTRL* m_Tool_Tex = { nullptr };
	UG_CTRL* m_Tool_Queue = { nullptr };
	UG_CTRL* m_Tool_Count = { nullptr };
	UG_CTRL* m_Portion_Tool_Select = { nullptr };

	// 좌하단 암 
	UG_CTRL* m_Arm_Tex = { nullptr };
	UG_CTRL* m_Arm_Name = { nullptr };
	UG_CTRL* m_Arm_Frame = { nullptr };
	UG_CTRL* m_Arm_Fill = { nullptr };

	// 우상단 코인 
	UG_CTRL* m_Coin_Count = { nullptr };

	// 우하단 무기 
	UG_CTRL* m_Weapon_Durable_Frame = { nullptr };
	UG_CTRL* m_Weapon_Durable_Fill = { nullptr };

	UG_CTRL* m_Weapon_Normal_Back = { nullptr };
	UG_CTRL* m_Weapon_Normal_Tex = { nullptr };
	UG_CTRL* m_Weapon_Special_Back = { nullptr };
	UG_CTRL* m_Weapon_Special_Tex = { nullptr };

	UG_CTRL* m_Weapon_Equip_Num = { nullptr };

	vector<UG_CTRL*> m_vecWeapon_Special_Frame;
	vector<UG_CTRL*> m_vecWeapon_Special_Fill;

	vector<UG_CTRL*> m_vecPage_Play_Ctrl;


public:
	static CUIPage_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END