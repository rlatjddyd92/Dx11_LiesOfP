#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
class CCamera;
END

BEGIN(Client)

class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void Add_Camera(const _wstring& strCameraTag, class CCamera* pCamera);
	HRESULT Change_Camera(const _wstring& strCameraTag);
	class CCamera* Find_Camera(const _wstring& strCameraTag);

	void Clear();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	class CCamera*			m_pCurrentCamera = { nullptr };
	map<const _wstring, class CCamera*> m_Cameras;

public:
	virtual void Free() override;
};

END

