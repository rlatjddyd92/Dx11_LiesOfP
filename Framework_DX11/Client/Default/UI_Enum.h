#pragma once

// UI Part/Page의 편리한 사용 및 휴먼 에러 방지를 위한 이넘 목록
// 필요한 클래스는 모두 사용 가능 (
// ★ 추후 Client_Defines에 넣을 지 논의 필요

	enum class MOVETYPE
	{
		TYPE_STATIC, // 움직이지 않음
		TYPE_MOVE, // 움직임
		TYPE_BAR, // 바 형태로 표시 (HP바 등)
		TYPE_END
	};

	enum class PAGEACTION
	{
		ACTION_NONE,
		ACTION_OPENING,
		ACTION_CLOSING,
		ACTION_SHAKING,
		ACTION_END
	};

	enum class UIPAGE
	{
		PAGE_MAIN,
		PAGE_LOADING,
		PAGE_PLAY,
		PAGE_END

		/*PAGE_MENU,
		PAGE_INVEN,
		PAGE_EQUIP,
		PAGE_STAT,
		PAGE_LEVELUP,
		PAGE_SKILL,*/

	};


	enum class UI_FONT
	{
		FONT_NORMAL_8,
		FONT_NORMAL_12,
		FONT_NORMAL_16,
		FONT_NORMAL_24,
		FONT_NORMAL_32,
		FONT_NORMAL_40,
		FONT_NORMAL_48,
		FONT_BOLD_8,
		FONT_BOLD_12,
		FONT_BOLD_16,
		FONT_BOLD_24,
		FONT_BOLD_32,
		FONT_BOLD_40,
		FONT_BOLD_48,
		FONT_END
	};


	enum class TEST_PAGE_VALUE_TYPE
	{
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_FLOAT2,
		TYPE_FLOAT3,
		TYPE_FLOAT4,
		TYPE_END
	};


	enum class TEST_PAGE_NAME
	{
		NAME_TEAMJANG,
		NAME_MAP,
		NAME_EFFECT,
		NAME_ANIM,
		NAME_PLAYER_STAT,
		NAME_ITEM,
		NAME_END
	};