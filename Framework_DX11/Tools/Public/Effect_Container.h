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
		vector<void*> pTrail_OPDesc;
		vector<void*> pTrail_TPDesc;
		vector<void*> pTrail_MPDesc;

		_tchar szEffectContainerName[MAX_PATH] = TEXT("");
	} EFFECT_DESC;

private:
	CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Container(const CEffect_Container& Prototype);
	virtual ~CEffect_Container() = default;

public:
	_wstring Get_ContainerName() {
		return m_strContainerName;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Reset_Effects();
	HRESULT Save_Effects(_wstring strFilePath);

	HRESULT Add_Effects_To_Layer();

	HRESULT Save_EffectContainer(_wstring strFolderPath);
	HRESULT Load_EffectContainer(_wstring strFilePath);

private:
	vector<class CEffect_Base*> m_Effects;
	vector<_wstring> m_EffectNames;
	_wstring m_strContainerName = TEXT("");


	//Å×½ºÆ®
	_bool m_bOrbit = { false };
	_bool m_bTurn = { false };

private:
	HRESULT Load_Effect_By_Path(const _wstring& strFilePath);
	_wstring getPreviousFolderPath(const _wstring& path);

public:
	static CEffect_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END