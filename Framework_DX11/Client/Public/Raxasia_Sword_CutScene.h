#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CRaxasia_Sword_CutScene final : public CGameObject
{
public:
	typedef struct
	{
		const _Matrix* pParentWorldMatrix = { nullptr };
		const _Matrix* pSocketBoneMatrix = { nullptr };
	}WEAPON_DESC;

private:
	CRaxasia_Sword_CutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaxasia_Sword_CutScene(const CRaxasia_Sword_CutScene& Prototype);
	virtual ~CRaxasia_Sword_CutScene() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_float           m_fEmissiveMask = {};

	_Vec4            m_vRimLightColor = {};


	_Matrix					m_WorldMatrix = {};
	const _Matrix*			m_pParentMatrix = { nullptr };
	const _Matrix*			m_pSocketMatrix = { nullptr };

private:

	HRESULT Ready_Components();


public:
	static CRaxasia_Sword_CutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END