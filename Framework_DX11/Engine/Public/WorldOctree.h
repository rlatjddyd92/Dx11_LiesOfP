#pragma once

#include "Base.h"

BEGIN(Engine)

class CWorldOctree final : public CBase
{
private:
	CWorldOctree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWorldOctree() = default;

public:
	HRESULT Initialize(_Vec3 _vMinPos, _Vec3 _vMaxPos, vector<class CGameObject*>& m_pParentObjList, _int* iDepth = nullptr);
	
public:
	void Culling(CGameInstance* pGameInstance, unordered_set<_int>& pFrustumCulledNodes);
	_bool Get_haveChild() { return m_bHaveChild; }

#ifdef _DEBUG
	virtual HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	vector<class CGameObject*> m_pObjectsList;
	CWorldOctree* m_pChildren[8] = { nullptr, };		//자식 노드
	BoundingBox* m_pBoundBox;			//현재 노드 AABB
	class CGameInstance* m_pGameInstance = { nullptr };

	static int  iStaticOctreeIndex;
	_int m_iOctreeIndex = { 0 };
	_int m_iDepth = { -1 };				//노드 깊이
	_int m_iObjectCount = { 0 };		//노드에 담긴 오브젝트 개수

	_float m_fRadius = { 0.f };

	_bool m_bHaveChild = { false };
	class CBounding* m_pBounding = { nullptr };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

private:
	void Build_WorldOctree(vector<class CGameObject*>& m_pParentObjList);
	BoundingBox CalculateObjAABB(CGameObject* pObj, CModel* pModel);
	_bool IsIntersactWithBoundingBox(BoundingBox ObjBoundingBox);

public:
	static CWorldOctree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _Vec3 _vMinPos, _Vec3 _vMaxPos, vector<class CGameObject*>& m_pParentObjList, _int* iDepth = nullptr);
	virtual void Free() override;
};


END