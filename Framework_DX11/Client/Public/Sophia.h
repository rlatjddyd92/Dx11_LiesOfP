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
protected:
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

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_bool m_bShadow = false;

private:
	HRESULT Ready_Components(OBJECT_DEFAULT_DESC* pDesc);

public:
	static CSophia* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END