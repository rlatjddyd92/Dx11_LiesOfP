#pragma once

#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster : public CPawn
{
public:
	enum MONSTER_STATE {
		IDLE, WALK, RUN, ATTACK, GROGY, HITFATAL, PARALYZE, DIE,

		MONSTER_STATE_END
	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Look_Player();
	_float		Calc_Distance_XZ();


protected:
	_Vec4		m_vRootMoveStack{};
	_Vec4		m_vCurRootMove{};
	_bool		m_bEndAnim{ false };
	_bool		m_bResetRootMove{ true };

	_double		m_GrogyTrackPos{};

protected:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END