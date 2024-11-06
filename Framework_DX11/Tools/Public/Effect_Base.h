#pragma once

#include "GameObject.h"
#include "Tools_Defines.h"

BEGIN(Tools)
class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		
	} EFFECT_BASE_DESC;

	enum EFFECT_TYPE { TYPE_PARTICLE, TYPE_TEXTURE, TYPE_END };

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& Prototype);
	virtual ~CEffect_Base() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	EFFECT_TYPE m_eEffectType = { TYPE_END };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
