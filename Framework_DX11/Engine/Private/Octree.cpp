#include "Octree.h"
#include "GameInstance.h"

COctree::COctree()
{
}

HRESULT COctree::Initialize(_Vec3 _vMinPos, _Vec3 _vMaxPos, _uint* pIndices,  VTXMESH* pVertexes,  _uint iNumTri, vector<int>& ParentTrigangleIndexes, _int* iDepth)
{
	//바운딩 박스 생성
	_Vec3 vCenter = { (_vMinPos.x + _vMaxPos.x) * 0.5f ,(_vMinPos.y + _vMaxPos.y) * 0.5f ,(_vMinPos.z + _vMaxPos.z) * 0.5f };
	_Vec3 vExtend = { abs(_vMinPos.x - _vMaxPos.x),abs(_vMinPos.y - _vMaxPos.y),abs(_vMinPos.z - _vMaxPos.z) };
	m_pBoundBox =new BoundingBox(vCenter, vExtend);

	m_fRadius = XMVectorGetX(XMVector3Length(_vMinPos - m_pBoundBox->Center));

	//if(*iDepth != -1)
	//	m_iDepth = *iDepth;

	//if (m_iDepth == -1)	//처음 노드 생성할 때
	//{
	//	m_iDepth = (int)(vExtend.x / 50.f) + 1;					//임시로 수치 지정
	//	Build_Octree(pIndices, pVertexes, iNumTri);
	//}
	//else
	//	Build_Octree(ParentTrigangleIndexes, pVertexes, pIndices, iNumTri);

	//if (m_iDepth <= 0)
	//	return 		S_OK;	
	
	if (iNumTri <= 10000)
		return 		S_OK;
	else
		m_bHaveChild = true;

	if (m_iDepth == -1)	//처음 노드 생성할 때
	{
		m_iDepth = (int)(vExtend.x / 50.f) + 1;					//임시로 수치 지정
		Build_Octree(pIndices, pVertexes, iNumTri);
	}
	else
		Build_Octree(ParentTrigangleIndexes, pVertexes, pIndices, iNumTri);

	//윗층
	m_pChildren[0] = COctree::Create(_Vec3(_vMinPos.x, _vMinPos.y + vExtend.y * 0.5f, _vMinPos.z + vExtend.z * 0.5f), _Vec3(_vMinPos.x + vExtend.x*0.5f, _vMaxPos.y, _vMaxPos.z), pIndices, pVertexes, m_iNumFaces, m_TrianglgeIndexes ,&m_iDepth);
	m_pChildren[1] = COctree::Create(_Vec3(_vMinPos.x + vExtend.x * 0.5f, _vMinPos.y + vExtend.y * 0.5f, _vMinPos.z + vExtend.z * 0.5f), _Vec3(_vMaxPos.x, _vMaxPos.y, _vMaxPos.z), pIndices, pVertexes, m_iNumFaces,  m_TrianglgeIndexes, &m_iDepth);
	m_pChildren[2] = COctree::Create(_Vec3(_vMinPos.x, _vMinPos.y + vExtend.y * 0.5f, _vMinPos.z), _Vec3(_vMinPos.x + vExtend.x * 0.5f, _vMaxPos.y, _vMinPos.z + vExtend.z * 0.5f), pIndices, pVertexes, m_iNumFaces,  m_TrianglgeIndexes, &m_iDepth);
	m_pChildren[3] = COctree::Create(_Vec3(_vMinPos.x + vExtend.x * 0.5f, _vMinPos.y + vExtend.y * 0.5f, _vMinPos.z), _Vec3(_vMaxPos.x, _vMaxPos.y, _vMinPos.z + vExtend.z * 0.5f), pIndices, pVertexes, m_iNumFaces,  m_TrianglgeIndexes, &m_iDepth);
	//아랫층
	m_pChildren[4] = COctree::Create(_Vec3(_vMinPos.x, _vMinPos.y , _vMinPos.z + vExtend.z * 0.5f), _Vec3(_vMinPos.x + vExtend.x * 0.5f, _vMinPos.y + vExtend.y * 0.5f, _vMaxPos.z), pIndices, pVertexes, m_iNumFaces, m_TrianglgeIndexes,  &m_iDepth);
	m_pChildren[5] = COctree::Create(_Vec3(_vMinPos.x + vExtend.x * 0.5f, _vMinPos.y, _vMinPos.z + vExtend.z * 0.5f), _Vec3(_vMaxPos.x, _vMinPos.y + vExtend.y * 0.5f, _vMaxPos.z), pIndices, pVertexes, m_iNumFaces,m_TrianglgeIndexes, &m_iDepth);
	m_pChildren[6] = COctree::Create(_Vec3(_vMinPos.x, _vMinPos.y, _vMinPos.z), _Vec3(_vMinPos.x + vExtend.x * 0.5f, _vMinPos.y + vExtend.y * 0.5f, _vMinPos.z + vExtend.z * 0.5f), pIndices, pVertexes, m_iNumFaces,m_TrianglgeIndexes, &m_iDepth);
	m_pChildren[7] = COctree::Create(_Vec3(_vMinPos.x + vExtend.x * 0.5f, _vMinPos.y, _vMinPos.z), _Vec3(_vMaxPos.x, _vMinPos.y + vExtend.y * 0.5f, _vMinPos.z + vExtend.z * 0.5f), pIndices, pVertexes, m_iNumFaces,m_TrianglgeIndexes, &m_iDepth);

    return 		S_OK;
}

