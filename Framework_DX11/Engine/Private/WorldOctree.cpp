#include "WorldOctree.h"
#include "GameInstance.h"
#include "GameObject.h"

_int CWorldOctree::iStaticOctreeIndex = 0;

CWorldOctree::CWorldOctree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CWorldOctree::Initialize(_Vec3 _vMinPos, _Vec3 _vMaxPos, vector<class CGameObject*>& m_pParentObjList, _int* iDepth)
{
#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t			iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	//정육면체 바운딩 박스 생성
	_Vec3 vCenter = { (_vMinPos.x + _vMaxPos.x) * 0.5f ,(_vMinPos.y + _vMaxPos.y) * 0.5f ,(_vMinPos.z + _vMaxPos.z) * 0.5f };

	//가장 긴 변을 기준으로 박스 생성
	_Vec3 vExtend = { abs(_vMinPos.x - _vMaxPos.x),abs(_vMinPos.y - _vMaxPos.y),abs(_vMinPos.z - _vMaxPos.z) };
	_float fLongestExtend = max(max(vExtend.x, vExtend.y), vExtend.z);
	_float fHalfExtend = fLongestExtend * 0.5f;

	m_pBoundBox = new BoundingBox(vCenter, _Vec3(fHalfExtend, fHalfExtend, fHalfExtend));

	CBounding_AABB::BOUNDING_AABB_DESC Desc = {};
	Desc.vCenter = vCenter;
	Desc.vExtents = _Vec3(fHalfExtend, fHalfExtend, fHalfExtend);
	m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, &Desc);

	m_fRadius = fHalfExtend *sqrt(3.0f) ;

	if(*iDepth != -1)
		m_iDepth = *iDepth;

	if (*iDepth == -100 )	//처음 노드 생성할 때
	{
		m_iDepth = 3;
	}

	//옥트리 생성
	Build_WorldOctree(m_pParentObjList);

	if (m_iDepth <= 0 || m_iObjectCount <= 10)	//자식 생성 멈춤
	{
		m_bHaveChild = false;
	}
	else
		m_bHaveChild = true;

	if(m_bHaveChild)
	{
		for (int i = 0; i < 8; ++i)
			m_pChildren[i] = nullptr;

		//윗층
		m_pChildren[0] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x - fHalfExtend, vCenter.y, vCenter.z), _Vec3(vCenter.x, vCenter.y + fHalfExtend, vCenter.z + fHalfExtend), m_pObjectsList, &m_iDepth);
		m_pChildren[1] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x, vCenter.y, vCenter.z), _Vec3(vCenter.x + fHalfExtend, vCenter.y + fHalfExtend, vCenter.z + fHalfExtend), m_pObjectsList, &m_iDepth);
		m_pChildren[2] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x - fHalfExtend, vCenter.y, vCenter.z - fHalfExtend), _Vec3(vCenter.x, vCenter.y + fHalfExtend, vCenter.z), m_pObjectsList, &m_iDepth);
		m_pChildren[3] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x, vCenter.y, vCenter.z - fHalfExtend), _Vec3(vCenter.x + fHalfExtend, vCenter.y + fHalfExtend, vCenter.z), m_pObjectsList, &m_iDepth);
		//아랫층								
		m_pChildren[4] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x - fHalfExtend, vCenter.y - fHalfExtend, vCenter.z), _Vec3(vCenter.x, vCenter.y, vCenter.z + fHalfExtend), m_pObjectsList, &m_iDepth);
		m_pChildren[5] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x, vCenter.y - fHalfExtend, vCenter.z), _Vec3(vCenter.x + fHalfExtend, vCenter.y, vCenter.z + fHalfExtend), m_pObjectsList, &m_iDepth);
		m_pChildren[6] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x - fHalfExtend, vCenter.y - fHalfExtend, vCenter.z - fHalfExtend), _Vec3(vCenter.x, vCenter.y, vCenter.z), m_pObjectsList, &m_iDepth);
		m_pChildren[7] = CWorldOctree::Create(m_pDevice, m_pContext, _Vec3(vCenter.x, vCenter.y - fHalfExtend, vCenter.z - fHalfExtend), _Vec3(vCenter.x + fHalfExtend, vCenter.y, vCenter.z), m_pObjectsList, &m_iDepth);
	}

	return 		S_OK;
}

