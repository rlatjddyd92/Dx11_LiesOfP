#pragma once

#include "Dissolve_Effect.h"

class CDissolve_Electric final : public CDissolve_Effect
{
private:
	CDissolve_Electric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDissolve_Electric(const CDissolve_Electric& Prototype);
	virtual ~CDissolve_Electric() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Set_On(_bool bOn) override;

private:
	// 이건 뭐 알아서 하고
	class CTexture* m_pTextureCom = nullptr;
	class CShader_Compute* m_pActionCS = { nullptr };

	// 렌더 셰이더에 전달
	_uint								m_iShaderIndex = { 0 };
	_uint								m_iGeomState = { 0 };
	_Vec2								m_vTexDevide = { 1.f ,1.f };
	_float								m_fSpriteSpeed = { 0.f };
	_Vec2								m_vStartScaling = { 1.f, 1.f };
	_Vec2								m_vScalingRatio = { 0.f, 0.f };
	_float								m_fStartRotation = { 0.f };
	_float								m_fRotationPerSecond = { 0.f };

	_uint								m_iState = {};

private:
	HRESULT Ready_Components();

public:
	static CDissolve_Electric* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDissolve_Electric* Clone(void* pArg);
	virtual void Free() override;
};

