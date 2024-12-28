#include "..\Public\Mesh.h"

#include "Model.h"
#include "Shader.h"
#include "Octree.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh & Prototype)
	: CVIBuffer{ Prototype }
	, m_pOctree{ Prototype.m_pOctree }
{
	Safe_AddRef(m_pOctree);
}

HRESULT CMesh::Initialize_Prototype(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix, const CModel::DISSOLVE_PARTICLE_DESC& ParticleDesc, _uint iMeshIndex)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	m_iNumVertexBuffers = 1;
	ReadFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = pModel->Get_ModelType() == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pFile, PreTransformMatrix) : Ready_VertexBuffer_Anim(pFile, pModel);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 동적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	vector<CVIBuffer_Dissolve_Instance::DISSOLVE_PARTICLE> DissolveParticles;

	for (size_t i = 0; i < m_iNumFaces; i++)
	{
		_uint iStartNum = iNumIndices;

		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);

		if(ParticleDesc.iNumInstance <= 0)
			continue;

		_Vec3 vPoint[3] = {};
		_Vec2 vUV[3] = {};

		if (CModel::TYPE_NONANIM == pModel->Get_ModelType())
		{
			vPoint[0] = m_pVertices[m_pIndices[iStartNum]].vPosition;
			vPoint[1] = m_pVertices[m_pIndices[iStartNum + 1]].vPosition;
			vPoint[2] = m_pVertices[m_pIndices[iStartNum + 2]].vPosition;

			vUV[0] = m_pVertices[m_pIndices[iStartNum]].vTexcoord;
			vUV[1] = m_pVertices[m_pIndices[iStartNum + 1]].vTexcoord;
			vUV[2] = m_pVertices[m_pIndices[iStartNum + 2]].vTexcoord;
		}
		else
		{
			vPoint[0] = m_pAnimVertices[m_pIndices[iStartNum]].vPosition;
			vPoint[1] = m_pAnimVertices[m_pIndices[iStartNum + 1]].vPosition;
			vPoint[2] = m_pAnimVertices[m_pIndices[iStartNum + 2]].vPosition;

			vUV[0] = m_pAnimVertices[m_pIndices[iStartNum]].vTexcoord;
			vUV[1] = m_pAnimVertices[m_pIndices[iStartNum + 1]].vTexcoord;
			vUV[2] = m_pAnimVertices[m_pIndices[iStartNum + 2]].vTexcoord;
		}

		_float fSize = Get_TriangleArea(vPoint[0], vPoint[1], vPoint[2]);
		fSize *= ParticleDesc.iNumInstance;

		if (fSize < 1.f)
			fSize = 1.f;

		for (size_t j = 0; j < (size_t)fSize; ++j)
		{
			CVIBuffer_Dissolve_Instance::DISSOLVE_PARTICLE pParticle = {};
			_Vec3 vResultPos = Get_RandomFacePos(vPoint[0], vPoint[1], vPoint[2]);
			pParticle.Particle.vTranslation = _Vec4(vResultPos.x, vResultPos.y, vResultPos.z, 0.f);

			pParticle.Particle.vPosition = _float3(0.f, 0.f, 0.f);

			_float fScale = m_pGameInstance->Get_Random(ParticleDesc.vSize.x, ParticleDesc.vSize.y);
			pParticle.Particle.vPSize = _float2(fScale, fScale);

			pParticle.Particle.vRight = _float4(1.f, 0.f, 0.f, 0.f);
			pParticle.Particle.vUp = _float4(0.f, 1.f, 0.f, 0.f);
			pParticle.Particle.vLook = _float4(0.f, 0.f, 1.f, 0.f);

			pParticle.Particle.vLifeTime = _float2(m_pGameInstance->Get_Random(ParticleDesc.vLifeTime.x, ParticleDesc.vLifeTime.y), 0.0f);
			pParticle.Particle.vColor = _float4(m_pGameInstance->Get_Random(ParticleDesc.vMinColor.x, ParticleDesc.vMaxColor.x),
				m_pGameInstance->Get_Random(ParticleDesc.vMinColor.y, ParticleDesc.vMaxColor.y),
				m_pGameInstance->Get_Random(ParticleDesc.vMinColor.z, ParticleDesc.vMaxColor.z),
				m_pGameInstance->Get_Random(ParticleDesc.vMinColor.w, ParticleDesc.vMaxColor.w));

			pParticle.Particle.fSpeed = m_pGameInstance->Get_Random(ParticleDesc.vSpeed.x, ParticleDesc.vSpeed.y);

			pParticle.Particle.vCurrenrRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
			pParticle.Particle.vNextRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
			
			pParticle.vTexcoord = Get_CalculateUV(vPoint[0], vPoint[1], vPoint[2], vUV[0], vUV[1], vUV[2], vResultPos);
			pParticle.isActive = false;

			DissolveParticles.emplace_back(pParticle);
		}
	}

	// 파티클 초기화 -완-
	// 이제 이걸 Buffer에 전달해서, 그걸로 버퍼 잘 만들어지는지 확인해야 함.
	if (0 < ParticleDesc.iNumInstance)
	{
		CVIBuffer_Dissolve_Instance::DISSOLVE_INSTANCE_DESC DissolveDesc = {};
		DissolveDesc.iNumInstance = DissolveParticles.size();
		DissolveDesc.pParticles = DissolveParticles.data();
		// 레벨, 테그, 
		if (FAILED(m_pGameInstance->Add_Prototype(0, TEXT("Test"), CVIBuffer_Dissolve_Instance::Create(m_pDevice, m_pContext, DissolveDesc))))
			return E_FAIL;
	}


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	//Safe_Delete_Array(m_pAnimVertices);
	//Safe_Delete_Array(m_pVertices);

	//Safe_Delete_Array(m_pIndices);

