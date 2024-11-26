#include "..\Public\Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "Channel.h"

#include "GameInstance.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
	, m_isEnd_Animations	{ nullptr }
	, m_isEnd_Animations_Boundary { nullptr }
{
	m_isCloned = false;
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
	, m_FilePaths { Prototype.m_FilePaths }
	, m_isUseBoundary { Prototype.m_isUseBoundary }
	, m_UFBIndices { Prototype.m_UFBIndices }
	, m_UseFullVtxIndices{ Prototype.m_UseFullVtxIndices }
	, m_isEnd_Animations{ nullptr }
	, m_isEnd_Animations_Boundary{ nullptr }
	, m_eType{ Prototype.m_eType }
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
	{
		CalculateBoundingBox_Model(pMesh, m_vMinPos, m_vMaxPos);
		Safe_AddRef(pMesh);
	}

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

_uint	CModel::Get_CurrentFrame(_bool isBoundary)
{
	if (!isBoundary)
	{
		if (m_isChangeAni)
		{
			return m_tChaneAnimDesc.iStartFrame;
		}
		else
			return m_iCurrentFrame;
	}

	else
	{
		if (m_isChangeAni_Boundary)
		{
			return m_tChaneAnimDesc_Boundary.iStartFrame;
		}
		else
			return m_iCurrentFrame;
	}
}

_double CModel::Get_CurrentTrackPosition(_bool isBoundary)
{

	if (isBoundary)
	{
		if (m_isChangeAni_Boundary)
		{
			return 0;
		}
		else
			return m_CurrentTrackPosition_Boundary;
	}
	else
	{
		if (m_isChangeAni)
		{
			return 0;
		}
		else
			return m_CurrentTrackPosition;
	}

}

CTexture* CModel::Find_Texture(_uint iMeshNum, TEXTURE_TYPE eMaterialType)
{
	_uint iMaterialIndex = m_Meshes[iMeshNum]->Get_MaterialIndex();
	return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType];
}

HRESULT CModel::Update_Boundary()
{
	_int iBoundaryBoneIndex = m_UFBIndices[UFB_BOUNDARY_UPPER];
	if (iBoundaryBoneIndex != 1024)
	{
		for (_int i = 0; i < m_Bones.size(); ++i)
		{
			m_Bones[i]->Update_Boundary(m_Bones, i, iBoundaryBoneIndex);
		}
		m_iCurrentAnimIndex_Boundary = m_iCurrentAnimIndex;
		m_isUseBoundary = true;
	}
	else
	{
		m_isUseBoundary = false;
		return E_FAIL;
	}
	
	return S_OK;
}

void CModel::SetUp_isNeedTuning(_int iBoneIndex, _bool bState)
{
	m_Bones[iBoneIndex]->SetUp_isNeedTuning(bState);
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix, _bool isBinaryAnimModel, FilePathStructStack* pStructStack)
{
	if (pStructStack != nullptr)
	{
		m_FilePaths = pStructStack;
	}

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

	if (isBinaryAnimModel)
	{//변경된 애니메이션 모델이라는 표시 -> 새로만든 바이너리 파일을 통해서 호출
		ReadyModel_To_Binary(&hFile);
		Update_Boundary();
	}
	else			//추가되기 전 일반 바이너리 파일로 불러오는
	{
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
	}

	CloseHandle(hFile);

	for (auto pAnimation : m_Animations)
	{
		pAnimation->Find_ChannelWide();
	}

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	m_isEnd_Animations = new _bool[m_iNumAnimations];
	m_isEnd_Animations_Boundary = new _bool[m_iNumAnimations];
	ZeroMemory(m_isEnd_Animations, m_iNumAnimations * sizeof(_bool));
	ZeroMemory(m_isEnd_Animations_Boundary, m_iNumAnimations * sizeof(_bool));

	m_isInstance = (_bool*)pArg;

	if (m_isInstance)
	{
		m_iInstanceStride = sizeof(VTXMODELINSTANCE);
		m_iNumInstance = 100;

		ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
		m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
		m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 동적버퍼로 생성한다. */
		m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_InstanceBufferDesc.MiscFlags = 0;
		m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

		m_pInstanceVertices = new VTXMODELINSTANCE[m_iNumInstance];
		ZeroMemory(m_pInstanceVertices, sizeof(VTXMODELINSTANCE) * m_iNumInstance);

		VTXMODELINSTANCE* pInstanceVertices = static_cast<VTXMODELINSTANCE*>(m_pInstanceVertices);

		_float	fScale = 10.f;
		for (size_t i = 0; i < m_iNumInstance; i++)
		{
			pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
			pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
			pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
			pInstanceVertices[i].vTranslation = _float4(_float(i * i), 1.f, 1.f, 1.f);
		}

		ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
		m_InstanceInitialData.pSysMem = m_pInstanceVertices;

		if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance)))
			return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();	

	return S_OK;
}



