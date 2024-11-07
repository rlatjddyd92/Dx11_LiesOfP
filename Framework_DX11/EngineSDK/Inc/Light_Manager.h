#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer); 

public:
	LIGHT_DESC* Get_LightDesc(_uint iIndex);
	_int Get_Total_Light_Count() { return (_int)m_Lights.size(); }
	void Delete_Light(_int iIndex);
	_int Find_Light_Index(_Vec4 vPos);

private:
	vector<class CLight*>				m_Lights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END