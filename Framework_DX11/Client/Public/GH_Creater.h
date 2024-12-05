#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CGH_Creater final : public CGameObject
{
public:
	typedef struct
	{
		class CMonster* pManus = { nullptr };
	}CREATER_DESC;

private:
	CGH_Creater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGH_Creater(const CGameObject& Prototype);
	virtual ~CGH_Creater() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

	class CTransform*	m_pCopyPlayerTransformCom = { nullptr };
	class CMonster*		m_pManus = { nullptr };

	_float		m_fGodHandsDelay = {};
	_float		m_fStackTime = {};


public:
	static CGH_Creater* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END