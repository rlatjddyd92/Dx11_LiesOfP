#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster : public CGameObject
{
public:
	enum COLLIDERTYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

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
	void		Change_State(const _uint iState, void* pArg = nullptr);
	void		Change_Animation(_uint iAnimIndex, _bool IsLoop, _bool bSetup = false);//셋업 변수 = 선형보간 할지

	void		Look_Player();
	_float		Calc_Distance_XZ();


protected:
	CCollider* m_pColliderCom[TYPE_END] = { nullptr, nullptr, nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CFsm* m_pFSMCom = { nullptr };

protected:
	_vector		m_vRootMoveStack{};
	_vector		m_vCurRootMove{};
	_bool		m_bEndAnim{ false };
	_bool		m_bResetRootMove{ true };

	_double		m_GrogyTrackPos{};

protected:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END