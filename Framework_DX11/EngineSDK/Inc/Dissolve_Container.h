#pragma once

#include "Component.h"
#include "VIBuffer_Dissolve_Instance.h"

BEGIN(Engine)
class ENGINE_DLL CDissolve_Container final : public CComponent
{
public:
	typedef struct
	{
		_uint iNumInstance;
		class CVIBuffer_Dissolve_Instance** pInstances;
	}DISSOLVE_CONTAINER_DESC;

public:
	_uint Get_InstanceNum() {
		return (_uint)m_DissolveInstances.size();
	}

private:
	CDissolve_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDissolve_Container(const CDissolve_Container& Prototype);
	virtual ~CDissolve_Container() = default;

public:
	HRESULT Initialize_Prototype(const DISSOLVE_CONTAINER_DESC& Desc);
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Bind_Buffer(class CShader_NonVTX* pShader, const _char* pConstantName, _uint iIndex);
	virtual HRESULT Render(_uint iIndex);

public:
	_bool DispatchCS(class CShader_Compute* pComputeShader, class CTexture* pTexture, class CModel* pModel, 
		const CVIBuffer_Dissolve_Instance::PARTICLE_MOVEMENT& MovementData, const CVIBuffer_Dissolve_Instance::DISSOLVE_DATA& DissolveData);

private:
	vector<class CVIBuffer_Dissolve_Instance*> m_DissolveInstances;

public:
	static CDissolve_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DISSOLVE_CONTAINER_DESC& Desc);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
