#pragma once

#include "GameObject.h"
#include "Tools_Defines.h"

BEGIN(Tools)
class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct
	{
		_int			iRenderGroup = { 0 };
		_int			iPpState = { PP_END };
	}RENDER_DESC;

	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _Matrix* pParentMatrix = { nullptr };
		_tchar			szEffectName[MAX_PATH] = L"";
		RENDER_DESC		RenderDesc = {};
	} EFFECT_BASE_DESC;


	enum EFFECT_TYPE { TYPE_PARTICLE, TYPE_TEXTURE, TYPE_MESH, TYPE_TRAIL_OP, TYPE_TRAIL_TP, TYPE_TRAIL_MP, TYPE_END };
	enum PARTICLE_TYPE { PT_SPREAD, PT_MOVE, PT_CONVERGE, PT_END };

	enum GEOM_STATE
	{
		PS_GROW = 0x0001,
		PS_SHRINK = 0x0002,
		PS_ROTATION = 0x0004,
		PS_END
	};

	enum EFFECT_POSTPROCESSING
	{
		PP_NONE = 0x0001,
		PP_DISTORTION = 0x0002,
		PP_BLUR = 0x0004,
		PP_END = 0xFFFF
	};

	enum EFFECT_TEXTURE
	{
		TEXTURE_DIFFUSE,
		TEXTURE_MASK_1,
		TEXTURE_MASK_2,
		TEXTURE_NORMAL,
		TEXTURE_END
	};

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& Prototype);
	virtual ~CEffect_Base() = default;

public:
	_wstring Get_EffectName() {
		return m_strEffectName;
	}

	EFFECT_TYPE Get_EffectType() {
		return m_eEffectType;
	}

	void Set_ParentMartix(_Matrix* pParentMatrix) {
		m_pParentMatrix = pParentMatrix;
	}

	virtual void Set_Loop(_bool bLoop) = 0;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	void Set_WorldMatrix();
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pConstantName);

public:
	virtual void Reset();
	virtual HRESULT Save(_wstring strFilePath);

protected:
	EFFECT_TYPE		m_eEffectType = { TYPE_END };
	RENDER_DESC		m_RenderDesc = {};

	_Matrix m_WorldMatrix = XMMatrixIdentity();
	const _Matrix* m_pParentMatrix = { nullptr };

	_wstring m_strEffectName = TEXT("");

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
