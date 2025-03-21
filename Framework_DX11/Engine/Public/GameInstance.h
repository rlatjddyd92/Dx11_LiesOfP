#pragma once

#include "Component_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"

#include "fmod.h"
#include "fmod.hpp"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	HRESULT Clear(_uint iLevelIndex);

	_float Get_Random_Normal() const { return (_float)rand() / RAND_MAX; }

	_float Get_Random(_float fMin, _float fMax) const { return fMin + Get_Random_Normal() * (fMax - fMin); }
	
	POINT Get_MosePos();

	void Set_Player_AreaNum(_int iAreaNum) { m_iPlayer_AreaNum = iAreaNum; };
	_int Get_Player_AreaNum() { return m_iPlayer_AreaNum; }

#pragma region GRAPHIC_DEVICE
public:
	ID3D11ShaderResourceView* Get_BackBuffer_SRV() const;
	void Render_Begin();
	void Render_End();
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);	
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT			Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	class CLevel*	Get_CurLevel();
	_uint			Get_CurLevelIndex();
	void			Set_CurLevelIndex(_uint iLevelIndex);
	_uint			Get_PrevLevelIndex();
	void			Set_PrevLevelIndex(_uint iLevelIndex);

	void			Set_StaticLevelIndex(_uint iLevelIndex);
	void			Set_LoadingLevelIndex(_uint iLevelIndex);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT				Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT				Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT				Add_Object_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	class CGameObject*	Get_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject*	Clone_GameObject(const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent*	Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0, _uint iPartObjIndex = 0);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject*	Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	class CGameObject*	Find_Prototype(const _wstring& strPrototypeTag);

	class CGameObject*	Find_Player(_uint iLevelIndex = 0);
	class CCamera*		Find_Camera(_uint iLevelIndex = 0);
	class CGameObject*	Find_NearObject(class CGameObject* pOwner, _uint iLevelIndex, const _wstring& strLayerTag);

	_uint Get_Object_Layer_Count(_uint iLevelIndex);
	_uint Get_Layer_ObjectCount(_uint iLevelIndex, const _wstring& strLayerTag);
	_wstring Get_LayerTag(_uint iLevelIndex, _uint index);


#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
	HRESULT Add_ModelPrototype(_uint iLevelIndex, const _char* strPrototypeTag, class CComponent* pPrototype);
	map<const _char*, class CComponent*> Get_ModelPrototypes(_uint iLevelIndex);
	class CComponent* Find_ComponentPrototype(_uint iLevelIndex, const _wstring& strPrototypeTag);
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);
	void Start_TimerLack(const _wstring& strTimerTag, _float fLackTimeDelta = 0.f, _float fDuration = 0.1f);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);

	SSAO_DESC*		Get_SSAODesc();
	HDR_DESC*		Get_HDRDesc();
	BLOOM_DESC*		Get_BloomDesc();
	DOF_DESC*		Get_DOFDesc();
	RADIAL_DESC* Get_RadialDesc();

	void			Set_SSAODesc(_bool bIsUsing);
	void			Set_HDRDesc(_bool bIsUsing);
	void			Set_BloomDesc(_bool bIsUsing);
	void			Set_DOFDesc(_bool bIsUsing);
	void			Set_RadialDesc(_bool bIsUsing);

	_bool*			Get_IsOnPBR();
	void			Toggle_PBR();

	_bool			Get_IsOnShadow();
	void			Toggle_Shadow();

#ifdef _DEBUG
	HRESULT Add_DebugObject(class CComponent* pDebugObject);
#endif
#pragma endregion

#pragma region PIPELINE
	void			Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void			Set_NearFar(_float fNear, _float fFar);

	const _Matrix&	Get_Transform(CPipeLine::D3DTRANSFORMSTATE eState) const;
	const _Matrix&	Get_Transform_Inverse(CPipeLine::D3DTRANSFORMSTATE eState) const;

	const _Matrix& Get_OldTransform(CPipeLine::D3DTRANSFORMSTATE eState) const;
	const _Matrix& Get_OldTransform_Inverse(CPipeLine::D3DTRANSFORMSTATE eState) const;

	const _Vec4& Get_CamPosition_Vec4() const;
	const _Vec3& Get_CamPosition_Vec3() const;

	const _float& Get_Near();
	const _float& Get_Far();

	const _Matrix* Get_CascadeViewMatrices() const;
	void Set_CascadeViewMatrices(_Matrix* CascadeViewMatrices);

	const _Matrix* Get_CascadeProjMatrices() const;
	void Set_CascadeProjMatrices(_Matrix* CascadeProjMatrices);
#pragma endregion