void CWorldOctree::Culling(CGameInstance* pGameInstance, unordered_set<_int>& pFrustumCulledNodes)
{
	//절두체에 걸리는지 확인
	if (!pGameInstance->isIn_Frustum_WorldSpace(XMLoadFloat3(&m_pBoundBox->Center), m_fRadius ))
		return;

	_Vec3 vCorners[8];
	m_pBoundBox->GetCorners(vCorners);

	//바운드 박스가 절두체에 얼마나 들어가는지 확인
	_bool isIn[8] = { false };
	_bool anyIn = false;
	_bool allIn = true;

	//_float fZero = 0.f;
	for (int i = 0; i < 8; ++i) {
		isIn[i] = pGameInstance->isIn_Frustum_WorldSpace(vCorners[i], m_fRadius);
		if (isIn[i])
		{
			anyIn = true;
		}
		else
		{
			allIn = false;
		}
	}

	if (allIn && m_iOctreeIndex != 0) {
		// 모두 포함된 경우
 		pFrustumCulledNodes.insert(m_iOctreeIndex);
	}
	else if (anyIn ) 	// 일부 포함된 경우
	{
		for (int i = 0; i < 8; i++)
		{
			if (m_bHaveChild)
				m_pChildren[i]->Culling(pGameInstance, pFrustumCulledNodes);
			else
 				pFrustumCulledNodes.insert(m_iOctreeIndex);
		}
	}

	return;
}

void CWorldOctree::Build_WorldOctree(vector<class CGameObject*>& m_pParentObjList)
{
	m_iDepth--;
	m_iOctreeIndex = iStaticOctreeIndex;
	iStaticOctreeIndex++;

	for (size_t i = 0; i < m_pParentObjList.size(); ++i)
	{
		CModel* pModel = static_cast<CModel*>(m_pParentObjList[i]->Find_Component(MODEL));
		if (pModel == nullptr)
			continue;

		BoundingBox ObjAABB = CalculateObjAABB(m_pParentObjList[i] ,pModel);

		//물체가 바운드 박스와 충돌하면 이 노드에 담는다.
		if (IsIntersactWithBoundingBox(ObjAABB)) {
			m_pObjectsList.push_back(m_pParentObjList[i]);
			m_pParentObjList[i]->Add_WorldOctreeIndex(m_iOctreeIndex);				//물체에 인덱스 넣기
			m_iObjectCount++;
		}
	}
}

BoundingBox CWorldOctree::CalculateObjAABB(CGameObject* pObj, CModel* pModel)
{
	_Vec3 MinPos = pModel->Get_Model_MinSize();
	_Vec3 MaxPos = pModel->Get_Model_MaxSize();

	//바운딩 박스 생성
	_Vec3 vCenter = { pObj->Get_Transform()->Get_State(CTransform::STATE_POSITION)};
	_Vec3 vExtend = { abs(MinPos.x - MaxPos.x) * 0.5f,abs(MinPos.y - MaxPos.y) * 0.5f,abs(MinPos.z - MaxPos.z) * 0.5f };

	return BoundingBox(vCenter, vExtend);
}

_bool CWorldOctree::IsIntersactWithBoundingBox(BoundingBox ObjBoundingBox)
{
	return m_pBoundBox->Intersects(ObjBoundingBox);
}

#ifdef _DEBUG
HRESULT CWorldOctree::Render()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	for (int i = 0; i < 8; i++)
	{
		if (m_pChildren[i])
			m_pChildren[i]->Render();
	}

	return S_OK;
}
#endif

CWorldOctree* CWorldOctree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _Vec3 _vMinPos, _Vec3 _vMaxPos, vector<class CGameObject*>& m_pParentObjList,  _int* iDepth)
{
	CWorldOctree* pInstance = new CWorldOctree(pDevice, pContext);

	if (FAILED(pInstance->Initialize(_vMinPos, _vMaxPos, m_pParentObjList, iDepth)))
	{
		/*MSG_BOX(TEXT("Failed to Cloned : CWorldOctree"));
		Safe_Release(pInstance);*/
		return nullptr;
	}

	return pInstance;
}

void CWorldOctree::Free()
{
	__super::Free();

	if (m_pBoundBox)
		delete m_pBoundBox;
	m_pBoundBox = nullptr;

	for (int i = 0; i < 8; ++i) {
		Safe_Release(m_pChildren[i]);
	}

	Safe_Release(m_pBounding);

#ifdef _DEBUG
	if (m_pBatch != nullptr)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
