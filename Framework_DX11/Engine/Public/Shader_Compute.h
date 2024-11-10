#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader_Compute final : public CComponent
{
private:
	CShader_Compute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader_Compute(const CShader_Compute& Prototype);
	virtual ~CShader_Compute() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const _char* pMainTag);
	virtual HRESULT Initialize(void* pArg) override;

	void Bind_UAV(ID3D11UnorderedAccessView* pUAV);
	void Bind_CBuffer(ID3D11Buffer* pConstantBuffer);	// 상수 버퍼 만들 때 패딩 해줘야 함!!!!!!!
	void Execute_ComputeShader(_uint iThreadGroupX, _uint iThreadGroupY, _uint iThreadGroupZ);

private:
	ID3D11ComputeShader* m_pComputeShader = { nullptr };

public:
	static CShader_Compute* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pMainTag);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END