HRESULT CModel::Render_Instance(_uint iMeshIndex)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pVertices = static_cast<VTXMODELINSTANCE*>(SubResource.pData);

	memcpy(SubResource.pData, m_InstanceDatas.data(), sizeof(VTXMODELINSTANCE) * m_InstanceDatas.size());

	/*_float4x4 v[10];
	_float4x4* pData = reinterpret_cast<_float4x4*>(SubResource.pData);
	for (int i = 0; i < 10; ++i) {
		v[i] = pData[i];
	}*/

	m_pContext->Unmap(m_pVBInstance, 0);

	m_Meshes[iMeshIndex]->Bind_Buffers_Instance(m_pVBInstance);

	_uint iNumIndices = m_Meshes[iMeshIndex]->Get_NumIndices();
	m_pContext->DrawIndexedInstanced(m_Meshes[iMeshIndex]->Get_NumIndices(), (_uint)m_InstanceDatas.size(), 0, 0, 0);

	return S_OK;
}

_uint CModel::Find_AnimationIndex(const _char* pAnimationmName, _float fSpeedRatio)
{
	_uint iAnimationIndex = { 0 };
	auto iter = find_if(m_Animations.begin(), m_Animations.end(), [&](CAnimation* pAnimation)-> _bool
		{
			if (0 == strcmp(pAnimationmName, pAnimation->Get_Name()))
				return true;
			++iAnimationIndex;
			return false;
		});

	if (iter == m_Animations.end())
		MSG_BOX(TEXT("없는데?"));

	m_Animations[iAnimationIndex]->Set_SpeedRatio(fSpeedRatio);

	return iAnimationIndex;
}

void CModel::SetUp_Animation(_uint iAnimationIndex, _bool isLoop)
{
	m_iCurrentAnimIndex = iAnimationIndex;
	m_isLoop = isLoop;
	m_CurrentTrackPosition = 0.0;
}

HRESULT CModel::SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop, _float fChangeDuration, _uint iStartFrame, _bool bEitherBoundary, _bool bSameChange)
{
	if (iNextAnimationIndex >= m_iNumAnimations)
		return E_FAIL;

	//같은걸로 바꿀순 없어
	if (m_iCurrentAnimIndex == iNextAnimationIndex && !m_isChangeAni && !bSameChange)
		return E_FAIL;

	// 이미 같은걸로 바꾸고 있어
	if (m_isChangeAni == true && iNextAnimationIndex == m_tChaneAnimDesc.iNextAnimIndex && !bSameChange)
		return S_OK;

	m_tChaneAnimDesc.iNextAnimIndex = iNextAnimationIndex;
	if (iNextAnimationIndex == m_iCurrentAnimIndex_Boundary && !m_isChangeAni_Boundary)
	{
		m_tChaneAnimDesc.iStartFrame = (_uint)m_CurrentTrackPosition_Boundary;
	}
	else
	{
		m_tChaneAnimDesc.iStartFrame = iStartFrame;
	}
	vector<CChannel*> NextChannels = m_Animations[m_tChaneAnimDesc.iNextAnimIndex]->Get_Channels();

	m_ChangeTrackPosition = 0.0;

	if(iStartFrame > 0)
		m_ChangeTrackPosition = m_Animations[m_tChaneAnimDesc.iNextAnimIndex]->Get_WideChannel()->Get_KeyFrame(iStartFrame).TrackPosition;

	m_vRootMoveStack = m_vCurRootMove = _vector{ 0, 0, 0, 1 };

	m_tChaneAnimDesc.fChangeDuration = fChangeDuration;
	m_tChaneAnimDesc.fChangeTime = 0.f;


	if (bEitherBoundary)
	{
		SetUp_NextAnimation_Boundary(iNextAnimationIndex, isLoop, fChangeDuration, iStartFrame, bSameChange);
	}

	m_isEnd_Animations[iNextAnimationIndex] = false;
	m_isEnd_Animations[m_iCurrentAnimIndex] = false;
	m_isLoop = isLoop;
	m_isChangeAni = true;

	return S_OK;
}

