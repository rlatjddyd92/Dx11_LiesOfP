#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader_NonVTX;
class CVIBuffer_Dissolve_Instance;
class CTexture;
class CShader_Compute;
class CModel;
END

BEGIN(Client)
class CSimonManus_2P_Aura final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		class CModel*			pModelCom = { nullptr };
		class CModel*			pCutSceneModelCom = { nullptr };
		class CTransform*		pManus_TransformCom = { nullptr };
		_float2					vTextureSize = {};
	}SIMONMANUS_2P_AURA_DESC;

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
	void Reset();
	void Set_On(_bool bOn, _bool bCutScene) {
		m_bOn = bOn;
		m_bCutScene = bCutScene;

		Reset();
	}

private:
	class CShader_NonVTX* m_pShaderCom = { nullptr };
	// 모델 만들 때 넣어준 테그 그대로 쓰고
	class CVIBuffer_Dissolve_Instance* m_pVIBufferCom = { nullptr };
	class CVIBuffer_Dissolve_Instance* m_pCutSceneVIBufferCom = { nullptr };
	// 이건 뭐 알아서 하고
	class CTexture* m_pTextureCom = nullptr;
	// 디졸브 컴퓨트 셰이더
	class CShader_Compute* m_pActionCS = { nullptr };
	class CShader_Compute* m_pResetCS = { nullptr };

	// 마누스 모델
	class CModel* m_pModelCom = { nullptr };
	class CModel* m_pCutSceneModelCom = { nullptr };

	class CTexture* m_pEmissiveTextureCom = { nullptr };

	// 마누스 트렌스폼
	class CTransform* m_pManus_TransformCom = { nullptr };

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

private:
	HRESULT Ready_Componet();

public:
	static CSimonManus_2P_Aura* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END