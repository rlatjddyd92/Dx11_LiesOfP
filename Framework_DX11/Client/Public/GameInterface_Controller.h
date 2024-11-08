#pragma once

#include "Client_Defines.h"
#include "Base.h"


#include "GameInstance.h"

#include "UI_Enum.h"

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
#pragma region UIManager





#pragma endregion


#pragma region ItemManager



#pragma endregion




public:
	virtual void Free() override;
};

END
