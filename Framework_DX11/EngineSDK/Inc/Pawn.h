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

protected:
	class CShader*		m_pShaderCom = { nullptr };
	class CModel*		m_pModelCom = { nullptr };
	class CNavigation*	m_pNavigationCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };
	class CFsm*			m_pFsmCom = { nullptr };

	_bool				m_isGravity = { false };


protected:
	vector<class CPartObject*>			m_Parts;

	HRESULT Add_PartObject(_uint iPartID, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Bind_WorldViewProj();

public:
	virtual CPawn* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END