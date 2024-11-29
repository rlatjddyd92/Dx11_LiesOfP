#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint			iLevelIndex;
	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	_bool					IsActive() { return m_isActive; }
	void					IsActive(_bool isActive) { m_isActive = isActive; }

	_bool					Get_Dead() { return m_isDead; }
	void					Set_Dead(_bool isDead) { m_isDead = isDead; }

	class CTransform*		Get_Transform() {
		if (m_pTransformCom)
			return m_pTransformCom;
		return nullptr;
	}

	_wstring				Get_Tag() { return m_strObjectTag; }
	void					Set_Tag(_wstring strTag) { m_strObjectTag = strTag; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Height() { return S_OK; }
	virtual HRESULT Render_LightDepth() { return S_OK; }
	virtual HRESULT Render_Picking() { return S_OK; }

	virtual _bool Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos = { 0.f,0.f,0.f }) { return false; }

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

public:
	virtual class CComponent* Find_Component(const _wstring& strComponentTag, _uint iPartObjIndex = 0);
	class CComponent* Find_Component(const COMPONENT_TYPE eType);
	virtual class CComponent* Change_Component(const _wstring& strComponentTag, CComponent* pComponent, _uint iPartObjIndex = 0);//받아온 컴포넌트로 교체하고 이전의 컴포넌트를 반환

	HRESULT Add_Component(const _wstring& strComponentTag, CComponent* pComponent);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	_uint						m_iLevelIndex = {};

	_bool						m_isActive = { true };
	_bool						m_isDead = { false };

	_wstring					m_strObjectTag = {};

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END