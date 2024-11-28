#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CLadder :
	public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_char		szModelTag[MAX_PATH];
		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;
		_bool		bShadow = { false };
		_uint		iRenderGroupID = { true };
	}LADDER_DESC;

	enum COLLIDERTYPE { UP, DOWN, COLLIDER_END };
protected:
	CLadder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLadder(const CLadder& Prototype);
	virtual ~CLadder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const _Vec3 Get_LadderUpPos();
	const _Vec3 Get_LadderDonwPos();

private:
	CCollider* m_pColliderCom[COLLIDER_END] = {nullptr, nullptr};
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
private:
	_bool m_bShadow = false;
private:
	HRESULT Ready_Components(LADDER_DESC* Desc);

public:
	static CLadder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END