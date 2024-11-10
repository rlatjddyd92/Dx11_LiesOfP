#pragma once

#include "Engine_Defines.h"
#include "GameObject.h"
#include "DebugDraw.h"


BEGIN(Engine)

class ENGINE_DLL CEasyDrawSphere abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vCenter		= {};
		_float fRadius		= {};
		_vector* pPos		= { nullptr };
	}EASYDRAWSPHERE_DESC;

protected:
	CEasyDrawSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEasyDrawSphere(const CEasyDrawSphere& Prototype);
	virtual ~CEasyDrawSphere() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual class CComponent* Change_Component(const _wstring& strComponentTag, CComponent* pComponent, _uint iPartObjIndex = 0) { return nullptr; }

private:
	class PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	class BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

	EASYDRAWSPHERE_DESC	m_tDesc;

	_bool						m_isCloned = { false };

private:
	BoundingSphere* m_pOriginalBoundingDesc = { nullptr };
	BoundingSphere* m_pBoundingDesc = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END