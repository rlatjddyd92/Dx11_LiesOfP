#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CModel;
class CShader;
END

BEGIN(Client)
class CBossDoor_Veli final : public CGameObject
{
public:
	typedef struct :CGameObject::GAMEOBJECT_DESC
	{
		_Vec3 vPos;
		_Vec3 vScale;
		_Vec3 vRotation;
	}BOSSDOOR_DESC;

	enum TEXTURE_TYPE{ DIFFUSE, MASK_1, MASK_2, DISSOLVE, TYPE_END };
	enum DOOR_STATE{ DOOR_OPEN, DOOR_DEFAULT, DOOR_CLOSE, DOOR_END };

private:
	CBossDoor_Veli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossDoor_Veli(const CBossDoor_Veli& Prototype);
	virtual ~CBossDoor_Veli() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_State(DOOR_STATE eState) {
		m_eDoorState = eState;
	}

private:
	class CTexture* m_pTextureCom[TYPE_END] = { nullptr, nullptr, nullptr, nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };

	_Vec4 m_vColor = {};
	_Vec2 m_vTileRepeat = {};
	_Vec2 m_vTileMove = {};
	_float m_fAlpha = { 0.f };
	_float m_fAccumulateTime = { 0.f };

	DOOR_STATE m_eDoorState = { DOOR_END };

private:
	HRESULT Ready_Components();

public:
	static CBossDoor_Veli* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBossDoor_Veli* Clone(void* pArg);
	virtual void Free() override;
};
END
