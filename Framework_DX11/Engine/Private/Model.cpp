#include "..\Public\Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "Channel.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
	m_eComponentType = MODEL;
}

CModel::CModel(const CModel & Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes { Prototype.m_iNumMeshes } 
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials { Prototype.m_Materials}
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_iCurrentAnimIndex { Prototype.m_iCurrentAnimIndex }
	, m_iNumAnimations { Prototype.m_iNumAnimations }
	, m_Animations { Prototype.m_Animations }
	, m_CurrentTrackPosition { Prototype.m_CurrentTrackPosition }
	, m_KeyFrameIndices {Prototype.m_KeyFrameIndices }
{
	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());	

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}
		

	for (auto& pMesh : m_Meshes)	
		Safe_AddRef(pMesh);

	m_UFBIndices.reserve(UFB_END);

	for (size_t i = 0; i < UFB_END; i++)
	{
		m_UFBIndices.emplace_back(1024);
	}

	m_isCloned = true;
	m_eComponentType = MODEL;
}

_int CModel::Get_BoneIndex(const _char * pBoneName) const
{
	_uint	iBoneIndex = { 0 };
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		if (0 == strcmp(pBone->Get_Name(), pBoneName))
			return true;
		++iBoneIndex;
		return false;
	});

	if (iter == m_Bones.end())
		MSG_BOX(TEXT("뼈 정보 잘못됐음"));

	return iBoneIndex;
}

_char* CModel::Get_CurrentAnimationName()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Name();
}

_bool CModel::Get_IsEnd_Animation(_uint iAnimationIndex)
{
	if (iAnimationIndex != m_iCurrentAnimIndex)
		return false;

	return m_isEnd_Animations[iAnimationIndex];
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint		iFlag = { 0 };	
	
	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_eType = eType;

	_tchar szFinalPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, pModelFilePath, (_uint)strlen(pModelFilePath), szFinalPath, MAX_PATH);

	HANDLE hFile = CreateFile(szFinalPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(TEXT("Failed to Open Model data"));
		return E_FAIL;
	}

	if (FAILED(Ready_Bones(&hFile, -1)))
		return E_FAIL;
	for (auto& Bone : m_Bones)
		Bone->Setting_ParentBoneName(this);

	if (FAILED(Ready_Meshes(&hFile)))
		return E_FAIL;

	if (FAILED(Ready_Materials(&hFile, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations(&hFile)))
		return E_FAIL;


	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	m_isEnd_Animations = new _bool[m_iNumAnimations];
	ZeroMemory(m_isEnd_Animations, m_iNumAnimations * sizeof(_bool));

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();	

	return S_OK;
}

void CModel::SetUp_Animation(_uint iAnimationIndex, _bool isLoop)
{
	m_iCurrentAnimIndex = iAnimationIndex;
	m_isLoop = isLoop;
}

HRESULT CModel::SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop, _float fChangeDuration, _uint iStartFrame)
{
	if (iNextAnimationIndex >= m_iNumAnimations)
		return E_FAIL;

	// 이미 같은걸로 바꾸고 있어
	if (m_isChangeAni == true && iNextAnimationIndex == m_tChaneAnimDesc.iNextAnimIndex)
		return S_OK;

	m_ChangeTrackPosition = 0.0;

	m_tChaneAnimDesc.iNextAnimIndex = iNextAnimationIndex;
	m_tChaneAnimDesc.iStartFrame = iStartFrame;
	m_tChaneAnimDesc.fChangeDuration = fChangeDuration;
	m_tChaneAnimDesc.fChangeTime = 0.f;

	m_isEnd_Animations[iNextAnimationIndex] = false;
	m_isLoop = isLoop;
	m_isChangeAni = true;

	return S_OK;
}

_uint CModel::Setting_Animation(const _char* szAnimationmName, _double SpeedRatio) const
{
	_uint iAnimationIndex = { 0 };
	auto iter = find_if(m_Animations.begin(), m_Animations.end(), [&](CAnimation* pAnimation)-> _bool
		{
			if (0 == strcmp(szAnimationmName, pAnimation->Get_Name()))
				return true;
			++iAnimationIndex;
			return false;
		});

	if (iter == m_Animations.end())
		MSG_BOX(TEXT("애니메이션이 존재하지 않음 확인해봐"));

	m_Animations[iAnimationIndex]->Set_SpeedRatio(SpeedRatio);

	return iAnimationIndex;
}

