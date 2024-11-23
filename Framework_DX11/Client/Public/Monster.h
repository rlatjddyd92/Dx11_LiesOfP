#pragma once

#include "Pawn.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster abstract : public CPawn
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

	_Vec4		Get_TargetPos() { return m_vPosTarget; }
	_Vec4		Get_TargetDir();

public:
	void		Set_UpTargetPos();
	void		Look_Player();
	_float		Calc_Distance_XZ();


protected:
	_Vec4		m_vRootMoveStack{};
	_Vec4		m_vCurRootMove{};
	_bool		m_bEndAnim{ false };
	_bool		m_bResetRootMove{ true };

	_float		m_fPrevTrackPos{};

	_Vec4		m_vPosTarget{};

protected:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END