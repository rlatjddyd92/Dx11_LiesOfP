#pragma once

#include "Dissolve_Effect.h"

BEGIN(Engine)
class CTexture;
class CShader_Compute;
class CModel;
END

BEGIN(Client)
class CSimonManus_2P_Aura final : public CDissolve_Effect
{
public:
	typedef struct : public CDissolve_Effect::DISSOLVE_EFFECT_DESC
	{
		class CModel*			pCutSceneModelCom = { nullptr };
		_float2					vTextureSize = {};
	}DISSOLVE_OBJECT_DESC;

private:
	CSimonManus_2P_Aura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSimonManus_2P_Aura(const CSimonManus_2P_Aura& Prototype);
	virtual ~CSimonManus_2P_Aura() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset() override;
	virtual void Set_On(_bool bOn) override;
	void Set_CutScene(_bool bCutScene) {
		m_bCutScene = bCutScene;
	}

private:
	// 모델 만들 때 넣어준 테그 그대로 쓰고
	class CVIBuffer_Dissolve_Instance* m_pCutSceneVIBufferCom = { nullptr };
	// 이건 뭐 알아서 하고
	class CTexture* m_pTextureCom = nullptr;
	// 디졸브 컴퓨트 셰이더
	class CShader_Compute* m_pActionCS = { nullptr };
	
	class CModel* m_pCutSceneModelCom = { nullptr };
	class CTexture* m_pEmissiveTextureCom = { nullptr };

	// 렌더 셰이더에 전달
	_uint								m_iShaderIndex = { 0 };
	_uint								m_iGeomState = { 0 };
	_Vec2								m_vTexDevide = { 2.f ,2.f };
	_float								m_fSpriteSpeed = { 0.f };
	_Vec2								m_vStartScaling = { 0.f, 0.f };
	_Vec2								m_vScalingRatio = { 1.5f, 1.5f };
	_float								m_fStartRotation = { 0.f };
	_float								m_fRotationPerSecond = { 0.f };

	_float2								m_vTextureSize = {};

	_bool								m_bCutScene = { false };
	_bool								m_bOn = { false };

	_uint								m_iState = { 0 };
	_float								m_fThreshold = { 0.f };

private:
	HRESULT Ready_Componet();

public:
	static CSimonManus_2P_Aura* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END