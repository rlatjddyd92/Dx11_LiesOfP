#include "..\Public\Channel.h"
#include "Bone.h"
#include "Model.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(HANDLE* pFile, const CModel* pModel)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	if (m_iBoneIndex == -1)
	{
		int a = 0;
	}

	/* 가장 큰 키 프레임을 찾음 */
	/* Assimp에서는 키 프레임이 이전 프레임과 큰 차이가 없으면 새로운 키 프레임을 생서하지 않음 (중간이 비어 있는 경우는 없음) */
	/* 따라서 가장 큰 키 프레임 기준으로 진행해야 함. */
	ReadFile(*pFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

	_float3			vScale = {};
	_float4			vRotation = {};
	_float3			vTranslation = {};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		ReadFile(*pFile, &KeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition)
{
	if (0.0 == CurrentTrackPosition)
		*pCurrentKeyFrameIndex  = 0;
	
	if (Bones[m_iBoneIndex]->Get_isNeedTuning())
	{
		Bones[m_iBoneIndex]->Apply_Tuning();
		return;
	}

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	_vector		vScale{}, vRotation{}, vTranslation{};

	/* 보간이 필요없는 경우 */
	if (CurrentTrackPosition >= LastKeyFrame.TrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}

	/* 보간이 필요한 대부분 경우 */
	else
	{
		if (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition)
		{
			while (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)
				++(*pCurrentKeyFrameIndex);
		}
		else//뒤로 재생되었을 경우에도 키프레임의 변경
		{
			while (CurrentTrackPosition <= m_KeyFrames[*pCurrentKeyFrameIndex - 1].TrackPosition)
				--(*pCurrentKeyFrameIndex);
		}


		_vector		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		_vector		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);

		_vector		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		_vector		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);

		_vector		vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
		_vector		vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

		/* 그거 내가 설명한거. */
		_double		Ratio = (CurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
	}

	/* 이 뼈만의 상태행렬 = 회전기준은 당연히 부모기준.(자식뼈만의 상태를 만들때 원점기준으로 정의해야한다.) */

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	
	
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
	
	
}

KEYFRAME CChannel::Find_KeyFrameIndex(_uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition)
{
	if (0.0 == CurrentTrackPosition)
	{
		*pCurrentKeyFrameIndex = 0;
	}

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	_vector		vScale, vRotation, vTranslation;

	/* 보간이 필요없는 경우 */
	if (CurrentTrackPosition >= LastKeyFrame.TrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
		return m_KeyFrames.back();
	}

	/* 보간이 필요한 대부분 경우 */
	else
	{
		while (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)
			++(*pCurrentKeyFrameIndex);
	}

	return m_KeyFrames[*pCurrentKeyFrameIndex];
}

KEYFRAME CChannel::Get_KeyFrame(_uint iFrame)
{
	if (m_KeyFrames.size() > iFrame)
	{
		return m_KeyFrames[iFrame];
	}
	else
	{
		return m_KeyFrames[0];
	}
}

HRESULT CChannel::Create_BinaryFile(HANDLE* pFile)
{
	_ulong dwByte = 0;
	//이름 저장
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	//키프레임 갯수 저장
	WriteFile(*pFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

	for (auto& KeyFrame : m_KeyFrames)
	{
		WriteFile(*pFile, &KeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
	}

	return S_OK;
}

CChannel* CChannel::Create(HANDLE* pFile, const CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pFile, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
