#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_PostProcess : public CBase
{
	DECLARE_SINGLETON(CController_PostProcess)

private:
	CController_PostProcess();
	virtual ~CController_PostProcess() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void Update_SSAO();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:

public:
	virtual void Free() override;
};

END

