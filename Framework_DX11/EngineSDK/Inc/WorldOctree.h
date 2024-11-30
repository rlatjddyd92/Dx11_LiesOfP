#pragma once

#include "Base.h"

BEGIN(Engine)

class CWorldOctree final : public CBase
{
private:
	CWorldOctree();
	virtual ~CWorldOctree() = default;

public:
	HRESULT Initialize(_Vec3 _vMinPos, _Vec3 _vMaxPos, vector<class CGameObject*>& m_pParentObjList, _int* iDepth = nullptr);
	
public:
	void Culling(CGameInstance* pGameInstance, unordered_set<_int>& pFrustumCulledNodes);
	_bool Get_haveChild() { return m_bHaveChild; }

private:
	vector<class CGameObject*> m_pObjectsList;
	CWorldOctree* m_pChildren[8] = { nullptr, };		//자식 노드
	BoundingBox* m_pBoundBox;			//현재 노드 AABB

	static int  iStaticOctreeIndex;
	_int m_iOctreeIndex = { 0 };
	_int m_iDepth = { -1 };				//노드 깊이
	_int m_iObjectCount = { 0 };		//노드에 담긴 오브젝트 개수

	_float m_fRadius = { 0.f };

	_bool m_bHaveChild = { false };

private:
	void Build_WorldOctree(vector<class CGameObject*>& m_pParentObjList);
	BoundingBox CalculateObjAABB(CModel* pModel);
	_bool IsIntersactWithBoundingBox(BoundingBox ObjBoundingBox);

public:
	static CWorldOctree* Create(_Vec3 _vMinPos, _Vec3 _vMaxPos, vector<class CGameObject*>& m_pParentObjList, _int* iDepth = nullptr);
	virtual void Free() override;
};


END