HRESULT CModel::SetUp_NextAnimation_Boundary(_uint iNextAnimationIndex, _bool isLoop, _float fChangeDuration, _uint iStartFrame, _bool bSameChange)
{
	if (iNextAnimationIndex >= m_iNumAnimations)
		return E_FAIL;

	//같은걸로 바꿀순 없어
	if (m_iCurrentAnimIndex_Boundary == iNextAnimationIndex && !m_isChangeAni_Boundary && !bSameChange)	//만약 지금 바꾸는중이 아니라면
		return E_FAIL;

	// 이미 같은걸로 바꾸고 있어														이미 변환이 진행중인데, 같은 변환이 들어왔으면
	if (m_isChangeAni_Boundary == true && iNextAnimationIndex == m_tChaneAnimDesc_Boundary.iNextAnimIndex)
		return S_OK;

	m_ChangeTrackPosition_Boundary = 0.0;

	m_tChaneAnimDesc_Boundary.iNextAnimIndex = iNextAnimationIndex;
	if (iNextAnimationIndex == m_iCurrentAnimIndex && !m_isChangeAni)
	{
		m_tChaneAnimDesc_Boundary.iStartFrame = (_uint)m_CurrentTrackPosition;
	}
	else
	{
		m_tChaneAnimDesc_Boundary.iStartFrame = iStartFrame;
	}

	vector<CChannel*> NextChannels = m_Animations[iNextAnimationIndex]->Get_Channels();

	for (_uint i = 0; i < NextChannels.size(); ++i)
	{
		KEYFRAME tNextKeyFrame = NextChannels[i]->Find_KeyFrameIndex(&m_KeyFrameIndices[iNextAnimationIndex][i], NextChannels[i]->Get_KeyFrame(iStartFrame).TrackPosition); // 여기로 보간

		if (m_ChangeTrackPosition_Boundary < tNextKeyFrame.TrackPosition)
		{
			m_ChangeTrackPosition_Boundary = tNextKeyFrame.TrackPosition;

		}
	}

	m_vRootMoveStack = m_vCurRootMove = _vector{ 0, 0, 0, 1 };

	m_tChaneAnimDesc_Boundary.fChangeDuration = fChangeDuration;
	m_tChaneAnimDesc_Boundary.fChangeTime = 0.f;
	

	m_isEnd_Animations_Boundary[iNextAnimationIndex] = false;
	m_isEnd_Animations_Boundary[m_iCurrentAnimIndex_Boundary] = false;
	m_isLoop_Boundary = isLoop;
	m_isChangeAni_Boundary = true;

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

_matrix CModel::CalcMatrix_forVtxAnim(_uint iMeshNum, VTXANIMMESH VtxStruct)
{
	return m_Meshes[iMeshNum]->CalcMatrix_forVtxAnim(m_Bones, VtxStruct); //m_isUseBoundary
}

void CModel::Update_Bone()
{
	for (auto& pBone : m_Bones)
	{
		/* 내 뼈의 행렬 * 부모의 컴바인드 */
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

}

_Vec3 CModel::Play_Animation(_float fTimeDelta, list<OUTPUT_EVKEY>* pEvKeyList)
{
	_bool	bRootCheck{false};
	if (m_UFBIndices[UFB_ROOT] != 1024)
	{
		bRootCheck = true;
		m_isUseRootBone = true;
	}

	// ==m_isUseBoundary 로 변경
	_float fAddTime = fTimeDelta * m_bPlayAnimCtr;
	m_isBoneUpdated = false; //뼈 업데이트 함수 호출시에 실제 업데이트 됐는지, 루프 애니메이션이 아니라서 중단됐는지 확인하기위한 불 변수

	//상하체 분리에 영향받지 않는 부분들의 업데이트
	Update_Animation(fAddTime, pEvKeyList);

	//저 부모뼈들 이름으로 인덱스 찾아서 해당 인덱스 업데이트 멈추고 확인해서 나머지 팔은 잘 움직인다면

	//분리된 부분 업데이트
	Update_Animation_Boundary(fAddTime, pEvKeyList);


	//루트본에 의한 이동값을 루트본에서 제거하고 이동량만큼 바깥으로 배출
	_vector vRootMove = Finish_Update_Anim();
	
	return vRootMove;
}

void CModel::Update_Animation(_float fTimeDelta, list<OUTPUT_EVKEY>* pEvKeyList)
{
	if (m_isChangeAni)
	{
		_bool isChangeEnd = false;

		m_tChaneAnimDesc.fChangeTime += fTimeDelta;

		vector<CChannel*> CurrentChannels = m_Animations[m_iCurrentAnimIndex]->Get_Channels();
		vector<CChannel*> NextChannels = m_Animations[m_tChaneAnimDesc.iNextAnimIndex]->Get_Channels();

		//m_CurrentTrackPosition += fTimeDelta;
		for (_int i = 0; i < CurrentChannels.size(); ++i)
		{
			if (m_isUseBoundary && m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Get_IsChildOf_Boundary() == true)
			{
				continue;
			}

			if (m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Get_isNeedTuning())
			{
				m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Apply_Tuning();
				continue;
			}


			KEYFRAME tCurrentKeyFrame = CurrentChannels[i]->Find_KeyFrameIndex(&m_KeyFrameIndices[m_iCurrentAnimIndex][i], m_CurrentTrackPosition); // 여기서부터
			KEYFRAME tNextKeyFrame = NextChannels[i]->Find_KeyFrameIndex(&m_KeyFrameIndices[m_tChaneAnimDesc.iNextAnimIndex][i], m_ChangeTrackPosition); // 여기로 보간


			_vector vScale, vRotation, vTranslation;

			/* 애니메이션 전환 시간이 끝났으면 */
			if (m_tChaneAnimDesc.fChangeTime >= m_tChaneAnimDesc.fChangeDuration)
			{
				vScale = XMLoadFloat3(&tNextKeyFrame.vScale);
				vRotation = XMLoadFloat4(&tNextKeyFrame.vRotation);
				if (m_UFBIndices[UFB_ROOT] == CurrentChannels[i]->Get_BoneIndex())
				{
					vTranslation = _vector{ 0, 0, 0, 1 };
				}
				else
					vTranslation = XMVectorSetW(XMLoadFloat3(&tNextKeyFrame.vTranslation), 1.f);

				isChangeEnd = true;
			}
			else
			{
				_vector		vSourScale = XMLoadFloat3(&tCurrentKeyFrame.vScale);
				_vector		vDestScale = XMLoadFloat3(&tNextKeyFrame.vScale);

				_vector		vSourRotation = XMLoadFloat4(&tCurrentKeyFrame.vRotation);
				_vector		vDestRotation = XMLoadFloat4(&tNextKeyFrame.vRotation);

				_vector		vSourTranslation{};
				_vector		vDestTranslation{};

				if (m_UFBIndices[UFB_ROOT] == CurrentChannels[i]->Get_BoneIndex())
				{
					vDestTranslation = vSourTranslation = _vector{ 0, 0, 0, 1 };
				}
				else
				{
					vSourTranslation = XMVectorSetW(XMLoadFloat3(&tCurrentKeyFrame.vTranslation), 1.f);
					vDestTranslation = XMVectorSetW(XMLoadFloat3(&tNextKeyFrame.vTranslation), 1.f);
				}

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
			if (m_tChaneAnimDesc.iNextAnimIndex == m_iCurrentAnimIndex_Boundary)
			{
				m_CurrentTrackPosition = m_CurrentTrackPosition_Boundary;
			}
			else
				m_CurrentTrackPosition = m_ChangeTrackPosition;
			//m_Animations[m_iCurrentAnimIndex]->Reset();

			//m_iCurrentFrame = m_Animations[m_iCurrentAnimIndex].
			m_iCurrentFrame = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex], m_isLoop, &m_isEnd_Animations[m_iCurrentAnimIndex], 0, false, nullptr, &m_isBoneUpdated);

			m_vRootMoveStack = m_vCurRootMove = _vector{ 0, 0, 0, 1 };
			m_iCurrentAnimIndex = m_tChaneAnimDesc.iNextAnimIndex;
			ZeroMemory(&m_tChaneAnimDesc, sizeof(CHANGEANIMATION_DESC));

			m_isChangeAni = false;
		}
	}
	else
	{

		/* 뼈를 움직인다.(CBone`s m_TransformationMatrix행렬을 갱신한다.) */
		m_iCurrentFrame = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex], m_isLoop, &m_isEnd_Animations[m_iCurrentAnimIndex], fTimeDelta, false, pEvKeyList, &m_isBoneUpdated);

	}
}

