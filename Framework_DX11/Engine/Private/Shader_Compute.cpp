#include "Shader_Compute.h"

CShader_Compute::CShader_Compute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CShader_Compute::CShader_Compute(const CShader_Compute& Prototype)
	: CComponent(Prototype)
	, m_pComputeShader(Prototype.m_pComputeShader)
{
	Safe_AddRef(m_pComputeShader);
}

HRESULT CShader_Compute::Initialize_Prototype(const _tchar* pShaderFilePath, const _char* pMainTag)
{
	ID3DBlob* pErrorBlob = { nullptr };
	ID3DBlob* pCS_Blob = { nullptr };
	if (GetFileAttributes(pShaderFilePath) == INVALID_FILE_ATTRIBUTES)
		int a = 0;

	// pMainTag : PS_MAIN처럼 어떤 셰이더 쓸지 정해줌.
	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, nullptr, pMainTag, "cs_5_0", D3DCOMPILE_ENABLE_STRICTNESS,
		0, &pCS_Blob, &pErrorBlob)))
	{
		_char* pChar = static_cast<char*>(pErrorBlob->GetBufferPointer());
		string str = pChar;
		Safe_Release(pErrorBlob);
		return E_FAIL;
	}

	m_pDevice->CreateComputeShader(pCS_Blob->GetBufferPointer(), pCS_Blob->GetBufferSize(), nullptr, &m_pComputeShader);
	Safe_Release(pCS_Blob);

	return S_OK;
}

HRESULT CShader_Compute::Initialize(void* pArg)
{
	// 생성 예시만 만들어 놓자.
#pragma region FOR_EXAMPLE
	// - 이미지 처리: 그룹당 16x16 또는 32x32 크기 권장.
	// - 입자 시스템: 그룹당 256 또는 512 쓰레드 크기 권장 (1차원 배열 접근).
	_uint iNumThread = { 10 };

#pragma region Share Buffer
	//// 공유 버퍼 구성
	//D3D11_BUFFER_DESC	BufferDesc = {};
	//BufferDesc.Usage = D3D11_USAGE_DEFAULT;		// GPU에서만 수정할거야
	//BufferDesc.ByteWidth = sizeof(_float) * iNumThread;	// 실수형 10개 만들게.
	//BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;	// UAV, SRV에 바인딩 할 거야.
	//BufferDesc.StructureByteStride = sizeof(_float);	// 개당 4바이트야
	//BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// 고정된 크기의 구조체 배열을 저장할 거야.

	//ID3D11Buffer* pSharedBuffer = nullptr;
	//if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &pSharedBuffer)))
	//	return E_FAIL;
#pragma endregion
	
#pragma region Constant Buffer
	//// Constant Buffer 구성
	//D3D11_BUFFER_DESC	ConstantBufferDesc = {};
	//ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	
	//ConstantBufferDesc.ByteWidth = sizeof(_float2);	// 8바이트짜리야.
	//ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// 상수버퍼에 바인딩 할 거야.
	//ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU에서 수정할 거야. 

	//ID3D11Buffer* pConstantBuffer = { nullptr };
	//if (FAILED(m_pDevice->CreateBuffer(&ConstantBufferDesc, nullptr, &pConstantBuffer)))
	//	return E_FAIL;
#pragma endregion

#pragma region UAV
	//// UAV 구성 : 컴퓨트 셰이더에서 값을 수정할 수 있음.
	//D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	//UAVDesc.Format = DXGI_FORMAT_UNKNOWN;					// 구조화된 데이터나 특정 포맷을 지정할 수 없는 데이터
	//UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;	//UAV가 1차원 버퍼 타입
	//UAVDesc.Buffer.NumElements = iNumThread;

	//// 텍스처일 경우
	//UAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 텍스처 포맷
	//UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;	// 2D 텍스처
	//UAVDesc.Texture2D.MipSlice = 0;							// 사용할 Mipmap 레벨

	//// 텍스처 배열일 경우
	//_uint iArraySize = {};
	//UAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 텍스처 포맷
	//UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;	// 텍스처 배열
	//UAVDesc.Texture2DArray.ArraySize = iArraySize;				// 배열 크기
	//UAVDesc.Texture2D.MipSlice = 0;								// 사용할 Mipmap 레벨


	//ID3D11UnorderedAccessView* pUAV = nullptr;
	//if (FAILED(m_pDevice->CreateUnorderedAccessView(pSharedBuffer, &UAVDesc, &pUAV)))
	//	return E_FAIL;
#pragma endregion

#pragma region SRV
	//// SRV 구성 : 읽기 전용.
	//D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	//SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	//SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	//SRVDesc.Buffer.NumElements = iNumThread;

	//ID3D11ShaderResourceView* pSRV = nullptr;
	//if (FAILED(m_pDevice->CreateShaderResourceView(pSharedBuffer, &SRVDesc, &pSRV)))
	//	return E_FAIL;
#pragma endregion
#pragma endregion

	return S_OK;
}

void CShader_Compute::Bind_UAV(ID3D11UnorderedAccessView* pUAV)
{
	m_pContext->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);
}

void CShader_Compute::Bind_CBuffer(ID3D11Buffer* pConstantBuffer)
{
	m_pContext->CSSetConstantBuffers(0, 1, &pConstantBuffer);
}


void CShader_Compute::Execute_ComputeShader(_uint iThreadGroupX, _uint iThreadGroupY, _uint iThreadGroupZ)
{
	// 셰이더 설정하고
	m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);

	// 실행하고
	m_pContext->Dispatch(iThreadGroupX, iThreadGroupY, iThreadGroupZ);

	// UAV 해제하고
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	// SRV 해제하고
	ID3D11ShaderResourceView* nullSRV = { nullptr };
	m_pContext->CSSetShaderResources(0, 1, &nullSRV);

	// 셰이더까지 해제하면 끝
	m_pContext->CSSetShader(nullptr, nullptr, 0);
}

CShader_Compute* CShader_Compute::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pMainTag)
{
	CShader_Compute* pInstance = new CShader_Compute(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pMainTag)))
	{
		MSG_BOX(TEXT("Create Failed : CShader_Compute"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader_Compute::Clone(void* pArg)
{
	CShader_Compute* pInstance = new CShader_Compute(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CShader_Compute"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader_Compute::Free()
{
	__super::Free();
	Safe_Release(m_pComputeShader);
}
