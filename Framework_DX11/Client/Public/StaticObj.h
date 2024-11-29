#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CStaticObj :
    public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_char		szModelTag[MAX_PATH];
		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;
		_bool		isInstance = { false };
		_bool		bShadow = { false };
		_uint		iRenderGroupID = { true };
		_int		iCurrentCellNum;
	}STATICOBJ_DESC;

private:
	CStaticObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticObj(const CStaticObj& Prototype);
	virtual ~CStaticObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool		m_isInstance = { false };
	_bool		m_bShadow = { false };

	_int		m_iCurrentCellNum = { -1 };

	_float		m_fCullDistance = { 50.f };
	_float		m_fEmissiveMask = {};

	INSTANCE_DATA		m_tInstanceData = {};

	_float4				m_vHashColor = {};

private:
	HRESULT Ready_Components(STATICOBJ_DESC* pNonAnimDesc);

public:
	static CStaticObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END