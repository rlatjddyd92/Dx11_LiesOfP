# Dx11_LiesOfP
DirectX11로 모작한 P의 거짓말 모작

김성용 참여 내용 
[1] UI 객체 시스템 구현 
[2] 아이템 관련 데이터 및 접근 시스템 
[3] 업적 기능 구현 
[4] 튜토리얼 미션 구현 

1. 게임 인터페이스 싱글톤 (UI 및 아이템 데이터 통합관리)
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/GameInterface_Controller.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/GameInterface_Controller.h

2. UI매니저 (모든 UI 객체 접근 및 관리)
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIManager.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIManager.h

3. 아이템 매니저 (인벤토리 및 보관한, 상점 데이터 접근)
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/Item_Manager.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/Item_Manager.h

4. UI 렌더링 객체 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIRender_Batching.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIRender_Batching.h

5. UI Page 객체 (UI 객체를 기능에 따라 나누어 통합 관리)
1) 업적 관련 기능 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Achievment.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Achievment.h
1-1) 업적 달성 여부 안내 UI
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/Achievment_DataLine.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/Achievment_DataLine.h
1-2) 업적 페이지 플레이 통계 UI
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/Statistic_Data.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/Statistic_Data.h
3) 보관함
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Chest.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Chest.h
4) 게임 화면 페이드 아웃, 페이드 인 조정 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Effect.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Effect.h
5) 유저 장비 창
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Equip.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Equip.h
6) 게임 진행 시, 상단에 노출되는 안내 메시지 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Inform.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Inform.h
7) 유저 인벤토리 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Inven.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Inven.h
8) 아이템 상호작용 시, 툴팁 및 아이템 조작 버튼
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_ItemInfo.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_ItemInfo.h
9) 게임 로딩 페이지 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Loading.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Loading.h
10) 게임 실행 시 노출되는 메인 화면 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Main.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Main.h
11) 게임 중 ESC 키로 접근하는 메뉴 화면 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Menu.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Menu.h
12) 게임 옵션 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Option.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Option.h
13) 몬스터 직교 UI
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Ortho.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Ortho.h
14) 게임 메인 플레이 화면 UI (체력바, 무기 정보 등) 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Play.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Play.h
15) 안내 팝업
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Popup.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Popup.h
16) 상점 기능
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Shop.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Shop.h
17) 유저 스탯 조정 기능 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Stat.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Stat.h
18) NPC 대화 기능 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Talking.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Talking.h
19) 순간이동 페이지 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Telepot.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Telepot.h
20) 튜토리얼 기능 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UIPage_Tutorial.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UIPage_Tutorial.h
19-1) 튜토리얼 중 조작 안내 페이지 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UITutorial_Guide.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UITutorial_Guide.h
19-2) 튜토리얼 중 미션 진행 상활 안내
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UITutorial_Info.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UITutorial_Info.h
19-3) 튜토리얼 중 다음 미션 안내 팝업
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UITutorial_Popup.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UITutorial_Popup.h
19-4) 튜토리얼 중 조작 안내 페이지 
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UITutorial_Result.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UITutorial_Result.h
19-5) 튜토리얼 중 시간 제한 기술 UI
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Private/UITutorial_Timing.cpp
https://github.com/rlatjddyd92/Dx11_LiesOfP/blob/main/Framework_DX11/Client/Public/UITutorial_Timing.h







