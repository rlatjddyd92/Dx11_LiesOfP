#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CEffect_Container final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _Matrix* pParentMatrix = { nullptr };
		const _Matrix* pSocketMatrix = { nullptr };

		_Vec3 vPos = {};
		_Vec3 vScale = { 1.f, 1.f, 1.f };
		_Vec3 vDir = { 0.f, 0.f, 1.f };
	} EFFECT_DESC;

private:
	CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Container(const CEffect_Container& Prototype);
	virtual ~CEffect_Container() = default;

public:
	_Matrix* Get_WorldMatrix_Ptr() {
		return &m_WorldMatrix;
	}

	void Set_Cloned(_bool isCloned) {
		m_isCloned = isCloned;
	}

	_bool Get_Cloned() {
		return m_isCloned;
	}

	_bool Get_Loop() {
		return m_isLoop;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Add_Effect(class CEffect_Base* pEffectBase);
	HRESULT Reset_Effects();

	void Set_Loop(_bool bLoop);
	void Set_EffectDesc(const EFFECT_DESC& desc);


private:
	vector<class CEffect_Base*> m_Effects;
	_Matrix m_WorldMatrix = {};
	const _Matrix* m_pParentMatrix = { nullptr };
	const _Matrix* m_pSocketMatrix = { nullptr };

	_bool m_isCloned = { false };
	_bool m_isLoop = { false };

	_Matrix m_PreWorldMatrix = {};

public:
	static CEffect_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END