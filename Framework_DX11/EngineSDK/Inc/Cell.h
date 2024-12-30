#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	typedef struct 
	{
		_int iIndex = 0;
		_uint iAreaNum = 0;
		_uint iCellTypeNum = 0;
	} CELL_DESC;

public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;
	
public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

	void Set_AreaNum(_uint _iNum) { m_iAreaNum = _iNum; }
	_uint Get_AreaNum() { return m_iAreaNum; }

	void Set_CellTypeNum(_uint _iNum) { m_iCellTypeNum = _iNum; }
	_uint Get_CellTypeNum() { return m_iCellTypeNum; }

	void Set_Point(POINT ePoint, _float3 _pos) {	m_vPoints[ePoint] = _pos;}

	void  CompareAndChange(_float3 _comparePos, _float3 _changePos);

	_float3* Get_All_Points() { return m_vPoints; }

public:
	HRESULT Initialize(const _float3* pPoints, void* pArg);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex, _Vec3* vOutLine = nullptr);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	_int							m_iIndex = {};
	_float3							m_vPoints[POINT_END] = {};
	_int							m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	_uint							m_iAreaNum = { 0 };
	_uint							m_iCellTypeNum = { 0 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, void* pArg);
	virtual void Free() override;
};

END