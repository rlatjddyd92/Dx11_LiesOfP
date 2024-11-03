#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(HANDLE* pFile, const aiNode* pAIBone, _int iParentBoneIndex)
{
	_ulong dwByte = 0;

	m_iParentBoneIndex = iParentBoneIndex;
	//WriteFile(*pFile, &m_iParentBoneIndex, sizeof(_int), &dwByte, nullptr);

    strcpy_s(m_szName, pAIBone->mName.data);
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

    memcpy(&m_TransformationMatrix, &pAIBone->mTransformation, sizeof(_float4x4));

    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	WriteFile(*pFile, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

	// 항등일뿐이니까 그냥 저장 안함
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

CBone* CBone::Create(HANDLE* pFile, const aiNode* pAIBone, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pFile, pAIBone, iParentBoneIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone::Free()
{
	__super::Free();
}