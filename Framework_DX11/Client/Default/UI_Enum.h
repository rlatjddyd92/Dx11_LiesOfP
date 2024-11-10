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
		FONT_INFO_12,
		FONT_INFO_18,
		FONT_INFO_24,
		FONT_INFO_36,
		FONT_INFO_48,
		FONT_INFO_60,
		FONT_INFO_72,
		FONT_TITLE_12,
		FONT_TITLE_18,
		FONT_TITLE_24,
		FONT_TITLE_36,
		FONT_TITLE_48,
		FONT_TITLE_60,
		FONT_TITLE_72,
		FONT_END
	};
