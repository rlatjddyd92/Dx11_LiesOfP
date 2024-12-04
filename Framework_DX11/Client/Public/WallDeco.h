#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CSound;
END

BEGIN(Client)

class CWallDeco :
	public CGameObject
{
protected:
	CWallDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWallDeco(const CWallDeco& Prototype);
	virtual ~CWallDeco() = default;

public:
	_bool Get_IsCanHit() { return m_isCanHit; }

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

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	_bool m_bShadow = { false };
	_bool m_bDetect = { false };
	_bool m_isPlayActiveSound = { false };
	_bool m_isCanHit = { false };

	_int m_iAnim_Activate = { 0 };
	_int m_iAnim_Deactivate = { 0 };

	_float m_fPlayActiveAnimTimer = { 0.f };
	_float m_fCoolTime = {};

	CGameObject* m_pCollider_Object = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CWallDeco* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END