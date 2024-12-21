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

class CSteelHeart :
	public CGameObject
{
private:
	CSteelHeart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSteelHeart(const CSteelHeart& Prototype);
	virtual ~CSteelHeart() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	_uint		m_iAnimation_Gear = {};

private:
	HRESULT Ready_Components();

public:
	static CSteelHeart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END