#pragma once
#include "Component.h"
#include "State.h"

BEGIN(Engine)

class ENGINE_DLL CFsm :
	public CComponent
{

private:
	CFsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFsm(const CFsm& Prototype);
	virtual ~CFsm() = default;

public:
	class CGameObject*	Get_Owner() { return m_pOwner; }

	_uint				Get_CurrentState() { return m_pNowState->Get_State(); }
	_uint				Get_PrevState() { return m_pPreState->Get_State(); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta);
	virtual void Release_States();

public:
	void				Add_State(CState* pState);
	void				Change_State(_uint eState, void* pArg = nullptr);
	void				Set_State(_uint eState, void* pArg = nullptr);

private:
	vector<CState*>		m_States;	//모든 상태를 갖고 있는 벡터 - 인덱스 접근용

	CState*				m_pNowState = { nullptr };			//지금 상태
	CState*				m_pPreState = { nullptr };			//이전 상태

	class CGameObject*	m_pOwner = { nullptr };

public:
	static CFsm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
