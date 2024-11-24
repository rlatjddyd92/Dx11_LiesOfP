#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Picking.h"
#include "Frustum.h"
#include "Layer.h"

#include "Collider_Manager.h"
#include "Key_Manager.h"
#include "PhysX_Manager.h"
#include "Sound_Manager.h"

// 2024-11-06 김성용
#include "CSVFile_Manager.h"

//정승현
#include "Instance_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	// srand(unsigned(time(nullptr)));

	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;


	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	/* 사운드 카드를 초기화하낟. */

	/* 입력장치를 초기화하낟. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	//m_pPicking = CPicking::Create(*ppDevice, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	//if (nullptr == m_pPicking)
	//	return E_FAIL;

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

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;


	/* 추가한 기능들 */
	m_pCollider_Manager = CCollider_Manager::Create();
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create(EngineDesc.hWnd);
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pPhysX_Manager = CPhysX_Manager::Create();
	if (nullptr == m_pPhysX_Manager)
		return E_FAIL;

	// 2024-11-06 김성용
	m_pCSVFile_Manager = CCSVFile_Manager::Create();
	if (nullptr == m_pCSVFile_Manager)
		return E_FAIL;

	// 정승현
	m_pInstance_Manager = CInstance_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pInstance_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 현재 키보드와 마우스의 상태를 받아올꺼야. */
	m_pInput_Device->Update();	// 이걸 써 말아

	m_pKey_Manager->Update();	// 키 매니저의 업데이트는 항상 맨 처음에 해주자

	m_pObject_Manager->Priority_Update(fTimeDelta);	

	m_pPipeLine->Update();

	m_pFrustum->Update();

	m_pObject_Manager->Update(fTimeDelta);

	m_pCollider_Manager->Update();
	
	m_pObject_Manager->Late_Update(fTimeDelta);
	
	m_pLevel_Manager->Update(fTimeDelta);		


	m_pPhysX_Manager->PhysX_Update(fTimeDelta);

	m_pSound_Manager->Update();
}

HRESULT CGameInstance::Draw_Engine()
{
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	if (iLevelIndex == 0)
		return S_OK;

	/* 특정 레벨을 위해 존재하는 자원들은 해당 레벨이 끝나면 삭제하자. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 원형들도 레벨별로 관리했었다. */
	m_pComponent_Manager->Clear(iLevelIndex);


	return S_OK;
}

POINT CGameInstance::Get_MosePos()
{
	POINT      ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_pGraphic_Device->Get_Handle(), &ptMouse);

	return ptMouse;
}

ID3D11ShaderResourceView * CGameInstance::Get_BackBuffer_SRV() const
{
	return m_pGraphic_Device->Get_BackBuffer_SRV();	
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();
	

}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();
	
	
}
#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);	
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

#pragma endregion

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

void CGameInstance::Set_CurLevelIndex(_uint iLevelIndex)
{
	m_pLevel_Manager->Set_CurLevelIndex(iLevelIndex);
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

HRESULT CGameInstance::Add_Object_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	return 	m_pObject_Manager->Add_Object_ToLayer(iLevelIndex, strLayerTag, pGameObject);
}

CGameObject* CGameInstance::Get_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pGameObject = m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);;
	if (nullptr == pGameObject)
		return nullptr;

	m_pObject_Manager->Add_Object_ToLayer(iLevelIndex, strLayerTag, pGameObject);

	return pGameObject;
}

CGameObject * CGameInstance::Clone_GameObject(const _wstring & strPrototypeTag, void * pArg)
{

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}


CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex, iPartObjIndex);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Object(iLevelIndex, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Find_Prototype(const _wstring& strPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

CGameObject* CGameInstance::Find_Player(_uint iLevelIndex)
{
	return m_pObject_Manager->Find_Player(iLevelIndex);
}

CCamera* CGameInstance::Find_Camera(_uint iLevelIndex)
{
	return m_pObject_Manager->Find_Camera(iLevelIndex);
}

CGameObject* CGameInstance::Find_NearObject(CGameObject* pOwner, _uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_NearObject(pOwner, iLevelIndex, strLayerTag);
}

_uint CGameInstance::Get_Object_Layer_Count(_uint iLevelIndex)
{
	return m_pObject_Manager->Get_Object_Layer_Count(iLevelIndex);
}

_uint CGameInstance::Get_Layer_ObjectCount(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);

	if (pLayer != nullptr)
		return pLayer->Get_ObjectCount();

	return 0;
}

_wstring CGameInstance::Get_LayerTag(_uint iLevelIndex, _uint index)
{
	return m_pObject_Manager->Get_LayerTag(iLevelIndex, index);
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

HRESULT CGameInstance::Add_ModelPrototype(_uint iLevelIndex, const _char* strPrototypeTag, CComponent* pPrototype)
{
	return m_pComponent_Manager->Add_ModelPrototype(iLevelIndex, strPrototypeTag, pPrototype);
}

map<const _char*, class CComponent*> CGameInstance::Get_ModelPrototypes(_uint iLevelIndex)
{
	return m_pComponent_Manager->Get_ModelPrototypes(iLevelIndex);
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

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject * pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}
SSAO_DESC* CGameInstance::Get_SSAODesc()
{
	return m_pRenderer->Get_SSAODesc();
}
HDR_DESC* CGameInstance::Get_HDRDesc()
{
	return m_pRenderer->Get_HDRDesc();
}
BLOOM_DESC* CGameInstance::Get_BloomDesc()
{
	return m_pRenderer->Get_BloomDesc();
}
DOF_DESC* CGameInstance::Get_DOFDesc()
{
	return m_pRenderer->Get_DOFDesc();
}
RADIAL_DESC* CGameInstance::Get_RadialDesc()
{
	return m_pRenderer->Get_RadialDesc();
}
#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugObject(CComponent * pDebugObject)
{
	return m_pRenderer->Add_DebugObject(pDebugObject);	
}
#endif
#pragma endregion

#pragma region PIPELINE
void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
void CGameInstance::Set_NearFar(_float fNear, _float fFar)
{
	return m_pPipeLine->Set_NearFar(fNear, fFar);
}
const _Matrix& CGameInstance::Get_Transform(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform(eState);
}
const _Matrix& CGameInstance::Get_Transform_Inverse(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse(eState);
}

const _Vec4& CGameInstance::Get_CamPosition_Vec4() const
{
	return m_pPipeLine->Get_CamPosition_Vec4();
}
const _Vec3& CGameInstance::Get_CamPosition_Vec3() const
{
	return m_pPipeLine->Get_CamPosition_Vec3();
}

const _float& CGameInstance::Get_Near()
{
	return m_pPipeLine->Get_Near();
}

const _float& CGameInstance::Get_Far()
{
	return m_pPipeLine->Get_Far();
}

const _Matrix* CGameInstance::Get_CascadeViewMatirx() const
{
	return m_pPipeLine->Get_CascadeViewMatirx();;
}

void CGameInstance::Set_CascadeViewMatirx(_Matrix* CascadeViewMatrices)
{
	m_pPipeLine->Set_CascadeViewMatirx(CascadeViewMatrices);
}

const _Matrix* CGameInstance::Get_CascadeProjMatirx() const
{
	return m_pPipeLine->Get_CascadeProjMatirx();
}

void CGameInstance::Set_CascadeProjMatirx(_Matrix* CascadeProjMatrices)
{
	m_pPipeLine->Set_CascadeProjMatirx(CascadeProjMatrices);
}

const _Matrix* CGameInstance::Get_CascadeProjInverseMatirx() const
{
	return m_pPipeLine->Get_CascadeProjInverseMatirx();
}

void CGameInstance::Set_CascadeProjInverseMatirx(_Matrix* CascadeProjInverseMatrices)
{
	m_pPipeLine->Set_CascadeProjInverseMatirx(CascadeProjInverseMatrices);
}

#pragma endregion

#pragma region LightManager

HRESULT CGameInstance::Add_Light(const LIGHT_DESC & LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
LIGHT_DESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}
#pragma endregion

HRESULT CGameInstance::Render_Lights(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

_int CGameInstance::Get_Total_LightCount()
{
	return m_pLight_Manager->Get_Total_Light_Count();
}

void CGameInstance::Delete_Light(_int iIndex)
{
	return m_pLight_Manager->Delete_Light(iIndex);
}

_int CGameInstance::Find_Light_Index(_Vec4 vPos)
{
	return m_pLight_Manager->Find_Light_Index(vPos);
}

_Vec3 CGameInstance::Get_DirectionLightDir()
{
	return m_pLight_Manager->Get_DirectionLightDir();
}



#pragma region FONT_MANAGER 

HRESULT CGameInstance::Add_Font(const _wstring & strFontTag, const _tchar * pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}
HRESULT CGameInstance::Render_Text(const _wstring & strFontTag, const _tchar * pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale);
}

HRESULT CGameInstance::Render_TextCenter(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	return m_pFont_Manager->Render_Center(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale);
}
// 24-11-22 김성용 
// 오른쪽 정렬로 텍스트 그리기
HRESULT CGameInstance::Render_Right(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	return m_pFont_Manager->Render_Right(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale);
}
#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring & strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_RenderTarget_Array(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize)
{
	return m_pTarget_Manager->Add_RenderTarget_Array(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor, iArraySize);
}

HRESULT CGameInstance::Add_RenderTarget_For_Desc(const _wstring& strTargetTag, D3D11_BUFFER_DESC* const& pBufferDesc, D3D11_TEXTURE2D_DESC* const& pTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC* const& pSRVDesc, D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUAVDesc, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget_For_Desc(strTargetTag, pBufferDesc, pTextureDesc, pSRVDesc, pUAVDesc, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring & strMRTTag, const _wstring & strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring & strMRTTag, ID3D11DepthStencilView* pDSV, _bool isClear)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV, isClear);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(CShader * pShader, const _wstring & strTargetTag, const _char * pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

HRESULT CGameInstance::BInd_RT_UnorderedView(CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_UnorderedAccess(pShader, strTargetTag, pConstantName);
}

HRESULT CGameInstance::Copy_RenderTarget(const _wstring & strTargetTag, ID3D11Texture2D * pTexture)
{
	return m_pTarget_Manager->Copy_RenderTarget(strTargetTag, pTexture);
}

HRESULT CGameInstance::Clear_MRT(const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Clear_MRT(strTargetTag);
}

ID3D11Buffer* CGameInstance::Get_Buffer(const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Get_Buffer(strTargetTag);
}

ID3D11UnorderedAccessView* CGameInstance::Get_UAV(const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Get_UAV(strTargetTag);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT_Debug(const _wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pTarget_Manager->Render(strMRTTag, pShader, pVIBuffer);	
}

#endif

#pragma endregion

#pragma region PICKING

_bool CGameInstance::Picking(_float3 * pPickPos)
{
	return m_pPicking->Picking(pPickPos);
}

_bool CGameInstance::Picking_Object(_uint* pPickID)
{
	return  m_pPicking->Picking_Object(pPickID);
}

_vector CGameInstance::Compute_Height(_fvector vWorldPos, _fmatrix ViewMatrix, _fmatrix ProjMatrix)
{

	return m_pPicking->Compute_Height(vWorldPos, ViewMatrix, ProjMatrix);
}

#pragma endregion

#pragma region FRUSTUM

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_WorldSpace(vPosition, fRadius);
}

_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_LocalSpace(vPosition, fRadius);
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	return m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

#pragma endregion

#pragma region Collider_Manager
HRESULT CGameInstance::Add_ColliderList(CCollider* pCollider)
{
	return m_pCollider_Manager->Add_ColliderList(pCollider);
}
#pragma endregion

#pragma region Key_Manager
KEY_STATE CGameInstance::Get_KeyState(KEY _eKey)
{
	return m_pKey_Manager->Get_KeyState(_eKey);
}
#pragma endregion

#pragma region PhysX_Manager

PxPhysics* CGameInstance::Get_PhysX() const
{
	return m_pPhysX_Manager->Get_PhysX();
}

PxScene* CGameInstance::Get_PhysXScene() const
{
	return m_pPhysX_Manager->Get_PhysXScene();
}

_bool CGameInstance::RayCast_PhysX(_vector vRayPos, _vector vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
	return m_pPhysX_Manager->RayCast(vRayPos, vRayDir, vHitPos, vNormal, fHitDistance);
}
#pragma endregion

// 2024-11-06 김성용 추가
#pragma region CSVFile_Manager
HRESULT CGameInstance::FileOpenByRow(const _char* FilePath, _bool bIsRead)
{
	return m_pCSVFile_Manager->FileOpenByRow(FilePath, bIsRead);
}
_bool CGameInstance::LoadDataByRow(vector<_wstring>* vecDataBuffer)
{
	return m_pCSVFile_Manager->LoadDataByRow(vecDataBuffer);
}
_bool CGameInstance::SaveDataByRow(vector<_wstring>& vecDataBuffer)
{
	return m_pCSVFile_Manager->SaveDataByRow(vecDataBuffer);
}
void CGameInstance::FileClose()
{
	m_pCSVFile_Manager->FileClose();
}
HRESULT CGameInstance::LoadDataByFile(const _char* FilePath, vector<vector<_wstring>>* vecDataBuffer)
{
	return m_pCSVFile_Manager->LoadDataByFile(FilePath, vecDataBuffer);
}
HRESULT CGameInstance::SaveDataByFile(const _char* FilePath, vector<vector<_wstring>>& vecDataBuffer)
{
	return m_pCSVFile_Manager->SaveDataByFile(FilePath, vecDataBuffer);
}
_bool CGameInstance::IsFileRead()
{
	return m_pCSVFile_Manager->IsFileRead();
}
_bool CGameInstance::IsFileWrite()
{
	return m_pCSVFile_Manager->IsFileWrite();
}
#pragma endregion

#pragma region CInstance_Manager
CModel* CGameInstance::Add_NonAnimModel_Instance(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pInstance_Manager->Add_NonAnimModel(iLevelIndex, strPrototypeTag, pArg);
}
HRESULT CGameInstance::Draw_Instance(_uint iPass)
{
	return m_pInstance_Manager->Draw(iPass);
}
HRESULT CGameInstance::Draw_Shadow_Instance()
{
	return m_pInstance_Manager->Draw_Shadow();
}
void CGameInstance::Clear_Instance()
{
	m_pInstance_Manager->Clear();
}
#pragma endregion

#pragma region SOUND_MANAGER

void CGameInstance::Play_BGM(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->Play_BGM(pSoundKey, fVolume);
}

void CGameInstance::Play_Effect(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->Play_Effect(pSoundKey, fVolume);
}

void CGameInstance::Stop_BGM()
{
	m_pSound_Manager->Stop_BGM();
}

void CGameInstance::Pause_BGM()
{
	m_pSound_Manager->Pause_BGM();
}

void CGameInstance::SetVolume_BGM(_float fVolume)
{
	m_pSound_Manager->SetVolume_BGM(fVolume);
}

void CGameInstance::Set_Listener(CGameObject* pListener)
{
	m_pSound_Manager->Set_Listener(pListener);
}

FMOD::System* CGameInstance::Get_SoundSystem()
{
	return m_pSound_Manager->Get_System();
}

map<TCHAR*, FMOD::Sound*>& CGameInstance::Get_Sounds()
{
	return m_pSound_Manager->Get_Sounds();
}

void CGameInstance::LoadSoundFile(const char* pFolderName)
{
	m_pSound_Manager->LoadSoundFile(pFolderName);
}

#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pCollider_Manager);
	// 2024-11-06 김성용
	Safe_Release(m_pCSVFile_Manager);

	Safe_Release(m_pFrustum);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pInstance_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	__super::Free();

}
