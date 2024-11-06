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

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if(-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
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

CBone * CBone::Clone()
{
	return new CBone(*this);	
}

void CBone::Free()
{
	__super::Free();

}
