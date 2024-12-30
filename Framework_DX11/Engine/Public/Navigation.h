#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int			iCurrentIndex = {-1};
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	void	Change_CurrentCellIndex(_int iIndex) { m_iCurrentCellIndex = iIndex; }
	_int	Get_CurrentCellIndex() { return m_iCurrentCellIndex; }
	_int	Get_CurrentAreaNum() { return m_iCurrentAreaIndex; }

	void	Set_ExceptCellNum(_int iCellNum) { m_iExceptCellNum = iCellNum; }

public:
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override; 

public:
	void Update(_fmatrix TerrainWorldMatrix);
	_bool isMove(_Vec3 vPosition);
	_float SetUp_OnCell(class CTransform* pTransform, _float fOffset, _float fTimeDelta);
	void Move_to_Cell(class CRigidBody* pRigidBody, _int iIndex);
	void Research_Cell(_Vec3 vNewPos, _uint* iIndex = nullptr);
	_float Get_CellPosY(class CTransform* pTransform, _float fOffset = 0.f);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	vector<class CCell*>				m_Cells;
	_int								m_iCurrentCellIndex = { -1 };
	_int								m_iCurrentAreaIndex = { 0 };
	static _Matrix						m_WorldMatrix;

	_Vec3								m_vOutLine{};		// ¼¿ÀÌ ³ª°£ ¸é

	_int								m_iExceptCellNum = { -1 };

#ifdef _DEBUG
private:
	class CShader*						m_pShader = { nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END