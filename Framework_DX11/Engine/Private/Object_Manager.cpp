#include "..\Public\Object_Manager.h"

#include "Layer.h"
#include "GameObject.h"
#include "Camera.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
	{
		Safe_Release(pPrototype);
		return E_FAIL;
	}

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	/* 복제해야할 원형을 차즌ㄴ다. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* 그 원형을 복제하여 사본 객체를 생성한다. */
	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	/* 객체들은 레잉어로 묶어서 관리하고 있었거든 */
	/* 사본을 추가하기위한 레이어를 찾자.*/
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 야 그 레이어가 없는디? */
	/* 내가 추가하려고했던 레이어가 아직없었다 == 처음 추가하는 객체였다. */
	/* 새롱누 렝이ㅓ를 만들어야지. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);	
		LayerTagList.push_back(strLayerTag);
	}
	else /* 내가 추가하려고 하는 레잉어가 만들어져있었어. */
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_Object_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else /* 내가 추가하려고 하는 레잉어가 만들어져있었어. */
		pLayer->Add_GameObject(pGameObject);

	// 이걸 쓸까??
	//pGameObject->Set_LayerTag(strLayerTag);

	return S_OK;
}

HRESULT CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Priority_Update(fTimeDelta);	
		}	
	}
	return S_OK;
}

HRESULT CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}

	return S_OK;
}

HRESULT CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}

	return S_OK;
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

CGameObject * CObject_Manager::Clone_GameObject(const _wstring & strPrototypeTag, void * pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

CComponent * CObject_Manager::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component(strComponentTag, iIndex, iPartObjIndex);	
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CGameObject * CObject_Manager::Find_Prototype(const _wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);
	if(iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CGameObject* CObject_Manager::Find_Player(_uint iLevelIndex)
{
	CGameObject* pPlayer = Find_Object(iLevelIndex, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
		return nullptr;

	return pPlayer;
}

CCamera* CObject_Manager::Find_Camera(_uint iLevelIndex)
{
	CGameObject* pCameraObject = Find_Object(iLevelIndex, TEXT("Layer_Camera"), 0);
	if (nullptr == pCameraObject)
		return nullptr;

	CCamera* pCamera = dynamic_cast<CCamera*>(pCameraObject);

	return pCamera;
}

CGameObject* CObject_Manager::Find_NearObject(CGameObject* pOwner, _uint iLevelIndex, const _wstring& strLayerTag)
{
	if (nullptr == Find_Layer(iLevelIndex, strLayerTag))
		return nullptr;

	list<CGameObject*> ObjectList = Find_Layer(iLevelIndex, strLayerTag)->Get_ObjectList();

	if (ObjectList.empty() || ObjectList.size() <= 0)
		return nullptr;

	CGameObject* pNearObject = { nullptr };

	_vector vDistance = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float  fDistance = -1.f;

	for (auto& Object : ObjectList)
	{
		if (!Object->IsActive())
			continue;

		_vector vOwnerPos = pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vFindObjPos = Object->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		vDistance = (vOwnerPos - vFindObjPos);

		if (fDistance < 0.f)
		{
			pNearObject = Object;

			XMStoreFloat(&fDistance, XMVector3Length(vDistance));
			continue;
		}

		_float fNewDistance;
		XMStoreFloat(&fNewDistance, XMVector3Length(vDistance));

		if (fNewDistance < fDistance)
		{
			pNearObject = Object;
			fDistance = fNewDistance;
		}
	}

	return pNearObject;
}

_uint CObject_Manager::Get_Object_Layer_Count(_uint iLevelIndex)
{
	return (_uint)m_pLayers[iLevelIndex].size();
}

_wstring CObject_Manager::Get_LayerTag(_uint iLevelIndex, _uint index)
{
	return  LayerTagList[index];
}

CGameObject* CObject_Manager::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Object(iIndex);
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])		
			Safe_Release(Pair.second);
		m_pLayers[i].clear();		
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();
}
