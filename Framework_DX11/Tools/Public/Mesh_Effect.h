#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Tools)
class CMesh_Effect final : public CEffect_Base
{
public:
	typedef struct
	{
		_tchar		szModelTag[MAX_PATH] = L"";

		_tchar		strDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		strMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		strMaskTextureTag_2[MAX_PATH] = L"";

	}PROTOTYPE_DESC;

	typedef struct
	{
		_uint		iState			= { 0 };
		_uint		iShaderIndex	= { 0 };
		_float		fDuration		= { 0.f };
		_Vec4		vColor			= {};
		_Vec2		vTileRepeat		= {};
		_Vec2		vTileMoveDir	= {};
		_float		fTileMoveSpeed	= { 0.f };
		_Vec3		vPos			= {};
		_Vec3		vStartRotation	= {};
		_Vec4		vRotationAxis	= {};
		_float		fRotationSpeed	= { 0.f };
		_Vec3		vScale			= {};
		_Vec3		vScalingSpeed	= {};
		_float		fAlpha			= { 0.f };
		_float		fAlphaSpeed		= { 0.f };

	}ACTION_DESC;

	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		PROTOTYPE_DESC PrototypeDesc = {};
		ACTION_DESC ActionDesc = {};
	}MESH_EFFECT_DESC;

private:
	CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Effect(const CMesh_Effect& Prototype);
	virtual ~CMesh_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Desc(const ACTION_DESC& desc);
	ACTION_DESC Get_Desc();

public:
	virtual void Reset();
	virtual HRESULT Save(_wstring strFilePath);

private:
	class CShader*	m_pShaderCom			= { nullptr };
	class CModel*	m_pModelCom				= { nullptr };
	class CTexture* m_pDiffuseTextureCom	= { nullptr };
	class CTexture* m_pMaskTextureCom_1		= { nullptr };
	class CTexture* m_pMaskTextureCom_2		= { nullptr };

private:
	ACTION_DESC m_ActionDesc = {};

	_Vec2	m_vCurrentTileMove = {};
	_float	m_fAccumulateTime = { 0.f };

	MESH_EFFECT_DESC m_SaveDesc = {};
private:
	HRESULT Ready_Components(const PROTOTYPE_DESC& desc);

public:
	static CMesh_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
