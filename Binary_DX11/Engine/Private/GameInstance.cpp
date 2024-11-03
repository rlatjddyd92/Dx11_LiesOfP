#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Picking_Manager.h"

#include "Key_Manager.h"
#include "Sound_Manager.h"
#include "UI_Manager.h"

#include "Event_Manager.h"

#include "Layer.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	/* 사운드 카드를 초기화하낟. */

	/* 입력장치를 초기화하낟. */
	m_pPicking_Manager = CPicking_Manager::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create(EngineDesc.hWnd);
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pUI_Manager = CUI_Manager::Create();
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	m_pEvent_Manager = CEvent_Manager::Create(m_pLevel_Manager, m_pObject_Manager, m_pUI_Manager); // << 이벤트매니저는 항상 마지막에 만드세요
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pKey_Manager->Update();	// 키 매니저의 업데이트는 항상 맨 처음에 해주자

	m_pObject_Manager->Priority_Update(fTimeDelta);	

	m_pPipeLine->Update();

	m_pPicking_Manager->Update();

	m_pObject_Manager->Update(fTimeDelta);
	
	m_pObject_Manager->Late_Update(fTimeDelta);

	//UIManager는 항상 Render 되기 전에 호출
	m_pUI_Manager->Update();
	
	m_pLevel_Manager->Update(fTimeDelta);		
}

HRESULT CGameInstance::Draw_Engine()
{
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

void CGameInstance::FinalUpdate_Engine()
{
	m_pEvent_Manager->Update();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨을 위해 존재하는 자원들은 해당 레벨이 끝나면 삭제하자. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 원형들도 레벨별로 관리했었다. */
	// m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

POINT CGameInstance::Get_MosePos()
{
	POINT      ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_pGraphic_Device->Get_Handle(), &ptMouse);

	return ptMouse;
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();
	

}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();
	
	
}

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);	
}

CLevel* CGameInstance::Get_CurLevel()
{
	return m_pLevel_Manager->Get_CurLevel();
}

_uint CGameInstance::Get_CurLevelIndex()
{
	return m_pLevel_Manager->Get_CurLevelIndex();
}

#pragma endregion

#pragma region OBJECT_MANAGER

HRESULT CGameInstance::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _char* strLayerTag, const _char* strPrototypeTag, void* pArg)
{
	_tchar szLayerTag[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strLayerTag, MAX_PATH, szLayerTag, MAX_PATH);

	_tchar szPrototypeTag[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strPrototypeTag, MAX_PATH, szPrototypeTag, MAX_PATH);

	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, szLayerTag, szPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);	
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

list<class CGameObject*>& CGameInstance::Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag)->Get_ObjectList();
}

CGameObject* CGameInstance::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Object(iLevelIndex, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Find_Prototype(const _wstring& strPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

CGameObject* CGameInstance::Find_Player()
{
	return m_pObject_Manager->Find_Player();
}

CGameObject* CGameInstance::Find_Camera()
{
	return m_pObject_Manager->Find_Camera();
}

#pragma endregion

#pragma region COMPONENT_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}

list<class CGameObject*>& CGameInstance::Get_RenderList(CRenderer::RENDERGROUP eGroup)
{
	return m_pRenderer->Get_RenderList(eGroup);
}

#pragma endregion

#pragma region PIPELINE

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}

#pragma endregion

#pragma region Picking_Manager

CGameObject* CGameInstance::Get_ModelPickedObject()
{
	return m_pPicking_Manager->Get_ModelPickedObject();
}

void CGameInstance::Transform_MouseRay_ToLocalSpace(const _fmatrix& WorldMatrix)
{
	m_pPicking_Manager->Transform_ToLocalSpace(XMMatrixInverse(nullptr, WorldMatrix));
}

_bool CGameInstance::isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut)
{
	return m_pPicking_Manager->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}

void CGameInstance::Add_ModelPickingList(CGameObject* pObj)
{
	return m_pPicking_Manager->Add_ModelPickingList(pObj);
}

