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

	virtual CHECK_MOUSE Check_Page_Action(_float fTimeDelta) override;

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

	HRESULT Render_PlayInfo(class CUIRender_Client* pRender_Client);

	// 좌상단 게이지는 스탯 매니저의 내용을 참조하여 변경 
	// 외부 -> 스탯 매니저 -> UI 매니저(Page_Play)

	// 좌하단 포션/도구 
	

private:
	void Action_Potion_Tool(_float fTimeDelta);
	void Action_Arm(_float fTimeDelta);
	void Action_Weapon(_float fTimeDelta);
	void Action_InterAction(_float fTimeDelta);


	void LU_Gauge_Update(_float fTimeDelta);
	void LD_Potion_Tool_Update(_float fTimeDelta);
	void LD_Bag_Update(_float fTimeDelta);
	void LD_Arm_Update(_float fTimeDelta);
	void RU_Coin_Update(_float fTimeDelta);
	void RD_Weapon_Update(_float fTimeDelta);

	void PlayInfo_Update(_float fTimeDelta);
	
private:
	_float m_fBag_Open_Waiting_Now = 0.f;
	_float m_fBag_Open_Waiting_Limit = 0.5f;

private: // 테스트 변수
	_bool m_bWeapon_Top = true;

public:
	static CUIPage_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END