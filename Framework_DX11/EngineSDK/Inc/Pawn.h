#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPawn abstract : public CGameObject
{
protected:
	CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPawn(const CPawn& Prototype);
	virtual ~CPawn() = default;

public:
	class CModel*		Get_Model() { return m_pModelCom; }
	class CRigidBody*	Get_RigidBody() { return m_pRigidBodyCom; }

	_float				Get_MoveSpeed() { return m_fMoveSpeed; }
	void				Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Height() { return S_OK; }
	virtual HRESULT Render_LightDepth() { return S_OK; }

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	void		Change_State(const _uint iState, void* pArg = nullptr);
	void		Set_Animation(_uint iAnimIndex, _bool IsLoop = false);
	void		Change_Animation(_uint iAnimIndex, _bool IsLoop = false, _float fDuration = 0.2f, _uint iStartFrame = 0, _bool bEitherBoundary = true);
	void		Change_Animation_Boundry(_uint iAnimIndex, _bool IsLoop = false, _float fDuration = 0.2f, _uint iStartFrame = 0);
	_uint		Get_CurrentAnimIndex();
	_int		Get_Frame();
	_bool		Get_EndAnim(_int iAnimIndex, _bool bIsBoundary = false);

protected:
	class CShader*		m_pShaderCom = { nullptr };
	class CModel*		m_pModelCom = { nullptr };
	class CNavigation*	m_pNavigationCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };
	class CFsm*			m_pFsmCom = { nullptr };
	class CRigidBody*	m_pRigidBodyCom = { nullptr };

	_bool				m_isGravity = { false };
	
	_float				m_fMoveSpeed = {};

protected:
	HRESULT Bind_WorldViewProj();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END