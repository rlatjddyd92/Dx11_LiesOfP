#pragma once

#include "Tools_Defines.h"
#include "EasyDrawSphere.h"
#include "DebugDraw.h"

BEGIN(Tools)

class CTargetBall final : public CEasyDrawSphere
{
public:
	typedef struct : public CEasyDrawSphere::EASYDRAWSPHERE_DESC
	{
		_bool* pUpdateCtr{ nullptr };
		_bool* pRenderCtr{ nullptr };
	}TARGETBALL_DESC;

private:
	CTargetBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTargetBall(const CTargetBall& Prototype);
	virtual ~CTargetBall() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual class CComponent* Change_Component(const _wstring& strComponentTag, CComponent* pComponent, _uint iPartObjIndex = 0) { return nullptr; }

private:
	TARGETBALL_DESC	m_tDesc;

public:
	static CTargetBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END