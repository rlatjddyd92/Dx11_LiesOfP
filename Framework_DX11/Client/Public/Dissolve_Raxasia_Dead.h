#pragma once

#include "Dissolve_Effect.h"

BEGIN(Engine)
class CTexture;
class CShader_Compute;
END

BEGIN(Client)
class CDissolve_Raxasia_Dead final : public CDissolve_Effect
{
public:
	typedef struct : public CDissolve_Effect::DISSOLVE_EFFECT_DESC
	{
		class CTexture*		pDissolveTextureCom = { nullptr };
		_float*				pThreshold = {};
		_float2				vTextureSize = {};
	}DISSOLVE_OBJECT_DESC;

private:
	CDissolve_Raxasia_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDissolve_Raxasia_Dead(const CDissolve_Raxasia_Dead& Prototype);
	virtual ~CDissolve_Raxasia_Dead() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	// 이건 뭐 알아서 하고
	class CTexture* m_pTextureCom = nullptr;
	// 디졸브 컴퓨트 셰이더
	class CShader_Compute* m_pActionCS = { nullptr };
	// 디졸브 텍스처
	class CTexture* m_pDissolveTextureCom = { nullptr };

	// 렌더 셰이더에 전달
	_uint								m_iShaderIndex = { 0 };
	_uint								m_iGeomState = { 0 };
	_Vec2								m_vTexDevide = { 1.f ,1.f };
	_float								m_fSpriteSpeed = { 0.f };
	_Vec2								m_vStartScaling = { 1.f, 1.f };
	_Vec2								m_vScalingRatio = { 0.f, 0.f };
	_float								m_fStartRotation = { 0.f };
	_float								m_fRotationPerSecond = { 0.f };

	_float*								m_pThreshold = {};
	_float2								m_vTextureSize = {};

private:
	HRESULT Ready_Componet();

public:
	static CDissolve_Raxasia_Dead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END