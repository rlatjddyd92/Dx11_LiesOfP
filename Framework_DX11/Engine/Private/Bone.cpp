#include "..\Public\Bone.h"
#include "Model.h"

CBone::CBone()
{

}

HRESULT CBone::Initialize(HANDLE* pFile, _int iParentBoneIndex)
{
	_ulong dwByte = 0;

	m_iParentBoneIndex = iParentBoneIndex;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

	// 매번 변하는 행렬임 -> 항등으로만 세팅해두기
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CBone::Initialize_ToBinary(HANDLE* pFile, _bool bUseBoundary, _uint eType)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_iParentBoneIndex, sizeof(_int), &dwByte, nullptr);

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);
	
	if (eType == CModel::TYPE_ANIM)
	{
		ReadFile(*pFile, &m_isNeedTuning, sizeof(_bool), &dwByte, nullptr);

		if (m_isNeedTuning)
		{
			ReadFile(*pFile, &m_TuningMatrix, sizeof(_float4x4), &dwByte, nullptr);
		}

		if (bUseBoundary)
		{
			ReadFile(*pFile, &m_bIsChildOf_Boundary, sizeof(_bool), &dwByte, nullptr);
		}

	}

	

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if(-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, m_TransformationMatrix * PreTransformMatrix);
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, m_TransformationMatrix * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
	}
}

void CBone::Setting_ParentBoneName(CModel* pModel)
{
	if (m_iParentBoneIndex == -1)
		return;

	vector<class CBone*> Bones = pModel->Get_Bones();
	strcpy_s(m_szParentName, Bones[m_iParentBoneIndex]->Get_Name());
}

void CBone::Update_Boundary(const vector<CBone*>& Bones, _int iCurBoneIndex, _int iBoundaryBoneIndex)
{
	if (iBoundaryBoneIndex == iCurBoneIndex)
	{
		m_bIsChildOf_Boundary = true;
		return;
	}

	if (m_iParentBoneIndex < iBoundaryBoneIndex)
	{
		m_bIsChildOf_Boundary = false;
	}
	else
	{
		m_bIsChildOf_Boundary = Bones[m_iParentBoneIndex]->Get_IsChildOf_Boundary();
	}
}

HRESULT CBone::Create_BinaryFile(HANDLE* pFile, _bool bUseBoundary, _uint eType)
{
	_ulong dwByte = 0;
	//부모뼈 인덱스
	WriteFile(*pFile, &m_iParentBoneIndex, sizeof(_int), &dwByte, nullptr);
	//뼈의 이름
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);
	//뼈 행렬
	WriteFile(*pFile, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

	if (eType == CModel::TYPE_ANIM)
	{
		//조정 여부
		WriteFile(*pFile, &m_isNeedTuning, sizeof(_bool), &dwByte, nullptr);

		//조정 행렬
		if (m_isNeedTuning == true)
		{
			WriteFile(*pFile, &m_TuningMatrix, sizeof(_float4x4), &dwByte, nullptr);
		}

		if (bUseBoundary == true)
		{
			WriteFile(*pFile, &m_bIsChildOf_Boundary, sizeof(_bool), &dwByte, nullptr);
		}
	}
	
	return S_OK;
}

void CBone::SetUp_isNeedTuning(_bool bState)
{
	if (bState)
	{
		//현재 상태를 저장
		m_TuningMatrix = m_TransformationMatrix;
	}
	//뼈의 조정이 필요한지를 저장
	m_isNeedTuning = bState;
}

void CBone::SetUp_TuningState(_bool bState)
{
	m_isNeedTuning = bState;
}

CBone* CBone::Create(HANDLE* pFile, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pFile, iParentBoneIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Create_To_Binary(HANDLE* pFile, _bool bUseBoundary, _uint eType)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize_ToBinary(pFile, bUseBoundary, eType)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);	
}

void CBone::Free()
{
	__super::Free();

}
