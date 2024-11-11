#pragma once

#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CPawn
{
public:
	enum PARTID { PART_BODY, PART_WEAPON, PART_END };

	enum PLAYER_STATE
	{
		IDLE, WALK, RUN, STATE_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_FSM();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};


END