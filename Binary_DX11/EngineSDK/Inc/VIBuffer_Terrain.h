#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct {
		_float	fPositionX = 0.f;
		_float	fPositionY = 0.f;
		_float	fPositionZ = 0.f;
		_uint	iNumVerticesX = 10;
		_uint	iNumVerticesZ = 10;
	}VIBUFFER_TERRAIN_DESC;

private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool isPicking(const _fmatrix& WorldMatrix, _float3* pOut) override;

private:
	_uint			m_iNumVerticesX = {};
	_uint			m_iNumVerticesZ = {};

	_bool			m_bCreate = { false };

	VIBUFFER_TERRAIN_DESC	m_tVIBuffer_Terrain_Desc;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticeX, _uint iNumVerticeZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END