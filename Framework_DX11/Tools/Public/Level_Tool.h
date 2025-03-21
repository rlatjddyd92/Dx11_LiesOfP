#pragma once

#include "Tools_Defines.h"


#include "Level.h"

BEGIN(Tools)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Effect();

	HRESULT Ready_LandObjects();
	//HRESULT Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	//HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

private:
	class CImGui_Manager* m_pImGui_Manager = { nullptr };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
