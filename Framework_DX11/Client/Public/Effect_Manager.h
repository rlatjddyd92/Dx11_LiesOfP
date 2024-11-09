#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
public:
	enum EFFECT
	{
		EFFECT_POWER_HIT,
		EFFECT_END
	};

private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strEffectPath);

public:
	class CEffect_Container* Clone_Effect(EFFECT eEffect, void* pArg);
	HRESULT Add_Effect_ToLayer(_uint iLevelID, EFFECT eEffect, void* pArg);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	
	map<const _wstring, CParticle_Effect::PARTICLE_EFFECT_DESC> m_PEDescs;
	map<const _wstring, CTexture_Effect::TEXTURE_EFFECT_DESC> m_TEDescs;

private:
	HRESULT Load_Effects(const _wstring& strEffectPath);
	HRESULT Load_Textures();
	_wstring Get_FileExtentin(const _wstring& strFileName);

	HRESULT Load_Particle_Effect(const _wstring& strResultPath);
	HRESULT Load_Texture_Effect(const _wstring& strResultPath);
	HRESULT Load_Mesh_Effect(const _wstring& strResultPath);

	CParticle_Effect* Clone_ParticleEffect(const _wstring& strEffectTag);
	CTexture_Effect* Clone_TextureEffect(const _wstring& strEffectTag);

public:
	virtual void Free() override;
};
END


