#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	_bool								IsActive() { return m_isActive; }
	void								IsActive(_bool isActive) { m_isActive = isActive; }

	class CGameObject*					Get_GameObject() { return m_pOwner; }
	void								Set_Owner(class CGameObject* pObject) { m_pOwner = pObject; }

	unordered_set<class CGameObject*>&	GetCollisionSet() { return m_ColliderObjects; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float4x4* pWorldMatrix);

	void	Change_BoundingDesc(void* pBoundingDesc);
	void	Reset_Bounding();

#ifdef _DEBUG
	virtual HRESULT Render() override; 
#endif

public:
	_bool Intersect(CCollider* pTargetCollider);

private:
	class CBounding*					m_pBounding = { nullptr };
	class CGameObject*					m_pOwner = { nullptr };

	TYPE								m_eColliderType = {};

	_bool								m_isActive = { true };
	_bool								m_isIntersect = { false };

	_wstring							m_strColliderTag = TEXT("");

	unordered_set<class CGameObject*>	m_ColliderObjects;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*			m_pBatch = { nullptr };
	BasicEffect*									m_pEffect = { nullptr };
	ID3D11InputLayout*								m_pInputLayout = { nullptr };


#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END