void CModel::Update_Animation_Boundary(_float fTimeDelta, list<OUTPUT_EVKEY>* pEvKeyList)
{
	if (m_isUseBoundary)
	{
		if (m_isChangeAni_Boundary)
		{
			_bool isChangeEnd = false;

			m_tChaneAnimDesc_Boundary.fChangeTime += fTimeDelta;

			vector<CChannel*> CurrentChannels = m_Animations[m_iCurrentAnimIndex_Boundary]->Get_Channels();
			vector<CChannel*> NextChannels = m_Animations[m_tChaneAnimDesc_Boundary.iNextAnimIndex]->Get_Channels();


			if (m_isChangeAni && (m_tChaneAnimDesc_Boundary.iNextAnimIndex == m_tChaneAnimDesc.iNextAnimIndex))
			{
				m_CurrentTrackPosition_Boundary = m_CurrentTrackPosition;
			}

			for (_int i = 0; i < CurrentChannels.size(); ++i)
			{
				if (m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Get_IsChildOf_Boundary() == false)
				{
					continue;
				}

				if (m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Get_isNeedTuning())
				{
					m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Apply_Tuning();
					continue;
				}

				KEYFRAME tCurrentKeyFrame = CurrentChannels[i]->Find_KeyFrameIndex(&m_KeyFrameIndices[m_iCurrentAnimIndex_Boundary][i], m_CurrentTrackPosition_Boundary); // 여기서부터
				KEYFRAME tNextKeyFrame = NextChannels[i]->Find_KeyFrameIndex(&m_KeyFrameIndices[m_tChaneAnimDesc_Boundary.iNextAnimIndex][i], m_ChangeTrackPosition_Boundary); // 여기로 보간

				_vector vScale, vRotation, vTranslation;

				/* 애니메이션 전환 시간이 끝났으면 */
				if (m_tChaneAnimDesc_Boundary.fChangeTime >= m_tChaneAnimDesc_Boundary.fChangeDuration)
				{
					vScale = XMLoadFloat3(&tNextKeyFrame.vScale);
					vRotation = XMLoadFloat4(&tNextKeyFrame.vRotation);

					if (m_UFBIndices[UFB_ROOT] == CurrentChannels[i]->Get_BoneIndex())
					{
						vTranslation = _vector{ 0, 0, 0, 1 };
					}
					else
						vTranslation = XMVectorSetW(XMLoadFloat3(&tNextKeyFrame.vTranslation), 1.f);

					isChangeEnd = true;
				}
				else
				{
					_vector		vSourScale = XMLoadFloat3(&tCurrentKeyFrame.vScale);
					_vector		vDestScale = XMLoadFloat3(&tNextKeyFrame.vScale);

					_vector		vSourRotation = XMLoadFloat4(&tCurrentKeyFrame.vRotation);
					_vector		vDestRotation = XMLoadFloat4(&tNextKeyFrame.vRotation);

					_vector		vSourTranslation{};
					_vector		vDestTranslation{};

					if (m_UFBIndices[UFB_ROOT] == CurrentChannels[i]->Get_BoneIndex())
					{
						vDestTranslation = vSourTranslation = _vector{ 0, 0, 0, 1 };
					}
					else
					{
						vSourTranslation = XMVectorSetW(XMLoadFloat3(&tCurrentKeyFrame.vTranslation), 1.f);
						vDestTranslation = XMVectorSetW(XMLoadFloat3(&tNextKeyFrame.vTranslation), 1.f);
					}

					_float		fRatio = m_tChaneAnimDesc_Boundary.fChangeTime / m_tChaneAnimDesc_Boundary.fChangeDuration;


					vScale = XMVectorLerp(vSourScale, vDestScale, (_float)fRatio);
					vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)fRatio);
					vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)fRatio);
				}

				_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

				m_Bones[CurrentChannels[i]->Get_BoneIndex()]->Set_TransformationMatrix(TransformationMatrix);
			}

			if (isChangeEnd)
			{
				if (m_tChaneAnimDesc_Boundary.iNextAnimIndex == m_iCurrentAnimIndex && !m_isChangeAni)
				{
					m_CurrentTrackPosition_Boundary = m_CurrentTrackPosition_Boundary;
				}
				else
					m_CurrentTrackPosition_Boundary = m_ChangeTrackPosition_Boundary;

				//m_Animations[m_iCurrentAnimIndex]->Reset();
				m_iCurrentFrame_Boundary = m_Animations[m_iCurrentAnimIndex_Boundary]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex_Boundary], m_isLoop, &m_isEnd_Animations_Boundary[m_iCurrentAnimIndex_Boundary], 0, true, nullptr, &m_isBoneUpdated, true);
				m_vRootMoveStack = m_vCurRootMove = _vector{ 0, 0, 0, 1 };
				m_iCurrentAnimIndex_Boundary = m_tChaneAnimDesc_Boundary.iNextAnimIndex;
				ZeroMemory(&m_tChaneAnimDesc_Boundary, sizeof(CHANGEANIMATION_DESC));

				m_isChangeAni_Boundary = false;
			}
		}
		else
		{
			//if 상하체 애니메이션이 같으면 시간누적 없는 업데이트 하체 변수로 호출
			if (m_iCurrentAnimIndex == m_iCurrentAnimIndex_Boundary && !m_isChangeAni)
			{
				m_iCurrentFrame_Boundary = m_Animations[m_iCurrentAnimIndex_Boundary]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition, m_KeyFrameIndices[m_iCurrentAnimIndex_Boundary], m_isLoop, &m_isEnd_Animations_Boundary[m_iCurrentAnimIndex_Boundary], fTimeDelta, true, pEvKeyList, &m_isBoneUpdated, true);
			}
			else
			{
				m_iCurrentFrame_Boundary = m_Animations[m_iCurrentAnimIndex_Boundary]->Update_TransformationMatrices(m_Bones, &m_CurrentTrackPosition_Boundary, m_KeyFrameIndices[m_iCurrentAnimIndex_Boundary], m_isLoop_Boundary, &m_isEnd_Animations_Boundary[m_iCurrentAnimIndex_Boundary], fTimeDelta, true, pEvKeyList, &m_isBoneUpdated, false);
			}
		}

	}
}