#pragma endregion


#pragma region Octree
	//옥트리 생성
	//_int iDepth = -1;
	//vector<int> tmp = {};

	//if (m_pVertices != nullptr)
	//	m_pOctree = COctree::Create(m_vMinPos, m_vMaxPos, m_pIndices, m_pVertices, m_iNumFaces, tmp, &iDepth);
#pragma endregion
	
	return S_OK;
}

HRESULT CMesh::Initialize_Prototype_To_Binary(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix, const CModel::DISSOLVE_PARTICLE_DESC& ParticleDesc, _uint iMeshIndex)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	m_iNumVertexBuffers = 1;
	ReadFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	Ready_VertexBuffer_To_Binary(pFile);

#pragma endregion



#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	vector<CVIBuffer_Dissolve_Instance::DISSOLVE_PARTICLE> DissolveParticles;

	_uint iCount = 0;
	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		ReadFile(*pFile, &m_pIndices[i], sizeof(_uint), &dwByte, nullptr);

		if (ParticleDesc.iNumInstance <= 0)
			continue;

		++iCount;
		_Vec3 vPoint[3] = {};
		_Vec2 vUV[3] = {};

		if(3 == iCount)
		{
			_uint iStartNum = i - 2;
			if (CModel::TYPE_NONANIM == pModel->Get_ModelType())
			{
				vPoint[0] = m_pVertices[m_pIndices[iStartNum]].vPosition;
				vPoint[1] = m_pVertices[m_pIndices[iStartNum + 1]].vPosition;
				vPoint[2] = m_pVertices[m_pIndices[iStartNum + 2]].vPosition;

				vUV[0] = m_pVertices[m_pIndices[iStartNum]].vTexcoord;
				vUV[1] = m_pVertices[m_pIndices[iStartNum + 1]].vTexcoord;
				vUV[2] = m_pVertices[m_pIndices[iStartNum + 2]].vTexcoord;
			}
			else
			{
				vPoint[0] = m_pAnimVertices[m_pIndices[iStartNum]].vPosition;
				vPoint[1] = m_pAnimVertices[m_pIndices[iStartNum + 1]].vPosition;
				vPoint[2] = m_pAnimVertices[m_pIndices[iStartNum + 2]].vPosition;

				vUV[0] = m_pAnimVertices[m_pIndices[iStartNum]].vTexcoord;
				vUV[1] = m_pAnimVertices[m_pIndices[iStartNum + 1]].vTexcoord;
				vUV[2] = m_pAnimVertices[m_pIndices[iStartNum + 2]].vTexcoord;
			}

			_float fSize = Get_TriangleArea(vPoint[0], vPoint[1], vPoint[2]);
			fSize *= ParticleDesc.iNumInstance;

			if (fSize < 1.f)
				fSize = 1.f;

			for (size_t j = 0; j < (size_t)fSize; ++j)
			{
				CVIBuffer_Dissolve_Instance::DISSOLVE_PARTICLE pParticle = {};
				_Vec3 vResultPos = Get_RandomFacePos(vPoint[0], vPoint[1], vPoint[2]);
				pParticle.Particle.vTranslation = _Vec4(vResultPos.x, vResultPos.y, vResultPos.z, 0.f);

				pParticle.Particle.vPosition = _float3(0.f, 0.f, 0.f);

				_float fScale = m_pGameInstance->Get_Random(ParticleDesc.vSize.x, ParticleDesc.vSize.y);
				pParticle.Particle.vPSize = _float2(fScale, fScale);

				pParticle.Particle.vRight = _float4(1.f, 0.f, 0.f, 0.f);
				pParticle.Particle.vUp = _float4(0.f, 1.f, 0.f, 0.f);
				pParticle.Particle.vLook = _float4(0.f, 0.f, 1.f, 0.f);

				pParticle.Particle.vLifeTime = _float2(m_pGameInstance->Get_Random(ParticleDesc.vLifeTime.x, ParticleDesc.vLifeTime.y), 0.0f);
				pParticle.Particle.vColor = _float4(m_pGameInstance->Get_Random(ParticleDesc.vMinColor.x, ParticleDesc.vMaxColor.x),
					m_pGameInstance->Get_Random(ParticleDesc.vMinColor.y, ParticleDesc.vMaxColor.y),
					m_pGameInstance->Get_Random(ParticleDesc.vMinColor.z, ParticleDesc.vMaxColor.z),
					m_pGameInstance->Get_Random(ParticleDesc.vMinColor.w, ParticleDesc.vMaxColor.w));

				pParticle.Particle.fSpeed = m_pGameInstance->Get_Random(ParticleDesc.vSpeed.x, ParticleDesc.vSpeed.y);

				pParticle.Particle.vCurrenrRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
				pParticle.Particle.vNextRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);

				pParticle.vTexcoord = Get_CalculateUV(vPoint[0], vPoint[1], vPoint[2], vUV[0], vUV[1], vUV[2], vResultPos);
				pParticle.isActive = false;

				DissolveParticles.emplace_back(pParticle);
			}
			iCount = 0;
		}
	}

	if (0 < ParticleDesc.iNumInstance)
	{
		CVIBuffer_Dissolve_Instance::DISSOLVE_INSTANCE_DESC DissolveDesc = {};
		DissolveDesc.iNumInstance = DissolveParticles.size();
		DissolveDesc.pParticles = DissolveParticles.data();
		// 레벨, 테그, 
		_wstring strBufferTag = ParticleDesc.strBufferTag;
		strBufferTag += to_wstring(iMeshIndex);
		if (FAILED(m_pGameInstance->Add_Prototype(ParticleDesc.iLevelID, strBufferTag, CVIBuffer_Dissolve_Instance::Create(m_pDevice, m_pContext, DissolveDesc))))
			return E_FAIL;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	//Safe_Delete_Array(m_pAnimVertices);
	//Safe_Delete_Array(m_pVertices);

	//Safe_Delete_Array(m_pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const CModel* pModel, CShader * pShader, const _char * pConstantName)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{	
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * pModel->Get_BoneCombindTransformationMatrix(m_BoneIndices[i]));
	}	

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);	
}

