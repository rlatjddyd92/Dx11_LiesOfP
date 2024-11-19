#pragma once

#include "Tools_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Tools)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	_bool isFinished() const {
		return m_isFinished_Main && m_isFinished_Map0 && m_isFinished_Map1 && m_isFinished_Monster;
	}

public:
	HRESULT Initialize(LEVELID eNextLevelID);
	HRESULT Loading();
	HRESULT LoadingMapModel0();
	HRESULT LoadingMapModel1();
	HRESULT LoadingMonsterModel1();
	void Draw_LoadingText();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	LEVELID						m_eNextLevelID = { LEVEL_END };

	HANDLE						m_hThread_Main = {};
	CRITICAL_SECTION			m_CriticalSection_Main = {};

	HANDLE						m_hThread_Map0 = {};
	CRITICAL_SECTION			m_CriticalSection_Map0 = {};

	HANDLE						m_hThread_Map1 = {};
	CRITICAL_SECTION			m_CriticalSection_Map1 = {};

	HANDLE						m_hThread_Monster = {};
	CRITICAL_SECTION			m_CriticalSection_Monster = {};

	_bool						m_isFinished_Main = { false };
	_bool						m_isFinished_Map0 = { false };
	_bool						m_isFinished_Map1 = { false };
	_bool						m_isFinished_Monster = { false };

	_tchar						m_szLoadingText[MAX_PATH] = {};

private:
	HRESULT Ready_Resources_For_LogoLevel();

	HRESULT Ready_Resources_For_ToolLevel();
	HRESULT Ready_Resources_For_ToolLevel_Map0();
	HRESULT Ready_Resources_For_ToolLevel_Map1();
	HRESULT Ready_Resources_For_ToolLevel_Monster();

	HRESULT Ready_Textures_For_Decal();

	HRESULT Ready_Resources_For_Monster();


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END