_vector CModel::Finish_Update_Anim()
{
	_vector vRootMove{};

	if (m_isBoneUpdated)
	{
		if (m_isUseRootBone)
		{
			m_Bones[m_UFBIndices[UFB_ROOT]]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
			m_vCurRootMove = m_Bones[m_UFBIndices[UFB_ROOT]]->Get_CombinedTransformationMatrix().Translation();
			_float4x4 RootMat = {};
			XMStoreFloat4x4(&RootMat, m_Bones[m_UFBIndices[UFB_ROOT]]->Get_TransformationMatrix());
			RootMat._41 = 0; RootMat._42 = 0; RootMat._43 = 0;

			m_Bones[m_UFBIndices[UFB_ROOT]]->Set_TransformationMatrix(XMLoadFloat4x4(&RootMat));

		}
	}

	if (m_isEnd_Animations[m_iCurrentAnimIndex])
	{
		m_vRootMoveStack = m_vCurRootMove = _vector{0, 0, 0, 1};
	}

	if (XMVectorGetX(XMVector3Length(m_vRootMoveStack)) == 0
		 || XMVectorGetX(XMVector3Length(m_vCurRootMove)) == 0)
	{
		vRootMove = _vector{ 0, 0, 0, 1 };
		m_vRootMoveStack = m_vCurRootMove;
	}
	else
	{
		vRootMove = m_vCurRootMove - m_vRootMoveStack;
		m_vRootMoveStack = m_vCurRootMove;
	}



	//for (_int i = 0; i < m_Bones.size(); ++i)
	//{
	//	if (i == 560 || i ==756)
	//	{
	//		m_Bones[i]->Apply_SaveCombined();
	//	}
	//	else
	//	{
	//		m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	//
	//	}
	//}

	/* 모든 뼈가 가지고 있는 m_CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		/* 내 뼈의 행렬 * 부모의 컴바인드 */
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return vRootMove;
}


