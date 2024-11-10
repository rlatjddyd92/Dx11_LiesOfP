#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameInstance.h"
#include "UIManager.h"


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CGameInterface_Controller : public CBase
{
	DECLARE_SINGLETON(CGameInterface_Controller)

private:
	CGameInterface_Controller();
	virtual ~CGameInterface_Controller() = default;






public: // 외부에서 매니저 접근 

#pragma region Common
	HRESULT Initialize_GameInterface(ID3D11Device** pDevice, ID3D11DeviceContext** pContext, CGameInstance* pInstance);
	void Update_GameInterface(_float fTimeDelta);
	void Release_GameInterface();
#pragma endregion


#pragma region UIManager
	void OpenMainPage() { m_pUIManager->OpenMainPage(); }
	void OpenLoadingPage() { m_pUIManager->OpenLoadingPage(); }

	_bool GetPlayMode() { return m_pUIManager->GetPlayMode(); }
	void SetPlayMode(_bool bIsPlayMode) { m_pUIManager->SetPlayMode(bIsPlayMode); }

	void SwicthPage(UIPAGE eNextPage) { m_pUIManager->SwicthPage(eNextPage); }


#pragma endregion


#pragma region ItemManager



#pragma endregion




private:
	CGameInstance* m_pGameInstance = { nullptr };
	CUIManager* m_pUIManager = { nullptr };





private: // <- 디바이스 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END
