#pragma once

#include "Base.h"

BEGIN(Engine)

class COctree final : public CBase
{
private:
	COctree();
	virtual ~COctree() = default;

public:
	HRESULT Initialize(_Vec3 _vMinPos, _Vec3 _vMaxPos, _uint* pIndices, VTXMESH* pVertexes, _uint iNumTri, vector<int>& ParentTrigangleIndexes, _int* iDepth = nullptr);
	void Culling(CGameInstance* pGameInstance, _uint* pOriIndices, _uint* pNewIndices, unordered_set<_int>& pDrawTriIndexes, _uint* pNumIndices); //게임 인스턴스, 인덱스 참조할 원본, 새로 그릴 인덱스들, 인덱스 갯수
	_bool Get_haveChild() { return m_bHaveChild; }
private:
	//vector<_int[3]> m_iTriangle;	//노드에 포함한 삼각형
	vector<_int> m_TrianglgeIndexes;	//노드에 포함한 삼각형의 인덱스
	COctree* m_pChildren[8] = {nullptr, };		//자식 노드
	BoundingBox* m_pBoundBox;			//현재 노드 AABB
	_int m_iDepth = { -1 };				//노드 깊이, 모델 사이즈에 따라 유동적으로 하는게 좋을 듯
	_uint m_iNumFaces = { 0 };
	_uint* m_pIndices = { nullptr };
	_float	m_fRadius = { 0.f };
	_bool m_bHaveChild = { false };
private:
	void Build_Octree(_uint* indices, VTXMESH* pVertexes, _uint iNumTri);
	void Build_Octree(vector<int> ParentTriangleIndexes, VTXMESH* pVertexes, _uint* indices, _uint iNumTri);
	BoundingBox calculateTriangleAABB(_Vec3 vPos1, _Vec3 vPos2, _Vec3 vPos3);
	_bool IsIntersactWithBoundingBox(BoundingBox triBoundingBox);

public:
	static COctree* Create(_Vec3 _vMinPos, _Vec3 _vMaxPos, _uint* pIndices, VTXMESH* pVertexes, _uint iNumTri, vector<int>& ParentTrigangleIndexes, _int* iDepth = nullptr);
	virtual void Free() override;
};


END