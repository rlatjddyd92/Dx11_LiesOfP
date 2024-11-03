#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(HANDLE* pFile, const aiAnimation* pAIAnimation)
{
	_ulong dwByte = 0;

	string strName = pAIAnimation->mName.data;
	strName = strName.substr(strName.find('|') + 1);

	while (true)
	{
		if (-1 == strName.find('|'))
			break;

		strName = strName.substr(strName.find('|') + 1);
	}

	strcpy_s(m_szName, strName.c_str());
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	m_Duration = pAIAnimation->mDuration;
	m_SpeedPerSec = pAIAnimation->mTicksPerSecond;
	WriteFile(*pFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	WriteFile(*pFile, &m_SpeedPerSec, sizeof(_double), &dwByte, nullptr);

	/* 이 애니메이션이 사용하는 뼈의 객수를 저장한다. */
	m_iNumChannels = pAIAnimation->mNumChannels;
	WriteFile(*pFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pFile, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(HANDLE* pFile, const aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pFile, pAIAnimation)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}
