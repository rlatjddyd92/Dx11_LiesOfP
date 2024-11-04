#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "GameInstance.h"

#include "Cell.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CNavigationController : public CBase
{
	DECLARE_SINGLETON(CNavigationController)

private:
	CNavigationController();
	virtual ~CNavigationController() = default;

public:
	_uint	Get_CellSize() { return (_uint)m_Cells.size(); }	//전체 Cell 개수

	_uint	Get_SelectCellIndex() { return m_iSelectCellIndex; }	//선택한 셀 인덱스 번호
	void	Set_SelectCell(_uint iIndex) { m_iSelectCellIndex = iIndex; }

	_int	SelectCell(_float3 vPickPos);
	void	Delete_Selected(_uint iIndex);

	_float3 Select_Vertex(_fvector vPos);
	void Set_All_Selected_Vertex_to_this(_float3 vChangePos);

	void Set_AreaNum(_uint _iNum) { m_iAreaNum = _iNum; }
	_uint Get_AreaNum(_int _iIndex) { return m_Cells[_iIndex]->Get_AreaNum(); }

	void Change_Cell_Inform(_uint _index, _uint _iType, _uint _iRoomNum);

	_bool isCell_Picked(_float3 vPickPos);
	_int Get_WhereCell(_float3 vPickPos);

	void Delete_All_Cell();
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Render();

public:
	void Add_Point(_Vec3 vPos);
	HRESULT Add_Cell(_float3* vPoints);

	void Sort_Cell(_float3* vPoints);

	CCell* Get_SelectCell();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	class CShader* m_pShader = { nullptr };

	vector<class CCell*>	m_Cells;
	vector<_Vec3>			m_vClickPoints;

	_int					m_iSelectCellIndex = { -1 };

	_float3					m_vClickPos = {};
	_float					m_fClosetDistance = {};

	_float3					m_vSelectVertexPos = {};

	_uint					m_iCellType = { 0 };
	_uint					m_iAreaNum = { 0 };

private:
	_Vec3 Find_ClosetPoint(_Vec3 vPos);

public:
	virtual void Free() override;
};

END