_vector CModel::Play_Animation(_float fTimeDelta, _bool* pOut)
{
	if (m_isChangeAni)
	{
		_bool isChangeEnd = false;
		if (m_tChaneAnimDesc.fChangeTime == 0.f && pOut != nullptr)
		{
			*pOut = true;
		}
		m_tChaneAnimDesc.fChangeTime += fTimeDelta;

		vector<CChannel*> CurrentChannels = m_Animations[m_iCurrentAnimIndex]->Get_Channels();
		vector<CChannel*> NextChannels = m_Animations[m_tChaneAnimDesc.iNextAnimIndex]->Get_Channels();

		m_CurrentTrackPosition += fTimeDelta;
		for (_int i = 0; i < CurrentChannels.size(); ++i)
		{

			KEYFRAME tCurrentKeyFrame = CurrentChannels[i]->Find_KeyFrameIndex(&m_KeyFrameIndices[m_iCurrentAnimIndex][i], m_CurrentTrackPosition); // 여기서부터
			KEYFRAME tNextKeyFrame = NextChannels[i]->Find_KeyFrameIndex(&m_KeyFrameIndices[m_tChaneAnimDesc.iNextAnimIndex][i], NextChannels[i]->Get_KeyFrame(m_tChaneAnimDesc.iStartFrame).TrackPosition); // 여기로 보간

			_vector vScale, vRotation, vTranslation;

			/* 애니메이션 전환 시간이 끝났으면 */
			if (m_tChaneAnimDesc.fChangeTime >= m_tChaneAnimDesc.fChangeDuration)
			{
				vScale = XMLoadFloat3(&tNextKeyFrame.vScale);
				vRotation = XMLoadFloat4(&tNextKeyFrame.vRotation);
				vTranslation = XMVectorSetW(XMLoadFloat3(&tNextKeyFrame.vTranslation), 1.f);

				isChangeEnd = true;
			}
			else
			{
				_vector		vSourScale = XMLoadFloat3(&tCurrentKeyFrame.vScale);
				_vector		vDestScale = XMLoadFloat3(&tNextKeyFrame.vScale);

				_vector		vSourRotation = XMLoadFloat4(&tCurrentKeyFrame.vRotation);
				_vector		vDestRotation = XMLoadFloat4(&tNextKeyFrame.vRotation);

				_vector		vSourTranslation = XMVectorSetW(XMLoadFloat3(&tCurrentKeyFrame.vTranslation), 1.f);
				_vector		vDestTranslation = XMVectorSetW(XMLoadFloat3(&tNextKeyFrame.vTranslation), 1.f);

				_float		fRatio = m_tChaneAnimDesc.fChangeTime / m_tChaneAnimDesc.fChangeDuration;

				vScale = XMVectorLerp(vSourScale, vDestScale, (_float)fRatio);
				vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)fRatio);
				vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)fRatio);
			}

			_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

			m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Set_TransformationMatrix(TransformationMatrix);
		}

		if (isChangeEnd)
		{
			m_CurrentTrackPosition = 0.0;
			//m_Animations[m_iCurrentAnimIndex]->Reset();
			m_iCurrentAnimIndex = m_tChaneAnimDesc.iNextAnimIndex;
			ZeroMemory(&m_tChaneAnimDesc, sizeof(CHANGEANIMATION_DESC));

			m_isChangeAni = false;
		}
	}
	else
	{
		/* 뼈를 움직인다.(CBone`s m_TransformationMatrix행렬을 갱신한다.) */
		m_iCurrentFrame = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex], m_isLoop, &m_isEnd_Animations[m_iCurrentAnimIndex], fTimeDelta);
	}

	_vector vRootMove{};
	/* 모든 뼈가 가지고 있는 m_CombinedTransformationMatrix를 갱신한다. */
	if (m_UFBIndices[UFB_ROOT] != 1024)
	{
		m_Bones[m_UFBIndices[UFB_ROOT]]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		vRootMove = m_Bones[m_UFBIndices[UFB_ROOT]]->Get_CombinedTransformationMatrix().r[3];
		if (m_CurrentTrackPosition == 0.f && pOut != nullptr)//애니메이션이 끝났는지에 대한 판단
		{
			*pOut = true;
		}
		_float4x4 RootMat = {};
		XMStoreFloat4x4(&RootMat, m_Bones[m_UFBIndices[UFB_ROOT]]->Get_TransformationMatrix());
		RootMat._41 = 0; RootMat._42 = 0; RootMat._43 = 0;

		m_Bones[m_UFBIndices[UFB_ROOT]]->Set_TransformationMatrix(XMLoadFloat4x4(&RootMat));
	}

	for (auto& pBone : m_Bones)
	{
		/* 내 뼈의 행렬 * 부모의 컴바인드 */
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return vRootMove;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, TEXTURE_TYPE eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType]->Bind_ShadeResource(pShader, pConstantName, 0);	
}

HRESULT CModel::Bind_MeshBoneMatrices(CShader * pShader, const _char * pConstantName, _uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(this, pShader, pConstantName);

	return S_OK;
}

HRESULT CModel::Ready_Meshes(HANDLE* pFile)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, pFile, this, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(HANDLE* pFile, const _char* pModelFilePath)
{
	_ulong dwByte = 0;

	// 머터리얼의 개수 저장해두기
	ReadFile(*pFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL		MeshMaterial{};

		// 텍스쳐는 최대 18개까지 저장 가능
		for (size_t j = 1; j < TEXTURE_TYPE_MAX; j++)
		{
			_bool isHaveTexture{};
			ReadFile(*pFile, &isHaveTexture, sizeof(_bool), &dwByte, nullptr);

			if (!isHaveTexture)
				continue;

			_char				szTexturePath[MAX_PATH] = "";

			ReadFile(*pFile, szTexturePath, MAX_PATH, &dwByte, nullptr);

			_tchar				szFinalPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTexturePath, (_int)(strlen(szTexturePath)), szFinalPath, MAX_PATH);

			if (m_pGameInstance)

				MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath, 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(HANDLE* pFile, _int iParentBoneIndex)
{
	_ulong dwByte = 0;

	CBone* pBone = CBone::Create(pFile, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint iNumChildren = { 0 };
	ReadFile(*pFile, &iNumChildren, sizeof(_uint), &dwByte, nullptr);

	_int		iParentIndex = (_int)m_Bones.size() - 1;

	for (size_t i = 0; i < iNumChildren; ++i)
	{
		Ready_Bones(pFile, iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(HANDLE* pFile)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	m_KeyFrameIndices.resize(m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(pFile, m_KeyFrameIndices[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}


CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	if (m_isCloned)
		Safe_Delete_Array(m_isEnd_Animations);

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();
}