void COctree::Culling(CGameInstance* pGameInstance, _uint* pOriIndices, _uint* pNewIndices, unordered_set<_int>& pDrawTriIndexes, _uint* pNumIndices)
{
	//절두체에 걸리는지 확인
	if (!pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&m_pBoundBox->Center), m_fRadius))
		return;
	
	_Vec3 vCorners[8] ;
	m_pBoundBox->GetCorners(vCorners);

	//바운드 박스가 절두체에 얼마나 들어가는지 확인
	_bool isIn[8];
	_bool anyIn = false;
	_bool allIn = true;
    for (int i = 0; i < 8; ++i) {
        isIn[i] = pGameInstance->isIn_Frustum_LocalSpace(vCorners[i], m_fRadius);
        if (isIn[i]) anyIn = true;
        else allIn = false;
    }

	if (allIn) {
		// 모두 포함된 경우
		for (int i = 0; i < m_iNumFaces; ++i)
		{
			//기존에 그린다고 담아둔 면인지 확인
			if (pDrawTriIndexes.find(m_TrianglgeIndexes[i]) == pDrawTriIndexes.end()) {
				// 삼각형 추가
				pNewIndices[(*pNumIndices)++] = pOriIndices[m_TrianglgeIndexes[i] * 3];
				pNewIndices[(*pNumIndices)++] = pOriIndices[m_TrianglgeIndexes[i] * 3 + 1];
				pNewIndices[(*pNumIndices)++] = pOriIndices[m_TrianglgeIndexes[i] * 3 + 2];
				pDrawTriIndexes.insert(m_TrianglgeIndexes[i]);
			}
		}
	}
	else if (anyIn) 	// 일부 포함된 경우
	{
		for (int i = 0; i < 8; i++)
		{
			if (nullptr != m_pChildren[i])
				m_pChildren[i]->Culling(pGameInstance, pOriIndices, pNewIndices, pDrawTriIndexes, pNumIndices);
		}
	}

	return;		
}

void COctree::Build_Octree(_uint* indices, VTXMESH* pVertexes, _uint iNumTri)
{
	m_iDepth--;

	for (int i = 0; i < iNumTri; ++i)
	{
		const _Vec3& v1 = pVertexes[indices[i * 3]].vPosition;
		const  _Vec3& v2 = pVertexes[indices[i * 3 + 1]].vPosition;
		const  _Vec3& v3 = pVertexes[indices[i * 3 + 2]].vPosition;
		BoundingBox triAABB = calculateTriangleAABB(v1, v2, v3);

		//삼각형이 바운드 박스와 충돌하면 이 노드에 담는다.
		if (IsIntersactWithBoundingBox(triAABB)) {
			m_TrianglgeIndexes.push_back(i);
			m_iNumFaces++;
		}
	}
}

void COctree::Build_Octree(vector<int> ParentTriangleIndexes, VTXMESH* pVertexes, _uint* indices, _uint iNumTri)
{
	m_iDepth--;

	for (int i = 0; i < iNumTri; ++i)
	{
		const _Vec3& v1 = pVertexes[indices[ParentTriangleIndexes[i] * 3]].vPosition;
		const  _Vec3& v2 = pVertexes[indices[ParentTriangleIndexes[i] * 3 + 1]].vPosition;
		const  _Vec3& v3 = pVertexes[indices[ParentTriangleIndexes[i] * 3 + 2]].vPosition;
		BoundingBox triAABB = calculateTriangleAABB(v1, v2, v3);

		//삼각형이 바운드 박스와 충돌하면 이 노드에 담는다.
		if (IsIntersactWithBoundingBox(triAABB)) {
			m_TrianglgeIndexes.push_back(ParentTriangleIndexes[i]);
			m_iNumFaces++;
		}
	}
}

BoundingBox COctree::calculateTriangleAABB(_Vec3 vPos1, _Vec3 vPos2, _Vec3 vPos3)
{
	_Vec3 MinPos;
	_Vec3 MaxPos;

	MinPos.x = min(min(vPos1.x, vPos2.x), vPos3.x);
	MinPos.y = min(min(vPos1.y, vPos2.y), vPos3.y);
	MinPos.z = min(min(vPos1.z, vPos2.z), vPos3.z);

	MaxPos.x = max(max(vPos1.x, vPos2.x), vPos3.x);
	MaxPos.y = max(max(vPos1.y, vPos2.y), vPos3.y);
	MaxPos.z = max(max(vPos1.z, vPos2.z), vPos3.z);

	//바운딩 박스 생성
	_Vec3 vCenter = { (MinPos.x + MaxPos.x) * 0.5f ,(MinPos.y + MaxPos.y) * 0.5f ,(MinPos.z + MaxPos.z) * 0.5f };
	_Vec3 vExtend = { abs(MinPos.x - MaxPos.x),abs(MinPos.y - MaxPos.y),abs(MinPos.z - MaxPos.z) };

	return BoundingBox(vCenter, vExtend);
}

_bool COctree::IsIntersactWithBoundingBox(BoundingBox triBoundingBox)
{
	return m_pBoundBox->Intersects(triBoundingBox);
}


COctree* COctree::Create(_Vec3 _vMinPos, _Vec3 _vMaxPos, _uint* pIndices, VTXMESH* pVertexes, _uint iNumTri, vector<int>& ParentTrigangleIndexes, _int* iDepth)
{
	COctree* pInstance = new COctree();

	if (FAILED(pInstance->Initialize(_vMinPos, _vMaxPos, pIndices, pVertexes, iNumTri, ParentTrigangleIndexes, iDepth)))
	{
		MSG_BOX(TEXT("Failed to Cloned : COctree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctree::Free()
{
	__super::Free();

	if (m_pBoundBox)
		delete m_pBoundBox;
	m_pBoundBox = nullptr;

	for (int i = 0; i < 8; ++i) {
		Safe_Release(m_pChildren[i]);
	}
}
