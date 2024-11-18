#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon abstract : public CGameObject
{
public:
	typedef struct
	{
		const _Matrix*	pParentWorldMatrix = { nullptr };
		const _Matrix*	pSocketBoneMatrix = { nullptr };
		const _Matrix*	pSocketBoneMatrix2 = { nullptr };
	}WEAPON_DESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	void Appear();
	void Disappear();

protected:
	CShader*			m_pShaderCom = { nullptr };	
	CModel*				m_pModelCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

protected:
	const _Matrix*			m_pParentMatrix = { nullptr };
	const _Matrix*			m_pSocketMatrix = { nullptr };
	const _Matrix*			m_pSocketMatrix2 = { nullptr };
	_Matrix					m_WorldMatrix = {};

protected:
	HRESULT Ready_Components();

	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pContantName);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END