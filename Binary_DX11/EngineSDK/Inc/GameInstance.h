#pragma once

#include "Component_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"

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
	HRESULT Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	void FinalUpdate_Engine();
	HRESULT Clear(_uint iLevelIndex);

public:
	POINT Get_MosePos();
	
#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin();
	void Render_End();
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	class CLevel* Get_CurLevel();
	_uint   Get_CurLevelIndex();
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _char* strLayerTag, const _char* strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	list<class CGameObject*>& Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);

	class CGameObject* Find_Player();
	class CGameObject* Find_Camera();
#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);	
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	list<class CGameObject*>& Get_RenderList(CRenderer::RENDERGROUP eGroup);
#pragma endregion

#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;

	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;
#pragma endregion

#pragma region PICKING
	class CGameObject* Get_ModelPickedObject();
	void Transform_MouseRay_ToLocalSpace(const _fmatrix& WorldMatrix);	
	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut);
	void Add_ModelPickingList(class CGameObject* pObj);
#pragma endregion

#pragma region KEY_MANAGER
	KEY_STATE   Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region SOUND_MANAGER
	void	Play_Sound(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void	Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void	Play_BGM(const TCHAR* pSoundKey, _float fVolume);
	void	Stop_Sound(_uint eID);
	void	Stop_All();
	void	Set_ChannelVolume(_uint eID, _float fVolume);
	void	Set_PlayeSpeed(_uint eID, _float fSpeedRatio);
	void	Pause(_uint eID);
	_bool	IsPlaying(_uint eID);
#pragma endregion

#pragma region EVENT_MANAGER
	HRESULT Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize = 0, void* pArg = nullptr);
	HRESULT Event_DestoryObject(class CGameObject* pGameObject);
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking_Manager*			m_pPicking_Manager = { nullptr };

	/*추가한 기능들*/
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CUI_Manager*				m_pUI_Manager = { nullptr };

	class CEvent_Manager*			m_pEvent_Manager = { nullptr };

public:	
	void Release_Engine();
	virtual void Free() override;
};

END