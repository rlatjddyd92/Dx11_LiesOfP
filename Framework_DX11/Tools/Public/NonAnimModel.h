#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Tools)

class CNonAnimModel final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_char		szModelTag[MAX_PATH];
		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;
		_bool		isLight = { false };
		_bool		isInstance = { false };
		_bool		bShadow = { false };
		_uint		iRenderGroupID = { true };
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
	virtual HRESULT Render_Picking()override;

	virtual _bool Is_Pick(_float3* vPickPos);
	int Get_HashId() { return m_iHashId; }

	int Get_RenderTargetId() { return m_iRenderGroupId; }
	void Set_RenderTargetId(int id) { m_iRenderGroupId = id; }
	void Set_Selected(_bool bSelect) {m_bSelected = bSelect;}
	_bool Get_isLight() { return m_isLight; }
	_bool Get_isInstance() { return m_isInstance; }
	_bool Get_bShadow() { return m_bShadow; }
	void Set_bShadow(_bool bShadow) { m_bShadow = bShadow; }
	_tchar* Get_ModelTag() { return m_szModelTag; }

public:
	class CShader*	m_pShaderCom = { nullptr };
	class CModel*	m_pModelCom = { nullptr };

	NONMODEL_DESC	m_tDesc;

private:
	int m_iRenderGroupId = { 0 };

	static _int m_iStaticHashId;

	_int		m_iHashId;

	_bool		m_bSelected = { false };
	_bool		m_isLight = { false };
	_bool		m_isInstance = { false };
	_bool		m_bShadow = { false };

	_tchar m_szModelTag[MAX_PATH];

private:
	HRESULT Ready_Components(NONMODEL_DESC* pNonAnimDesc);

public:
	static CNonAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END