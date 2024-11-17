#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"
#include "Mesh_Effect.h"

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
	class CEffect_Container* Clone_Effect(const _wstring& strECTag, void* pArg);
	HRESULT Add_Effect_ToLayer(_uint iLevelID, const _wstring& strECTag, void* pArg);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	
	map<const _wstring, CParticle_Effect::PARTICLE_EFFECT_DESC> m_PEDescs;
	map<const _wstring, CTexture_Effect::TEXTURE_EFFECT_DESC> m_TEDescs;
	map<const _wstring, CMesh_Effect::MESH_EFFECT_DESC> m_MEDescs;

	map<const _wstring, vector<_wstring>> m_EffectContainers;

private:
	HRESULT Load_Effects(const _wstring& strEffectPath);
	HRESULT Load_EffectContainers(const _wstring& strEffectPath);

	HRESULT Load_Textures();
	HRESULT Load_Models();
	HRESULT Load_Shaders();
	HRESULT Load_Objects();

	HRESULT Load_Particle_Effect(const _wstring& strResultPath);
	HRESULT Load_Texture_Effect(const _wstring& strResultPath);
	HRESULT Load_Mesh_Effect(const _wstring& strResultPath);

	CParticle_Effect* Clone_ParticleEffect(const _wstring& strEffectTag);
	CTexture_Effect* Clone_TextureEffect(const _wstring& strEffectTag);
	CMesh_Effect* Clone_MeshEffect(const _wstring& strEffectTag);

	_wstring Get_FileName(const _wstring& strFileTag);
	_wstring Get_FileExtentin(const _wstring& strFileTag);


public:
	virtual void Free() override;
};
END


