#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CCutScene :
	public CGameObject
{
public:
	enum ObjType { PLAYER, BOSS1, BOSS2, TYPE_END };

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
	_float* Get_CurFrame_Ptr() { return &m_fTrackPosition; }
	void			Set_MaxFrame(_float fFrame) { m_fMaxFrame = fFrame; }
	void			Set_CutSceneIndex(_uint iIndex){m_iIndex = iIndex;}
	void			Keyframe_Actived_Reset();
	void			Create_KeyFrame();
	CUTSCENE_KEYFRAME_DESC* Get_Selected_KeyFrame(_int iIndex) { return m_KeyFrames[iIndex]; }
	void			Load_KeyFrame(CUTSCENE_KEYFRAME_DESC pDesc);

	void			Start_Play();
	_bool			Get_isFinished() { return m_bFinished; }

private:
	_uint						m_iIndex = { 0 };
	_float						m_fMaxFrame = { 50.f };
	_float						m_fTrackPosition = { 0.f };
	_bool						m_bPlay = { false };
	_bool						m_bFinished = { false };

	vector<CUTSCENE_KEYFRAME_DESC*>		m_KeyFrames;

	class CCamera* m_pCamera = { nullptr };
	class CPawn*  m_pObjects[3] = { nullptr,nullptr,nullptr };

private:
	void Play_Keyframes(_float fTimeDelta);
	void Active_Shader(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc);
	void Active_UI(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc);
	void Active_Camera(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc);
	void Active_Obj(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc);
public:
	static CCutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
