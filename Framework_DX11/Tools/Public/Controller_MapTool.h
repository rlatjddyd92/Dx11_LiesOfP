#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_MapTool : public CBase
{
	DECLARE_SINGLETON(CController_MapTool)

private:
	CController_MapTool();
	virtual ~CController_MapTool() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Control_Player();
	void Create_Map();
	void Pick_Object();
	void PickPos();
	void Save_Load();
private:
	void Select_Map_Model();
	void Show_List(_uint iFolder);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CNavigationController* m_pNavigationController = { nullptr };

	vector<const char*>	m_FileNames;	//ListBox에 띄울 항목들 이름

	_int m_iListCount = { 0 };			//ListBox항목 개수
	_int m_iListSelectNum = { 0 };		//ListBox항목중 선택한 인덱스

	_uint m_iPickObject_ID = { 0 };
	_uint m_iPre_Picked_ID={ 0 };

	class CGameObject* m_pSelectObject = { nullptr };		//새로 선택한 물체
	class CGameObject* m_pPreSelectObject = { nullptr };	//전에 선택했던 물체

	static _float3 m_vScale;	//오브젝트 크기
	static _float3 m_vPosition; //오브젝트 좌표
	static _float3 m_vRotation; //오브젝트 좌표

	_float3 m_vPickPos = {}; //피킹한 좌표

private:
	void SaveMap();
	void LoadMap();
	void Find_PickObject();
	void Map_Menu();
	void Nav_Menu();

public:
	virtual void Free() override;
};

END

