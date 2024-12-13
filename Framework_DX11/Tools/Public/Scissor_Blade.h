#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Tools)

class CScissor_Blade final : public CGameObject
{
public:
	typedef struct : public GAMEOBJECT_DESC
	{
		_uint iWeaponType = 0;
		const _Matrix* pParentWorldMatrix = { nullptr };
		const _Matrix* pSocketBoneMatrix = { nullptr };
	}BLADE_DESC;

private:
	CScissor_Blade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScissor_Blade(const CScissor_Blade& Prototype);
	virtual ~CScissor_Blade() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_vector		m_vRootMoveStack{};

	_float		m_fEmissive = {};
	_float		m_fRimLight = {};

	_Matrix					m_WorldMatrix = {};
	const _Matrix*			m_pParentMatrix = { nullptr };
	const _Matrix*			m_pSocketMatrix = { nullptr };

private:
	HRESULT Ready_Components(_uint iType);

public:
	static CScissor_Blade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END