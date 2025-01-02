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
		GROUP_POTION_FRAME,
		GROUP_TOOL_FRAME,

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


		// 획득 아이템 정보
		GROUP_DROP_STATIC,
		GROUP_DROP_ITEM,
		GROUP_DROP_TYPE,
		GROUP_DROP_NAME,

		// 상호작용 안내 
		GROUP_INTER_STATIC,
		GROUP_INTER_EKEY,
		GROUP_INTER_TEXT,

		// 디버프 
		GROUP_DEBUFF_STATIC,
		GROUP_DEBUFF_ICON,
		GROUP_DEBUFF_NAME,
		GROUP_DEBUFF_FILL,

		// 보스 체력바 
		GROUP_BOSS_NAME,
		GROUP_BOSS_STATIC,
		GROUP_BOSS_HP_FILL,

		// 보조
		GROUP_PLAYER_HP_SECOND,
		GROUP_PLAYER_STAMINA_SECOND,
		GROUP_BOSS_HP_SECOND,

		// 게이지 폰트 
		GROUP_HP_NUM,
		GROUP_ST_NUM,

		// 아뮬렛
		GROUP_AMULET_DEFENCE_STATIC,
		GROUP_AMULET_DEFENCE_ITEM,
		GROUP_AMULET_DEFENCE_NAME,
		GROUP_AMULET_STAT_ICON,
		
		GROUP_AMULET_DEFENCE_DESC,
		GROUP_AMULET_DEFENCE_STAT,

		GROUP_CTRL,

		GROUP_END
	};

	typedef struct DROP_ITEM_INFO
	{
		_int iIndex = -1;
		_int iCount = 0;
		_float fLifeTime = 0.f;
	}DROP;

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

	virtual CHECK_MOUSE Check_Page_Action(_float fTimeDelta) override;

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

	HRESULT Render_PlayInfo(class CUIRender_Client* pRender_Client);

	_bool Action_InterAction(_wstring strInterName);

	void Input_Drop_Item_Info(_int iIndex, _int iCount)
	{
		DROP_ITEM_INFO* pNew = new DROP_ITEM_INFO;
		pNew->iIndex = iIndex;
		pNew->iCount = iCount;
		pNew->fLifeTime = m_fDrop_Item_ShowTime;
		m_DropItem_Index_list.push_back(pNew);
		if (m_DropItem_Index_list.size() > m_iDrop_Item_Render_Limit)
			while (m_DropItem_Index_list.size() > m_iDrop_Item_Render_Limit)
			{
				Safe_Delete(m_DropItem_Index_list.front());
				m_DropItem_Index_list.pop_front();
			}
	}

	// 보스 체력바 
	void Activate_Boss_Hp_Bar(_bool bActive) { m_bIs_BossHp_Activate = bActive; }
	void Set_Boss_Hp_Bar_Info(_wstring strName, _float HpNow, _float HpMax) { m_strBossName = strName, m_fBoss_Hp_Now = HpNow; m_fBoss_Hp_Max = HpMax; }

	// 무기 락 조정
	void SetWeaponLock(_bool bIsWeaponLock);

	// 키 가이드
	void KeyGuideOnOff(_bool bIsOn) { m_bIsKeyGuideOn = bIsOn; }
	_bool IsKeyGuideOn() { return m_bIsKeyGuideOn; }

public: 



private:
	void Action_Potion_Tool(_float fTimeDelta);
	void Action_Arm(_float fTimeDelta);
	void Action_Weapon(_float fTimeDelta);

	void LU_Gauge_Update(_float fTimeDelta);
	void LD_Potion_Tool_Update(_float fTimeDelta);
	void LD_Bag_Update(_float fTimeDelta);
	void LD_Arm_Update(_float fTimeDelta);
	void RU_Coin_Update(_float fTimeDelta);
	void RD_Weapon_Update(_float fTimeDelta);

	void STAT_Page_Update(_float fTimeDelta);

	void Boss_Hp_Update(_float fTimeDelta);

	void PlayInfo_Update(_float fTimeDelta);
	void Add_Render_Info_DropInfo(_float fTimeDelta);
	void Add_Render_Info_BuffInfo(_float fTimeDelta);

	void Switch_Weapon_UI_Action(_float fTimeDelta);
	void Switch_Bag_UI_Action(_float fTimeDelta, _bool bIsOpen);
	void Using_Fable_UI_Action(_float fTimeDelta);


private:
	// 보조 가방
	_bool m_bIsBagOpen = false;
	_float m_fBag_Open_Waiting_Now = 0.f;
	_float m_fBag_Open_Waiting_Limit = 0.5f;

	_Vec2 m_vBag_OpenAction_Time = { 0.f,0.5f };

	// 인터랙션
	_bool m_bCan_InterAction = true;

	// 드랍
	list<DROP_ITEM_INFO*> m_DropItem_Index_list;
	_int m_iDrop_Item_Render_Limit = 3;
	_float m_fEmerge_Effect_Time = 1.f;
	_float m_fDrop_Item_ShowTime = 3.f;

	// 무기 조정 
	_bool m_bIsWeapon_Lock[2] = { false, false }; // <- [0] : UI의 작동을 실제 막는 변수, [1] : 외부에서 요청 들어온 내용 
	// 만일 [0], [1]이 {false,true}인 경우 무기 전환 후 {true,true}로 바로 맞춘다 
	// [0], [1]이 {true, false}인 경우 {false,false}로 맞추기만 한다 

	_int m_iWeapon_Equip_0_Symbol = 0;
	_Vec2 m_vSwitch_Time = { 0.f,0.25f };
	_float m_fNormal_Weapon_Fx_Alpha_Origin[4] = { 0.f, };
	_Vec2 m_vNormal_Weapon_Fx_Size_Origin[4] = { {0.f,0.f}, };

	_Vec2 m_vFable_Art_ActionTime = { 0.f,0.5f };

	_int m_iFable_Art_Cell_Now = 0;

	// 보스 체력바 
	_bool m_bIs_BossHp_Activate = false;
	_wstring m_strBossName = {};
	_float m_fBoss_Hp_Now = 0.f;
	_float m_fBoss_Hp_Max = 0.f;

	// 스탯창 
	_bool m_bStat_Open = true;
	_Vec2 m_vStat_Open_Time = { 0.f,0.5f };

	// 그라인더 
	_Vec2 m_vGrinder_Wait = { 0.f,2.f };

	_bool m_bIsStart = true;

	// 키 가이드 
	_bool m_bIsKeyGuideOn = true;
	_Vec2 m_vKeyGuideTime = { 0.f,1.f };

private: // 테스트 변수
	_bool m_bWeapon_Top = true;

public:
	static CUIPage_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END