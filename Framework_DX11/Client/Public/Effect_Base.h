#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_Matrix* pParentMatrix = { nullptr };

	} EFFECT_BASE_DESC;

	enum EFFECT_TYPE { TYPE_PARTICLE, TYPE_TEXTURE, TYPE_END };

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& Prototype);
	virtual ~CEffect_Base() = default;

public:
	void Set_ParentMatrix_Ptr(const _Matrix* pParentMatrix) {
		m_pParentMatrix = pParentMatrix;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset();
	

protected:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pConstantName);

protected:
	EFFECT_TYPE m_eEffectType = { TYPE_END };

	_Matrix m_WorldMatrix = XMMatrixIdentity();
	const _Matrix* m_pParentMatrix = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