HRESULT CModel::Create_BinaryFile(const _char* ModelTag)
{
	_ulong dwByte = 0;

#pragma region 경로 세팅하기
	// 경로, 이름, 확장자 정하기
	string strFilePath;
	strFilePath.assign(ModelTag);

	//"Prototype_AnimModel_Test"  앞부분 제외 뒷부분 이름으로 사용

	strFilePath.erase(0, 20);

	char szCreateFolderPath[MAX_PATH];
	if (m_eType == TYPE_NONANIM)
		strcpy_s(szCreateFolderPath, "../Bin/ModelData/NonAnim/CreatedBinFiles/");
	else
		strcpy_s(szCreateFolderPath, "../Bin/ModelData/Anim/CreatedBinFiles/");

	string strDat = "";
	strDat.assign(".Dat");

	strcat_s(szCreateFolderPath, strFilePath.c_str());
	strcat_s(szCreateFolderPath, strDat.c_str());
	strFilePath.assign(szCreateFolderPath);

	_tchar szFinalPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, strFilePath.c_str(), (_uint)strlen(strFilePath.c_str()), szFinalPath, MAX_PATH);

#pragma endregion

	HANDLE hFile = CreateFile(szFinalPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MSG_BOX(TEXT("Failed to Open Models data"));
		return E_FAIL;
	}

	//상하체 분리 여부
	WriteFile(hFile, &m_isUseBoundary, sizeof(_bool), &dwByte, nullptr);

	//사용하려고 저장한 뼈
	for (_int i = 0; i < UFB_END; ++i)
	{
		WriteFile(hFile, &m_UFBIndices[i], sizeof(_uint), &dwByte, nullptr);
	}

	//아래 정점의 갯수
	_int iNumUFVtx = (_int)m_UseFullVtxIndices.size();
	WriteFile(hFile, &iNumUFVtx, sizeof(_int), &dwByte, nullptr);
	//사용하려고 저장한 정점
	for (int j = 0; j < iNumUFVtx; ++j)
	{
		WriteFile(hFile, &m_UseFullVtxIndices[j], sizeof(UFVTX), &dwByte, nullptr);
	}

	if (FAILED(Create_Bin_Bones(&hFile)))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	if (FAILED(Create_Bin_Meshes(&hFile)))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}
		
	if (FAILED(Create_Bin_Materials(&hFile)))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}
	
	if (FAILED(Create_Bin_Animations(&hFile)))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::Create_Bin_Bones(HANDLE* pFile)
{
	_ulong dwByte = 0;

	_uint iNumBone = (_uint)m_Bones.size();
	//뼈의 갯수
	WriteFile(*pFile, &iNumBone, sizeof(_uint), &dwByte, nullptr);

	for (auto& pBone : m_Bones)
	{//뼈의 바이너리화 함수 호출
		pBone->Create_BinaryFile(pFile, m_isUseBoundary, m_eType);
	}
	return S_OK;
}

