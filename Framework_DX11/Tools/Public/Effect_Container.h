#pragma once

#include "GameObject.h"
#include "Tools_Defines.h"


BEGIN(Tools)
class CEffect_Container final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		vector<void*> pParticleEffect_Descs;
		vector<void*> pTextureEffect_Descs;
		vector<void*> pMeshEffect_Descs;
	} EFFECT_DESC;

private:
	CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Container(const CEffect_Container& Prototype);
	virtual ~CEffect_Container() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Reset_Effects();

private:
	vector<class CEffect_Base*> m_Effects;


public:
	static CEffect_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END