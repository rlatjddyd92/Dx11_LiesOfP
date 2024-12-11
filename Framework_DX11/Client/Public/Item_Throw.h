#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Item_Enum.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CSound;
class CRigidBody;
class CNavigation;
END

BEGIN(Client)

class CItem_Throw :
	public CGameObject
{
public:
	typedef struct
	{
		const _Matrix*	pParentWorldMatrix = { nullptr };
		const _Matrix*	pSocketBoneMatrix = { nullptr };

		_Vec3		vThrowDir = {};

		SPECIAL_ITEM	eType = SPECIAL_ITEM::SP_END;
	}THROWITEM_DESC;

private:
	CItem_Throw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Throw(const CItem_Throw& Prototype);
	virtual ~CItem_Throw() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

	void Explosion();
	void Throw();

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

private:
	_bool			m_isThrow = { false };
	_bool			m_isExplosion = { false };

	_float			m_fThrowTime = {};
	_float			m_fThrowSpeed = {};

	_Vec3			m_vThrowDir = {};

	_Matrix			m_WorldMatrix = {};

	const _Matrix*	m_pParentMatrix = { nullptr };
	const _Matrix*	m_pSocketMatrix = { nullptr };

	SPECIAL_ITEM	m_eType = { SPECIAL_ITEM::SP_END };

private:
	HRESULT Ready_Components();

public:
	static CItem_Throw* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END