#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"


BEGIN(Tools)

class CCutScene :
    public CGameObject
{
public:
	enum ActorType {CAMERA, UI, SHADER,GAMEOBJECT, TYPE_END};

	typedef struct
	{
		enum ShaderType {DOF, SHADER_END};

	}SHADER_DESC;

	typedef struct
	{
		_float			dTrackPosition;
		_bool			bActived = false;
	}CUTSCENE_DESC;
	
private:
	CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutScene(const CCutScene& Prototype);
	virtual ~CCutScene() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	_float* Get_CurFrame_Ptr() { return &m_fTrackPosition; }
	_float Get_MaxFrame() { return m_fMaxFrame; }
	void Set_MaxFrame(_float fFrame) { m_fMaxFrame = fFrame; }
	void Set_Play(_bool bPlay) { m_bPlay = bPlay; }
	void Keyframe_Actived_Reset();
	void Create_KeyFrame();
private:
	_float m_fMaxFrame = { 50.f };
	_float m_fTrackPosition = { 0.f };
	_bool m_bPlay = { false };

	vector<CUTSCENE_DESC*>	m_KeyFrames;

private:
	void Play_Keyframes(_float fTimeDelta);

public:
	static CCutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
