#pragma once

#include "Tools_Defines.h"
#include "Base.h"
#include "Engine_Struct.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_Cutscene : public CBase
{
	DECLARE_SINGLETON(CController_Cutscene)

private:
	CController_Cutscene();
	virtual ~CController_Cutscene() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update(_float fTimeDelta);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
    class CCutScene* m_pCurrentCutScene = {};
	class CCamera * m_pCamera = {};


    _bool m_bFirst = { true };
    _bool m_bPlay = { true };
    _float* m_fTrackPosition = { nullptr };
    _float m_fTrackPosition_Zero = { 0.f };
    _int m_iPreSelectedCutScene = { -1 };
    _int m_iPreSelected_KeyFrame = { -1 };

	CUTSCENE_DESC* pCutScene_Desc = { nullptr };

	class CCutScene* m_CutSceneList[8] = {nullptr,};
    const char* m_CutSceneNameList[8] = { "Boss1_Meet", "Boss1_Phase2", "Boss1_Dead", "Sophia_EnterRoom", "Boss2_Meet",  "Boss2_Phase2",  "Boss2_Defeat", "Sophia_Dead" };
    const char* m_ActorTypeNameList[5] = { "Camera", "UI", "Shader", "GameObject"};


private:
	void Menu();	
	void Show_Camera_State();
	void Show_UI_State();
	void Show_Shader_State();
	void Show_GamgeObject_State();
	void Camera_Memu();
	void UI_Memu();
	void Shader_Memu();
	void GamgeObject_Memu();
	void Show_CurCamState();

public:
	virtual void Free() override;
};

END