#pragma endregion

#pragma region Key_Manager

KEY_STATE CGameInstance::Get_KeyState(KEY _eKey)
{
	return m_pKey_Manager->Get_KeyState(_eKey);
}

#pragma endregion

#pragma region SOUND_MANAGER

void CGameInstance::Play_Sound(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
	m_pSound_Manager->PlaySound(pSoundKey, eID, fVolume);
}

void CGameInstance::Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
	m_pSound_Manager->Play_SoundRepeat(pSoundKey, eID, fVolume);
}

void CGameInstance::Play_BGM(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->Play_BGM(pSoundKey, fVolume);
}

void CGameInstance::Stop_Sound(_uint eID)
{
	m_pSound_Manager->Stop_Sound(eID);
}

void CGameInstance::Stop_All()
{
	m_pSound_Manager->Stop_All();
}

void CGameInstance::Set_ChannelVolume(_uint eID, _float fVolume)
{
	m_pSound_Manager->Set_ChannelVolume(eID, fVolume);
}

void CGameInstance::Set_PlayeSpeed(_uint eID, _float fSpeedRatio)
{
	m_pSound_Manager->Set_PlayeSpeed(eID, fSpeedRatio);
}

void CGameInstance::Pause(_uint eID)
{
	m_pSound_Manager->Pause(eID);
}

_bool CGameInstance::IsPlaying(_uint eID)
{
	return m_pSound_Manager->IsPlaying(eID);
}

HRESULT CGameInstance::Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize, void* pArg)
{
	CEvent_Manager::EVENT tEvent = {};
	tEvent.eEven = CEvent_Manager::EVENT_TYPE::CREATE_OBJECT;	//생성 이벤트 

	// 클론할 정보들 채워넣기
	CEvent_Manager::CLONE_DESC* pCloneDesc = new CEvent_Manager::CLONE_DESC;
	pCloneDesc->iLevelIndex = iLevelIndex;	// 어디 레벨?
	pCloneDesc->strLayerTag = strLayerTag;	// 어디 레이어?
	pCloneDesc->strPrototypeTag = strPrototypeTag; // 프로토타입 태그 뭐야?

	if (nullptr != pArg)
	{
		// 이 void*는 객체를 Clone하면서 Initizlie에 넘겨줄 정보가 필요하면 저장해주는 용도
		// 근데 구조체 정보 많고 넘겨줘야할 정보가 모두 다름 -> 근데 프레임이 밀리기 때문에 함수 내에 생성하면 사라짐
		// 근데 어떻게 정보를 유지하면서 넘겨주지? -> 아하 그냥 포인터로 쓰면 되겠다 -> 근데 void*인데 어떻게 넘겨주지?
		// 아하 그럼 사이즈도 매개변수로 받아와서 1byte단위인 char로 초기화하고 메모리 자체를 복사해버리자!!
		
		// 받아온 사이를 이용해서 pArg 생성 
		pCloneDesc->pArg = new char[iSize];

		// 메모리 채로 복사
		memcpy(pCloneDesc->pArg, pArg, iSize);
	}

	tEvent.CloneDesc = pCloneDesc;

	m_pEvent_Manager->AddEvent(tEvent);

	return S_OK;
}

HRESULT CGameInstance::Event_DestoryObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	CEvent_Manager::EVENT tEvent = {};
	tEvent.eEven = CEvent_Manager::EVENT_TYPE::DELETE_OBJECT;	// 삭제 이벤트야

	// 일단 있어야만 해서 생성함 신경X
	CEvent_Manager::CLONE_DESC* pCloneDesc = new CEvent_Manager::CLONE_DESC;
	tEvent.lParam = (DWORD_PTR)pGameObject;

	tEvent.CloneDesc = pCloneDesc;

	m_pEvent_Manager->AddEvent(tEvent);

	return S_OK;
}

#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);

	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	__super::Free();

}
