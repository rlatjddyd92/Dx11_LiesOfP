#include "Channel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(HANDLE* pFile, const aiNodeAnim* pAIChannel)
{
	_ulong dwByte = 0;

	strcpy_s(m_szName, pAIChannel->mNodeName.data);
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	/* 가장 큰 키 프레임을 찾음 */
	/* Assimp에서는 키 프레임이 이전 프레임과 큰 차이가 없으면 새로운 키 프레임을 생서하지 않음 (중간이 비어 있는 경우는 없음) */
	/* 따라서 가장 큰 키 프레임 기준으로 진행해야 함. */
	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);
	WriteFile(*pFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

	_float3			vScale = {};
	_float4			vRotation = {};
	_float3			vTranslation = {};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.TrackPosition = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.TrackPosition = pAIChannel->mRotationKeys[i].mTime;
		}


		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.TrackPosition = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		WriteFile(*pFile, &KeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(HANDLE* pFile, const aiNodeAnim* pNodeAnim)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pFile, pNodeAnim)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
