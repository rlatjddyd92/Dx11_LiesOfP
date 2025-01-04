#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader_NonVTX;
class CVIBuffer_Dissolve_Instance;
class CShader_Compute;
class CModel;
class CTransform;
END

BEGIN(Client)
class CDissolve_Effect abstract : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_wstring strVIBufferTag = TEXT("");

		class CModel* pTarget_ModelCom = { nullptr };
		class CTransform* pTarget_TransformCom = { nullptr };
	}DISSOLVE_EFFECT_DESC;

protected:
	CDissolve_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDissolve_Effect(const CDissolve_Effect& Prototype);
	virtual ~CDissolve_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset();
	virtual void Set_On(_bool bOn) {
		m_bOn = bOn;
		Reset();
	}

protected:
	class CShader_NonVTX* m_pShaderCom = { nullptr };
	class CVIBuffer_Dissolve_Instance* m_pVIBufferCom = { nullptr };
	class CShader_Compute* m_pResetCS = { nullptr };

	class CModel* m_pTarget_ModelCom = { nullptr };
	class CTransform* m_pTarget_TransformCom = { nullptr };

	_bool	m_bOn = { false };

private:
	HRESULT Ready_Components(const DISSOLVE_EFFECT_DESC& Desc);

public:
	virtual void Free() override;
};
END
