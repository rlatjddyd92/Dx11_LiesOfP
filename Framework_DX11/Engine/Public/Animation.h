#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	_char*						Get_Name() { return m_szName; }

	vector<class CChannel*>&	Get_Channels() { return m_Channels; }
	void						Set_Channels(vector<class CChannel*>& Channels) { m_Channels = Channels; }

	_double						Get_SpeedRatio() { return m_SpeedRatio; }
	void						Set_SpeedRatio(_double SpeedRatio) { m_SpeedRatio = SpeedRatio; }

	void						Add_EventKeyFrame(EVENT_KEYFRAME Event_Struct_KeyFrame) { m_EventKeyFrames.push_back(Event_Struct_KeyFrame); }
	vector<EVENT_KEYFRAME>*		Get_EventKeyFrames_Ptr() { return &m_EventKeyFrames; }

	//이봉준 애니메이션 툴 속도 제어, 
	_double						Get_SpeedPerSec() { return m_SpeedPerSec; }
	void						Set_SpeedPerSec(_double SpeedPerSec) { m_SpeedPerSec = SpeedPerSec; }

	_double						Get_Duration() { return m_Duration; }

public:
	HRESULT Initialize(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const class CModel* pModel);
	_uint Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _bool* isEnd, _float fTimeDelta);

private:
	_char						m_szName[MAX_PATH] = {};

	/* 이 애니메이션이 사용하는 뼈의 갯수 */
	_uint						m_iNumChannels = { 0 };
	/* 실제 뼈들의 정보*/
	/* Channel : 뼈 (시간에 따른 이 뼈의 상태 행렬을 보관한다) */
	vector<class CChannel*>		m_Channels;

	vector<EVENT_KEYFRAME>		m_EventKeyFrames;

	/* 이 애니메이션을 구동하기위해 거리는 전체 거리. */
	_double						m_Duration = {};
	_double						m_SpeedRatio = { 1.0 };

	/* 애니메이션의 초당 재생속도 */
	_double						m_SpeedPerSec = {};


public:
	static CAnimation* Create(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END