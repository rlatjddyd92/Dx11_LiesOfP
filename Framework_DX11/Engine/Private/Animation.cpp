#include "..\Public\Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(*pFile, &m_SpeedPerSec, sizeof(_double), &dwByte, nullptr);

	/* 이 애니메이션이 사용하는 뼈의 객수를 저장한다. */
	ReadFile(*pFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	KeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pFile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize__To_Binary(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(*pFile, &m_SpeedPerSec, sizeof(_double), &dwByte, nullptr);

	/* 이 애니메이션이 사용하는 뼈의 객수를 저장한다. */
	ReadFile(*pFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	KeyFrameIndices.resize(m_iNumChannels);

	_int iNumEventKeyFrame;
	ReadFile(*pFile, &iNumEventKeyFrame, sizeof(_uint), &dwByte, nullptr);
	for (_int i = 0; i < iNumEventKeyFrame; ++i)
	{
		EVENT_KEYFRAME EventKeyFrame;

		ReadFile(*pFile, &EventKeyFrame, sizeof(EVENT_KEYFRAME), &dwByte, nullptr);

		m_EventKeyFrames.push_back(EventKeyFrame);
	}

	for (_int i = 0; i < (_int)m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pFile, pModel);
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_uint CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _bool* isEnd, _float fTimeDelta, _bool isChildOfBoundary, OUTPUT_EVKEY* pOutputKey, _bool BlockStackTime)
{
	if (!BlockStackTime)
	{
		/* 현재 재생위치를 계산하낟. */
		*pCurrentTrackPosition += m_SpeedPerSec * m_SpeedRatio * fTimeDelta;

		if (*pCurrentTrackPosition >= m_Duration)
		{
			if (false == isLoop)
			{
				*isEnd = true;
				return true;
			}
				

			*pCurrentTrackPosition = 0.f;
		}
	}

	//이벤트 키 프레임 설정
	if (pOutputKey != nullptr)
	{
		_double CurPos = *pCurrentTrackPosition;
		for (_int i = 0; i < m_EventKeyFrames.size(); ++i)
		{
			if (m_EventKeyFrames[i].eEvent_type == EVENT_KEYFRAME::ET_ONCE)
			{
				if (CurPos >= m_EventKeyFrames[i].Start_TrackPosition)
				{
					pOutputKey->bActiveEffect = true;
					pOutputKey->eEvent_type = EVENT_KEYFRAME::ET_ONCE;
					pOutputKey->iBoneIndex = m_EventKeyFrames[i].iBoneIndex;
					pOutputKey->iEffectNum = m_EventKeyFrames[i].iEffectNum;
				}
			}
			else if (m_EventKeyFrames[i].eEvent_type == EVENT_KEYFRAME::ET_REPET)
			{
				if (CurPos >= m_EventKeyFrames[i].Start_TrackPosition && m_EventKeyFrames[i].End_TrackPosition >= CurPos)
				{
					pOutputKey->bActiveEffect = true;
					pOutputKey->eEvent_type = EVENT_KEYFRAME::ET_REPET;
					pOutputKey->iBoneIndex = m_EventKeyFrames[i].iBoneIndex;
					pOutputKey->iEffectNum = m_EventKeyFrames[i].iEffectNum;
				}
			}
		}
	}

	_uint iCurrentFrame = 0;

	/* 현재 재생위치에 맞게 현재 애니메이션이 컨트롤해야 할 뼈의 상태들을 갱신해준다. */
	_uint		iChannelIndex = { 0 };
	for (auto& pChannel : m_Channels)
	{
		if (isChildOfBoundary == Bones[pChannel->Get_BoneIndex()]->Get_IsChildOf_Boundary())
		{
			pChannel->Update_TransformationMatrix(Bones, &CurrentKeyFrameIndices[iChannelIndex], *pCurrentTrackPosition);
			if (iCurrentFrame < CurrentKeyFrameIndices[iChannelIndex])
			{
				// 혹시모를 예외 처리 갑자기 프레임이 이상하게 증가하는 오류 방지
				if (CurrentKeyFrameIndices[iChannelIndex] - iCurrentFrame < 3)
					iCurrentFrame = CurrentKeyFrameIndices[iChannelIndex];
			}
		}

		++iChannelIndex;
	}

	return iCurrentFrame;
}

HRESULT CAnimation::Create_BinaryFile(HANDLE* pFile)
{
	_ulong dwByte = 0;
	//이름 저장
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	//길이, 속도 저장
	WriteFile(*pFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	WriteFile(*pFile, &m_SpeedPerSec, sizeof(_double), &dwByte, nullptr);

	//채널 갯수 저장
	WriteFile(*pFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	_int iNumEventKeyFrame{};
	//이벤트 키프레임 갯수 저장
	iNumEventKeyFrame = (_int)m_EventKeyFrames.size();
	WriteFile(*pFile, &iNumEventKeyFrame, sizeof(_uint), &dwByte, nullptr);

	for (_int i = 0; i < m_EventKeyFrames.size(); ++i)
	{//이벤트 키프레임 저장
		WriteFile(*pFile, &m_EventKeyFrames[i], sizeof(EVENT_KEYFRAME), &dwByte, nullptr);
	}

	for (auto& pChannel : m_Channels)
	{//채널 저장
		pChannel->Create_BinaryFile(pFile);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pFile, KeyFrameIndices, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create_To_Binary(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize__To_Binary(pFile, KeyFrameIndices, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);	
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
