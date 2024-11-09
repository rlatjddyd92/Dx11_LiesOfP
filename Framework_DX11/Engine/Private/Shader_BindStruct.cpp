#include "..\Public\Shader_BindStruct.h"

CShader_BindStruct::CShader_BindStruct(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CShader_BindStruct::CShader_BindStruct(const CShader_BindStruct& Prototype)
	: CComponent{ Prototype }
	, m_pEffect{ Prototype.m_pEffect }
	, m_iNumPasses{ Prototype.m_iNumPasses }
{
	Safe_AddRef(m_pEffect);
}

HRESULT CShader_BindStruct::Initialize_Prototype(const _tchar* pShaderFilePath)
{
	_uint			iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pErrorMsg = { nullptr };

	/* 내가 작성한 쉐이더 파일을 빌드하여 객체화(ID3DX11Effect*) 했다. */
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc{};

	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	return S_OK;
}

HRESULT CShader_BindStruct::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader_BindStruct::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectPass* pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	pPass->Apply(0, m_pContext);

	return S_OK;
}

HRESULT CShader_BindStruct::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	/* 쉐이더 안에 선언되어있는 전역변수의 컴객체 핸들을 얻어온다. */
	/* 전달해준 이름을 가진 전역변수가 있다면 유효한 포인터를 얻어온다.(타입에 대한 고려x) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	///* 특정 변수들에 대해서는 허용되지 않거나 변환과정을 스스로 수행한다.  */
	//pVariable->SetRawValue(void* , 0, 넘기고자하는 데이터의 크기.);	

	/* 얻어온 전역변수가 행렬이었다면 행렬 타입에 대한 핸들을 다시 얻어온다. */
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader_BindStruct::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
	/* 쉐이더 안에 선언되어있는 전역변수의 컴객체 핸들을 얻어온다. */
	/* 전달해준 이름을 가진 전역변수가 있다면 유효한 포인터를 얻어온다.(타입에 대한 고려x) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	///* 특정 변수들에 대해서는 허용되지 않거나 변환과정을 스스로 수행한다.  */
	//pVariable->SetRawValue(void* , 0, 넘기고자하는 데이터의 크기.);	

	/* 얻어온 전역변수가 행렬이었다면 행렬 타입에 대한 핸들을 다시 얻어온다. */
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

HRESULT CShader_BindStruct::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	/* 쉐이더 안에 선언되어있는 전역변수의 컴객체 핸들을 얻어온다. */
	/* 전달해준 이름을 가진 전역변수가 있다면 유효한 포인터를 얻어온다.(타입에 대한 고려x) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader_BindStruct::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumSRVs)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResourceArray(ppSRVs, 0, iNumSRVs);
}

HRESULT CShader_BindStruct::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader_BindStruct::Bind_CBuffer(const _char* pConstantName, ID3D11Buffer* pBuffer)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	// Constant Buffer를 바인딩합니다.
	ID3DX11EffectConstantBuffer* pConstantBuffer = pVariable->AsConstantBuffer();
	if (nullptr == pConstantBuffer)
		return E_FAIL;

	pConstantBuffer->SetConstantBuffer(pBuffer);
	return S_OK;
}

CShader_BindStruct* CShader_BindStruct::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath)
{
	CShader_BindStruct* pInstance = new CShader_BindStruct(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CShader_BindStruct"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CShader_BindStruct::Clone(void* pArg)
{
	CShader_BindStruct* pInstance = new CShader_BindStruct(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShader_BindStruct"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CShader_BindStruct::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