_matrix CMesh::CalcMatrix_forVtxAnim(vector<class CBone*>& Bones, VTXANIMMESH VtxStruct)
{
	_float fWeightW = 1 - (VtxStruct.vBlendWeights.x + VtxStruct.vBlendWeights.y + VtxStruct.vBlendWeights.z);

	_matrix BoneMat =
		VtxStruct.vBlendWeights.x * (m_BoneMatrices[VtxStruct.vBlendIndices.x]) +
		VtxStruct.vBlendWeights.y * (m_BoneMatrices[VtxStruct.vBlendIndices.y]) +
		VtxStruct.vBlendWeights.z * (m_BoneMatrices[VtxStruct.vBlendIndices.z]) +
		fWeightW * (m_BoneMatrices[VtxStruct.vBlendIndices.w]);

	return BoneMat;
}

HRESULT CMesh::Create_BinaryFile(HANDLE* pFile)
{
	_ulong dwByte = 0;
	//이름 저장
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	//머티리얼 인덱스 저장
	WriteFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

	//버텍스 갯수 저장
	WriteFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

	//인덱스 갯수 저장
	WriteFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);

	//면 수 저장
	WriteFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);

	//정점 버퍼 저장
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		WriteFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
	}

	//영향받는 뼈 갯수 저장
	WriteFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	for (size_t j = 0; j < m_iNumBones; ++j)
	{//오프셋 매트릭스 저장
		WriteFile(*pFile, &m_OffsetMatrices[j], sizeof(_float4x4), &dwByte, nullptr);
		
		string strBoneName;
		_char	BoneName[MAX_PATH] = {};
		
		strcpy_s(BoneName, m_BoneNaems[j].c_str());

		WriteFile(*pFile, &BoneName, MAX_PATH, &dwByte, nullptr);

		WriteFile(*pFile, &m_BoneIndices[j], sizeof(_int), &dwByte, nullptr);

	}

	for (_uint k = 0; k < m_iNumIndices; ++k)
	{//인덱스 버퍼 저장
		WriteFile(*pFile, &m_pIndices[k], sizeof(_uint), &dwByte, nullptr);
	}
	

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(HANDLE* pFile, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _Vec3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_Vec3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ReadFile(*pFile, &m_pVertices[i], sizeof(VTXMESH), &dwByte, nullptr);

		XMStoreFloat3(&m_pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vPosition), PreTransformMatrix));

		m_pVertexPositions[i] = m_pVertices[i].vPosition;

		//모델 사이즈 계산
		CalculateBoundingBox_Mesh( m_pVertices[i].vPosition);
	}


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//Select_InitPoint();

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _Vec3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_Vec3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
		m_pVertexPositions[i] = m_pAnimVertices[i].vPosition;
	}

	ReadFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		ReadFile(*pFile, &m_OffsetMatrices[i], sizeof(_float4x4), &dwByte, nullptr);

		_uint iNumWeights = { 0 };
		ReadFile(*pFile, &iNumWeights, sizeof(_uint), &dwByte, nullptr);

		/* 내 모델이 들고 있는 전체 뼈들중에 이 메시가 사용하는 뼈와이름이 같은 뼈의 인덱스를 저장해둔다. */
		_char szBoneName[MAX_PATH];
		ReadFile(*pFile, &szBoneName, MAX_PATH, &dwByte, nullptr);

		string strBoneName = szBoneName;
		m_BoneNaems.emplace_back(strBoneName);
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(szBoneName));

		/* 이 뼈는 몇개(mNumWeights) 정점에게 영향을 주는가? */
		for (size_t j = 0; j < iNumWeights; j++)
		{
			_uint iWeightIndex = { 0 };
			ReadFile(*pFile, &iWeightIndex, sizeof(_uint), &dwByte, nullptr);

			ReadFile(*pFile, &m_pAnimVertices[iWeightIndex].vBlendIndices, sizeof(XMUINT4), &dwByte, nullptr);
			ReadFile(*pFile, &m_pAnimVertices[iWeightIndex].vBlendWeights, sizeof(XMFLOAT4), &dwByte, nullptr);

		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		// 뼈가 없는 상황에서는 메쉬의 이름과 같은 뼈의 이름을 찾아 강제로 1로 늘려 넣어줌
		// Weight가 0이어도 fWeightW로 살려두었기에 행렬 곱에서는 상관없음
		// OffsetMatrix가 문제임 -> 항등으로 저장해버리기
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));

		_float4x4	OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pAnimVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_To_Binary(HANDLE* pFile)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _Vec3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_Vec3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
		m_pVertexPositions[i] = m_pAnimVertices[i].vPosition;
	}

	ReadFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	
	m_OffsetMatrices.reserve(m_iNumBones);
	m_BoneNaems.reserve(m_iNumBones);
	m_BoneIndices.reserve(m_iNumBones);

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_float4x4 OffsetMat{};
		ReadFile(*pFile, &OffsetMat, sizeof(_float4x4), &dwByte, nullptr);
		m_OffsetMatrices.emplace_back(OffsetMat);

		_char szBoneName[MAX_PATH];
		ReadFile(*pFile, &szBoneName, MAX_PATH, &dwByte, nullptr);

		string strBoneName = szBoneName;
		m_BoneNaems.emplace_back(strBoneName);

		_int iIndexBone = {};
		ReadFile(*pFile, &iIndexBone, sizeof(_int), &dwByte, nullptr);
		m_BoneIndices.emplace_back(iIndexBone);

	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pAnimVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

