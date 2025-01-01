#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "UIRender.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)



class CController_3DUITool : public CBase
{
	DECLARE_SINGLETON(CController_3DUITool)

public:
	typedef struct
	{
		








	}UI_3D_PART;

	typedef struct
	{
		_int iGuide_NodeIndex = -1;
		_int iNavi_Index = -1;
		_Vec4 vPosition_Origin = { 0.f,0.f,0.f,0.f }; // UI를 고정할 
		_Vec4 vPosition_Adjust = { 0.f,0.f,0.f,0.f }; 
		list<_int> Linked_Nodelist;
		_int iDestNum = -1;
	}UI_3D_GUIDE_PART;


public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	vector<_wstring>& Get_PrototypeTags() { return m_PrototypeTags; }
	_wstring& Get_PrtotypeTagIndex(_uint iIndex) { return m_PrototypeTags[iIndex]; }

	void Add_ProtytypeTag(_wstring strTag) { m_PrototypeTags.emplace_back(strTag); }



public:
	void Edit_Guidline();
	void Render_Guidline();


private:
	void SelectNavi();
	
	void SelectNode();
	void Adjust_Pos();
	void ConnectingNode();

private:
	CController_3DUITool();
	virtual ~CController_3DUITool() = default;

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<_wstring>	m_PrototypeTags;

	vector<UI_3D_GUIDE_PART>	m_vecGuidePart;



public:
	virtual void Free() override;
};

END