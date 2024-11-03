#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(HANDLE* pFile, const aiAnimation* pAIAnimation);

private:
	_char						m_szName[MAX_PATH] = {};

	/* 이 애니메이션이 사용하는 뼈의 갯수 */
	_uint						m_iNumChannels = { 0 };

	/* 실제 뼈들의 정보 */
	/* Channel : 뼈 (시간에 따른 이 뼈의 상태 행렬을 보관한다) */
	vector<class CChannel*>		m_Channels;

	/* 이 애니메이션을 구동하기위해 걸리를 전체 거리. */
	_double						m_Duration = {};

	/* 애니메이션의 초당 재생속도 */
	_double						m_SpeedPerSec = {};

	_double						m_CurrentTrackPosition = {};

public:
	static CAnimation* Create(HANDLE* pFile, const aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END