#pragma region LIGHT_MANAGER
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _bool isOnPBR);
	_int Get_Total_LightCount();
	void Delete_Light(_int iIndex);
	_int Find_Light_Index(_Vec4 vPos);
	_Vec3 Get_DirectionLightDir();

	void Clear_Lights();
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
	HRESULT Render_TextCenter(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
	// 24-11-22 김성용 
	// 오른쪽 정렬로 텍스트 그리기
	HRESULT Render_TextRight(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
	// 24-12-26 김성용 
	// 텍스트 크기 확인
	_Vec4 Measure_Font(const _wstring& strFontTag, _wstring strText);
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_RenderTarget_Array(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize);
	HRESULT Add_RenderTarget_For_Desc(const _wstring& strTargetTag,
		D3D11_BUFFER_DESC* const& pBufferDesc,
		D3D11_TEXTURE2D_DESC* const& pTextureDesc,
		D3D11_SHADER_RESOURCE_VIEW_DESC* const& pSRVDesc,
		D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUAVDesc, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isClear = true);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	HRESULT Bind_ShaderResource_NonVTX(class CShader_NonVTX* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	HRESULT BInd_RT_UnorderedView(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture);
	HRESULT Clear_MRT(const _wstring& strMRTTag);
	HRESULT Clear_RTV(const _wstring& strTargetTag);

	ID3D11Buffer* Get_Buffer(const _wstring& strTargetTag);
	ID3D11UnorderedAccessView* Get_UAV(const _wstring& strTargetTag);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif


#pragma endregion

#pragma region PICKING
	_bool Picking(_float3* pPickPos); 
	_bool Picking_Object(_uint* pPickID);
	_vector Compute_Height(_fvector vWorldPos, _fmatrix ViewMatrix, _fmatrix ProjMatrix);
	_bool Get_Is_Picking() { return m_isPicking; }
	void Set_Is_Picking(_bool bPicking) {m_isPicking = bPicking; }
#pragma endregion

#pragma region FRUSTUM
	public:
		_bool isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius = 0.f);
		_bool isIn_Frustum_LocalSpace(_fvector vPosition, _float fRadius);
		void Transform_ToLocalSpace(_fmatrix WorldMatrix);
#pragma endregion


		/* 추가한 기능들 */

#pragma region Collider_Manager
		HRESULT Add_ColliderList(class CCollider* pCollider);
#pragma endregion

#pragma region KEY_MANAGER
		KEY_STATE   Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region PhysX_MANAGER
		PxPhysics* Get_PhysX() const;
		PxScene* Get_PhysXScene() const;
		PxFoundation* Get_PhysXFoundation() const;
		PxCudaContextManager* Get_PhysXCuda() const;
		void Set_Gravity(_float fY);
		_bool   RayCast_PhysX(_vector vRayPos, _vector vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance);
#pragma endregion

		// 2024-11-06 김성용 추가 
#pragma region CSVFileManager
		HRESULT		FileOpenByRow(const _char* FilePath, _bool bIsRead);// bIsRead가 true면 읽기 false면 쓰기
		_bool		LoadDataByRow(vector<_wstring>* vecDataBuffer);
		_bool		SaveDataByRow(vector<_wstring>& vecDataBuffer);
		void		FileClose();

		HRESULT		LoadDataByFile(const _char* FilePath, vector<vector<_wstring>>* vecDataBuffer);
		HRESULT		SaveDataByFile(const _char* FilePath, vector<vector<_wstring>>& vecDataBuffer);

		_bool		IsFileRead();
		_bool		IsFileWrite();

#pragma endregion 

#pragma region CInstance_Manager
		CModel* Add_NonAnimModel_Instance(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
		HRESULT Draw_Instance(_uint iPass);
		HRESULT Draw_Shadow_Instance();
		void Clear_Instance();
#pragma endregion

#pragma region SOUND_MANAGER
		void Play_BGM(const TCHAR* pSoundKey, _float* fVolume);
		void Play_Effect(const TCHAR* pSoundKey, _float* fVolume);
		void Stop_BGM();
		void Pause_BGM();
		void SetVolume_BGM(_float fVolume);
		_bool IsPlaying_BGM();
		_bool IsPause_BGM();

		void Play_ENV(const TCHAR* pSoundKey, _float* fVolume);
		void Stop_ENV();
		void Pause_ENV();

		void Play_Cinematic(const TCHAR* pSoundKey, _float* fVolume);
		void Stop_Cinematic();
		void Pause_Cinematic();

		void	Set_Listener(class CGameObject* pListener);
		FMOD::System* Get_SoundSystem();
		map<TCHAR*, FMOD::Sound*>& Get_Sounds();
		void	LoadSoundFile(const char* pFolderName);
#pragma endregion

#pragma region World_Octree
		_bool Is_In_FrustumCulledOctree(vector<_int>& pWorldOctreeIndex);
		_bool Is_Active_Octree();

		void Create_Octree(_Vec3 vMinPos, _Vec3 vMaxPos);
		void World_Octree_Render();
		void Change_Active_Octree();
#pragma endregion

		_float Lerp(_float fSour, _float fDest, _float fRatio);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };

	/* 추가한 기능들*/
	class CCollider_Manager*		m_pCollider_Manager = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CInstance_Manager*		m_pInstance_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };

	// 2024-11-06 김성용
	class CCSVFile_Manager*			m_pCSVFile_Manager = { nullptr };

	//옥트리
	class CWorldOctree_Manager*		m_pWorldOctree_Manager = {nullptr};

private:
	_bool m_isPicking = { false };
	_int m_iPlayer_AreaNum = { 0 };

public:	
	void Release_Engine();
	virtual void Free() override;
};

END