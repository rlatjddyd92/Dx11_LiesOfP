#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"


BEGIN(Tools)

class CCutScene :
    public CGameObject
{
public:
	enum ActorType {CAMERA, UI, SHADER,GAMEOBJECT, SOUND, TYPE_END};

private:
	CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutScene(const CCutScene& Prototype);
	virtual ~CCutScene() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;

public:
	_float*			Get_CurFrame_Ptr() { return &m_fTrackPosition; }
	_float			Get_MaxFrame() { return m_fMaxFrame; }
	void			Set_MaxFrame(_float fFrame) { m_fMaxFrame = fFrame; }
	void			Set_Play(_bool bPlay) { m_bPlay = bPlay; }
	void			Keyframe_Actived_Reset();
	void			Create_KeyFrame();
	void			Sort_KeyFrame_TrackPosition();
	void			Delete_Selected_Keyframe(_int iIndex);
	void			Load_KeyFrame(CUTSCENE_KEYFRAME_DESC pDesc);
	_int			Get_KeyFrameCount() { return (_int)m_KeyFrames.size(); }
	CUTSCENE_KEYFRAME_DESC*	Get_Selected_KeyFrame(_int iIndex) { return m_KeyFrames[iIndex]; }

private:
	_float						m_fMaxFrame = { 50.f };
	_float						m_fTrackPosition = { 0.f };
	_float						m_fActiveDofValue = { 0.f };
	_bool						m_bPlay = { false };

	vector<CUTSCENE_KEYFRAME_DESC*>		m_KeyFrames;

	class CController_UITool*	m_pController_UITool = { nullptr };
	class CCamera*				m_pCamera = { nullptr };
private:
	void Play_Keyframes(_float fTimeDelta);
	void Active_Shader(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc, _float fTimeDelta);
	void Active_UI(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc);
	void Active_Camera(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc);
public:
	static CCutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
