#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CNonAnimModel final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_char		szLayerTag[MAX_PATH];
		_char		szPrototypeTag[MAX_PATH];
		_char		szModelTag[MAX_PATH];
		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;
	}NONMODEL_DESC;

private:
	CNonAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNonAnimModel(const CNonAnimModel& Prototype);
	virtual ~CNonAnimModel() = default;

public:
	NONMODEL_DESC Get_NonAniModelDesc() { return m_tDesc; }
	void Set_NonAniModelDesc(NONMODEL_DESC* Desc) { memcpy(&m_tDesc, Desc, sizeof(NONMODEL_DESC)); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader*	m_pShaderCom = { nullptr };
	class CModel*	m_pModelCom = { nullptr };

	NONMODEL_DESC	m_tDesc;

private:
	HRESULT Ready_Components(NONMODEL_DESC* pNonAnimDesc);

public:
	static CNonAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END