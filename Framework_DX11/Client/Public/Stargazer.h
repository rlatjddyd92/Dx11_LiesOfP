#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CRigidBody;
END

BEGIN(Client)

class CStargazer :
    public CGameObject
{
public:
	enum STARTYPE{BROKEN, RESTORED, TYPE_END};
protected:
	CStargazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStargazer(const CStargazer& Prototype);
	virtual ~CStargazer() = default;

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

public:
	_bool Get_IsOpend() { return m_isOpened; }

private:
	CCollider* m_pColliderCom = { nullptr};
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom[TYPE_END] = {nullptr, nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };

private:
	_bool m_bShadow = { false };
	_bool m_bCollison = { false };
	_bool m_bRestored = { false };
	_bool m_isClose = { true };					//작동 전
	_bool m_isOpening = { false };				//열리는 중
	_bool m_isOpened = { false };				//작동 완
		
	_int m_iAnim_Broken = { 0 };
	_int m_iAnim_Restore = { 0 };
	_int m_iAnim_Close = { 0 };
	_int m_iAnim_Open = { 0 };
	_int m_iAnim_OpenIdle = { 0 };
	_int m_iCurrnetCellNum = { -1 };

	_float m_fRestoreTimer = { 0.f };

	CGameObject* m_pPlayer = { nullptr };
	CModel* m_pCurrentModel = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static CStargazer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END