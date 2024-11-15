#pragma once

#include "GameObject.h"
#include "Tools_Defines.h"

BEGIN(Tools)
class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _Matrix*	pParentMatrix = { nullptr };
		_tchar			strEffectName[MAX_PATH] = L"";
		_uint			iRenderGroup = { 0 };
		_uint			iPpState = { PP_END };
	} EFFECT_BASE_DESC;

	enum EFFECT_TYPE { TYPE_PARTICLE, TYPE_TEXTURE, TYPE_MESH, TYPE_END };
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
	EFFECT_TYPE m_eEffectType = { TYPE_END };
	_uint m_iRenderGroup = { 0 };
	_uint m_iPpState = { PP_END };

	_Matrix m_WorldMatrix = XMMatrixIdentity();
	const _Matrix* m_pParentMatrix = { nullptr };

	_wstring m_strEffectName = TEXT("");

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
