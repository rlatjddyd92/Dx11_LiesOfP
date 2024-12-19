#pragma once
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CSophia :
	public CGameObject
{
private:
	CSophia(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSophia(const CSophia& Prototype);
	virtual ~CSophia() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Active_HeartEffect();
	void DeActive_HeartEffect();
	void Active_DisapperEffect();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	class CEffect_Container* m_pHearEffeft = { nullptr };
private:
	_bool m_bShadow = false;

private:
	HRESULT Ready_Components(OBJECT_DEFAULT_DESC* pDesc);
	HRESULT Ready_Effect();

public:
	static CSophia* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END