HRESULT CModel::Create_Bin_Meshes(HANDLE* pFile)
{
	_ulong dwByte = 0;
	//메쉬의 갯수
	WriteFile(*pFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (auto& pMesh : m_Meshes)
	{//뼈의 바이너리화 함수 호출
		pMesh->Create_BinaryFile(pFile);
	}

	return S_OK;
}

HRESULT CModel::Create_Bin_Materials(HANDLE* pFile)
{
	_ulong dwByte = 0;
	//머티리얼 갯수 저장
	WriteFile(*pFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	_int iMaterialNum{};
	for (auto & pMaterialSet : m_Materials)
	{//머티리얼 저장 수 가 최대 수만큼 반복

		_int iTextureNum{};
		for (_int i = 0; i < TEXTURE_TYPE_MAX; ++i)
		{
			pMaterialSet.pMaterialTextures;

			_bool isHaveTexture = true;
			if (pMaterialSet.pMaterialTextures[i] == nullptr)
			{//저장 할 것이 없다면 스킵
				isHaveTexture = false;;
			}
			//텍스쳐 존재 여부를 저장
			WriteFile(*pFile, &isHaveTexture, sizeof(_bool), &dwByte, nullptr);

			if (isHaveTexture)
			{//텍스쳐가 있을 경우에만 저장


				WriteFile(*pFile, m_FilePaths->pStruct[iMaterialNum].m_ModelFilePaths[iTextureNum].c_str(), MAX_PATH, &dwByte, nullptr);
				++iTextureNum;
			}
		}
		++iMaterialNum;
	}

	return S_OK;
}

HRESULT CModel::Create_Bin_Animations(HANDLE* pFile)
{
	_ulong dwByte = 0;
	//애니메이션 수 저장
	WriteFile(*pFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	for (auto& pAnimation : m_Animations)
	{
		pAnimation->Create_BinaryFile(pFile);
	}
	return S_OK;
}

HRESULT CModel::ReadyModel_To_Binary(HANDLE* pFile)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_isUseBoundary, sizeof(_bool), &dwByte, nullptr);

	m_UFBIndices.resize(UFB_END);

	for (_int i = 0; i < UFB_END; ++i)
	{
		ReadFile(*pFile, &m_UFBIndices[i], sizeof(_uint), &dwByte, nullptr);
	}

	_int iNumUFVtx = {};
	ReadFile(*pFile, &iNumUFVtx, sizeof(_int), &dwByte, nullptr);

	for (_int i = 0; i < iNumUFVtx; ++ i)
	{
		ReadFile(*pFile, &m_UseFullVtxIndices[i], sizeof(UFVTX), &dwByte, nullptr);
	}
	//여기까지 모델.
	

	_uint iNumBone = {};
	ReadFile(*pFile, &iNumBone, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNumBone; ++ i)
	{
		CBone* pBone = CBone::Create_To_Binary(pFile, m_isUseBoundary, m_eType);
		if (pBone == nullptr)
			return E_FAIL;
		
		m_Bones.emplace_back(pBone);
	}
	//여기까지 뼈


	ReadFile(*pFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create_To_Binary(m_pDevice, m_pContext, pFile, this, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);

	}
	//여기까지 메쉬


	ReadFile(*pFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	
	if (m_FilePaths != nullptr)
	{
		m_FilePaths->pStruct.resize(m_iNumMaterials);
	}

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESH_MATERIAL		MeshMaterial{};
		
		for (_uint j = 0; j < TEXTURE_TYPE_MAX; ++j)
		{
			_bool	isHaveTextures = true;
			ReadFile(*pFile, &isHaveTextures, sizeof(_bool), &dwByte, nullptr);

			if (!isHaveTextures)
				continue;


			_char				szTexturePath[MAX_PATH] = "";

			ReadFile(*pFile, szTexturePath, MAX_PATH, &dwByte, nullptr);
			
			if (m_FilePaths != nullptr)
			{
				string strTextureFilePath;
				strTextureFilePath.assign(szTexturePath);
				m_FilePaths->pStruct[i].m_ModelFilePaths.push_back(strTextureFilePath);
			}

			_tchar				szFinalPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTexturePath, (_int)(strlen(szTexturePath)), szFinalPath, MAX_PATH);

			if (m_pGameInstance)

				MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath, 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);

	}
	//여기까지 머티리얼


	ReadFile(*pFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	m_KeyFrameIndices.resize(m_iNumAnimations);

	for (_int i = 0; i < (_int)m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create_To_Binary(pFile, m_KeyFrameIndices[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
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
	if (m_FilePaths != nullptr)
	{
		m_FilePaths->pStruct.resize(m_iNumMaterials);
	}
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

			if (m_FilePaths != nullptr)
			{
				string strTextureFilePath;
				strTextureFilePath.assign(szTexturePath);
				m_FilePaths->pStruct[i].m_ModelFilePaths.push_back(strTextureFilePath);
			}
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

void CModel::Culling(_Matrix worldMatrix)
{
	for (auto& iter : m_Meshes)
		iter->Culling(m_pGameInstance, worldMatrix);
}

void CModel::CalculateBoundingBox_Model(CMesh* pMesh, _Vec3& minPos, _Vec3& maxPos)
{
	_Vec3 vMesh_Min_Pos = pMesh->Get_MinPos_Vertex();
	_Vec3 vMesh_Max_Pos = pMesh->Get_MaxPos_Vertex();

	//정점 개수 만큼 반복
	for (_uint i = 0; i < pMesh->Get_NumVertices(); ++i)
	{
		minPos.x = min(minPos.x, vMesh_Min_Pos.x);
		minPos.y = min(minPos.y, vMesh_Min_Pos.y);
		minPos.z = min(minPos.z, vMesh_Min_Pos.z);

		maxPos.x = max(maxPos.x, vMesh_Max_Pos.x);
		maxPos.y = max(maxPos.y, vMesh_Max_Pos.y);
		maxPos.z = max(maxPos.z, vMesh_Max_Pos.z);
	}
}


CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix, _bool isBinaryAnimModel, FilePathStructStack* pStructStack)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix, isBinaryAnimModel, pStructStack)))
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
	{
		Safe_Delete_Array(m_isEnd_Animations);
		Safe_Delete_Array(m_isEnd_Animations_Boundary);

		if (m_isInstance)
		{
			Safe_Release(m_pVBInstance);
			Safe_Delete_Array(m_pInstanceVertices);
		}
	}

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

	if (m_FilePaths != nullptr)
	{
		for (auto& pPathvec : m_FilePaths->pStruct)
		{
			pPathvec.m_ModelFilePaths.clear();
		}
		m_FilePaths->pStruct.clear();
	}
}
