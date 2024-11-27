#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;

END

BEGIN(Client)

class CStargazer :
    public CGameObject
{
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

private:
	CCollider* m_pColliderCom = { nullptr};
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_bool m_bShadow = { false };
	_bool m_bClose_with_Player = { false };
	_bool m_isClose = { true };					//작동 전
	_bool m_isOpening = { false };				//열리는 중
	_bool m_isOpened = { false };				//작동 완
		
	_int m_iAnim_Close = { 0 };
	_int m_iAnim_Open = { 0 };
	_int m_iAnim_OpenIdle = { 0 };
	_int m_iCurrnetCellNum = { -1 };
	CGameObject* m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();
	void Calculate_Distance_Between_Player();

public:
	static CStargazer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END