#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CLift_Floor :
    public CGameObject
{
public:
	enum MOVEDIR { UP,DOWN,DIR_END };

private:
	CLift_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift_Floor(const CLift_Floor& Prototype);
	virtual ~CLift_Floor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	_bool Get_isMoving() { return m_isMoving; }
	void Set_Move_Dir(_int iDir);

private:
	CCollider* m_pColliderCom = { nullptr};
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CRigidBody* m_pRigidBodyCom = { nullptr };

private:
	_bool m_bShadow = { false };
	_bool m_isMoving = { false };	//이동 중
	_bool m_bMove = { false };		//이동 할 건지
	_bool m_bArrive = { false };		//이동 할 건지
	_bool m_bCollid = { false };		//이동 할 건지

	_int m_iPreDir = { DIR_END };

	_float m_fMoveDir = { 0.f };
	_float m_fCollisonTimer = { 0.f };
	_float m_vTargetPosList[2] = { -97.8f,-130.85f};
	_Vec4 m_vTargetPos = {};

	class CLift_Door* m_pDoors[DIR_END] = { nullptr,nullptr };
private:
	HRESULT Ready_Components(OBJECT_DEFAULT_DESC* pDesc);

	_bool Calculate_Arrive_TargetPos();
	void Open_Door();
	void Close_Door();
public:
	static CLift_Floor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END