#pragma once

#include "GameObject.h"
#include "Tools_Defines.h"

BEGIN(Tools)
class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _Matrix* pParentMatrix = { nullptr };
		_tchar		strEffectName[MAX_PATH] = L"";
	} EFFECT_BASE_DESC;

	enum EFFECT_TYPE { TYPE_PARTICLE, TYPE_TEXTURE, TYPE_MESH, TYPE_END };

	enum RENDER_STATE
	{
		RS_NONBLEND		= 0x0001,
		RS_NONLIGHT		= 0x0002,
		RS_DISTORTION	= 0x0004,
		RS_BLUR			= 0x0008,
		RS_BLEND		= 0x0010,
		RS_END			= 0x9999
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

	_Matrix m_WorldMatrix = XMMatrixIdentity();
	const _Matrix* m_pParentMatrix = { nullptr };

	_wstring m_strEffectName = TEXT("");

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