void CMesh::Culling(CGameInstance* pGameInstance, _Matrix worldMatrix)
{
	if (m_pOctree != nullptr)
		if (m_pOctree->Get_haveChild() == false)
			return;

	pGameInstance->Transform_ToLocalSpace(worldMatrix);
	_uint			iNumIndices = { 0 };
	unordered_set<_int> pDrawTriIndexes = {};

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	if(m_pOctree != nullptr)
		m_pOctree->Culling(pGameInstance, m_pIndices, (_uint*)SubResource.pData, pDrawTriIndexes,  &iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}


void CMesh::CalculateBoundingBox_Mesh(const _Vec3& vVertexPos)
{
	m_vMinPos.x = min(m_vMinPos.x, vVertexPos.x);
	m_vMinPos.y = min(m_vMinPos.y, vVertexPos.y);
	m_vMinPos.z = min(m_vMinPos.z, vVertexPos.z);

	m_vMaxPos.x = max(m_vMaxPos.x, vVertexPos.x);
	m_vMaxPos.y = max(m_vMaxPos.y, vVertexPos.y);
	m_vMaxPos.z = max(m_vMaxPos.z, vVertexPos.z);
}

_Vec3 CMesh::Get_RandomFacePos(_Vec3 vFirst, _Vec3 vSecond, _Vec3 vThird)
{
	_float fBeta = m_pGameInstance->Get_Random_Normal();
	_float fGamma = m_pGameInstance->Get_Random_Normal() * (1.f - fBeta);
	_float fAlpha = 1.f - fBeta - fGamma;

	_Vec3 vResult = _Vec3(
		vFirst.x * fAlpha + vSecond.x * fBeta + vThird.x * fGamma,
		vFirst.y * fAlpha + vSecond.y * fBeta + vThird.y * fGamma,
		vFirst.z * fAlpha + vSecond.z * fBeta + vThird.z * fGamma
	);

	return vResult;
}

_Vec2 CMesh::Get_CalculateUV(const _Vec3& A, const _Vec3& B, const _Vec3& C, const _Vec2& UV_A, const _Vec2& UV_B, const _Vec2& UV_C, const _Vec3& D)
{
	// 벡터 정의
	_vector v0 = XMVectorSet(B.x - A.x, B.y - A.y, B.z - A.z, 0.0f);
	_vector v1 = XMVectorSet(C.x - A.x, C.y - A.y, C.z - A.z, 0.0f);
	_vector v2 = XMVectorSet(D.x - A.x, D.y - A.y, D.z - A.z, 0.0f);

	// 내적 계산
	_float d00 = XMVectorGetX(XMVector3Dot(v0, v0));
	_float d01 = XMVectorGetX(XMVector3Dot(v0, v1));
	_float d11 = XMVectorGetX(XMVector3Dot(v1, v1));
	_float d20 = XMVectorGetX(XMVector3Dot(v2, v0));
	_float d21 = XMVectorGetX(XMVector3Dot(v2, v1));

	// 중심 좌표계 가중치 계산
	_float denom = d00 * d11 - d01 * d01;
	_float lambda2 = (d11 * d20 - d01 * d21) / denom;
	_float lambda3 = (d00 * d21 - d01 * d20) / denom;
	_float lambda1 = 1.0f - lambda2 - lambda3;

	// UV 좌표 보간
	_Vec2 UV_D = {};
	UV_D.x = lambda1 * UV_A.x + lambda2 * UV_B.x + lambda3 * UV_C.x;
	UV_D.y = lambda1 * UV_A.y + lambda2 * UV_B.y + lambda3 * UV_C.y;

	return UV_D;
}

_float CMesh::Get_TriangleArea(const _Vec3& A, const _Vec3& B, const _Vec3& C)
{
	// 벡터 v0 = B - A, v1 = C - A
	_vector v0 = XMVectorSet(B.x - A.x, B.y - A.y, B.z - A.z, 0.0f);
	_vector v1 = XMVectorSet(C.x - A.x, C.y - A.y, C.z - A.z, 0.0f);

	// 외적 계산
	_vector cross = XMVector3Cross(v0, v1);

	// 외적 크기 계산
	_float magnitude = XMVectorGetX(XMVector3Length(cross));

	// 삼각형 넓이 = 0.5 * |외적 크기|
	return 0.5f * magnitude;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix, const CModel::DISSOLVE_PARTICLE_DESC& ParticleDesc, _uint iMeshIndex)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFile, pModel, PreTransformMatrix, ParticleDesc, iMeshIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create_To_Binary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix, const CModel::DISSOLVE_PARTICLE_DESC& ParticleDesc, _uint iMeshIndex)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_To_Binary(pFile, pModel, PreTransformMatrix, ParticleDesc, iMeshIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pAnimVertices);
	Safe_Delete_Array(m_pVertices);

	Safe_Delete_Array(m_pIndices);
	Safe_Release(m_pOctree);
	Safe_Delete_Array(m_pWeightsCnts);
}
