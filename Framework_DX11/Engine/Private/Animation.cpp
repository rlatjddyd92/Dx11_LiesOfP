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

	for (_int i = 0; i < (_int)m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pFile, pModel);
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}


_uint CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones
	, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _bool* isEnd
	, _float fTimeDelta, _bool isChildOfBoundary, _bool* bBoneUpdated,_bool BlockStackTime)
{
	_uint iCurrentFrame = 0;

	if (!BlockStackTime)
	{
		/* 현재 재생위치를 계산하낟. */
		*pCurrentTrackPosition += m_SpeedPerSec * m_SpeedRatio * fTimeDelta;

		if (*pCurrentTrackPosition >= m_Duration)
		{
			if (true == isLoop)
			{
				*pCurrentTrackPosition = 0.f;
			}
			else
				*isEnd = true;

			for (auto& pKeyFrameIndex : CurrentKeyFrameIndices)
			{
				pKeyFrameIndex = 0;
			}

		}
	}


	/* 현재 재생위치에 맞게 현재 애니메이션이 컨트롤해야 할 뼈의 상태들을 갱신해준다. */
	_uint		iChannelIndex = { 0 };
	for (auto& pChannel : m_Channels)
	{
		if (isChildOfBoundary == Bones[pChannel->Get_BoneIndex()]->Get_IsChildOf_Boundary())
		{
			pChannel->Update_TransformationMatrix(Bones, &CurrentKeyFrameIndices[iChannelIndex], *pCurrentTrackPosition);
			if (iCurrentFrame < CurrentKeyFrameIndices[iChannelIndex])
			{
					iCurrentFrame = CurrentKeyFrameIndices[iChannelIndex];
			}
		}

		++iChannelIndex;
	}
	
	if (bBoneUpdated != nullptr)
	{
		*bBoneUpdated = true;
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

	for (auto& pChannel : m_Channels)
	{//채널 저장
		pChannel->Create_BinaryFile(pFile);
	}

	return S_OK;
}

void	 CAnimation::Find_ChannelWide()
{
	_int iSize{};
	_int iChannelIndex{};

	for (_int i = 0; i < m_Channels.size(); ++i)
	{
		if (m_Channels[i]->Get_KeyFrames().size() > iSize)
		{
			iChannelIndex = i;
			iSize = (_int)m_Channels[i]->Get_KeyFrames().size();
		}
	}
	m_iWideChannelIndex = iChannelIndex;
}

CChannel* CAnimation::Get_WideChannel()
{
	return m_Channels[m_iWideChannelIndex];
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
