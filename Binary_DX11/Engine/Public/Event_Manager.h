#pragma once
#include "Base.h"

BEGIN(Engine)

class CEvent_Manager final : public CBase
{
public:
	enum EVENT_TYPE { DELETE_OBJECT, CREATE_OBJECT, CHANGE_LEVEL, CHANGE_STATE };

	// 오브젝트를 클론할 때 받아오는 구조체 정보임
	typedef struct
	{
		_uint iLevelIndex;
		_wstring strLayerTag;
		_wstring strPrototypeTag;
		void* pArg = nullptr;

	}CLONE_DESC;

	// 이벤트 매니저에서 어떤 이벤트가 호출됐는지를 받아오는 구조체 정보임
	// CLONE_DESC까지 포함해서 나중에 사용할거임
	typedef struct : public CLONE_DESC
	{
		EVENT_TYPE				eEven;
		DWORD_PTR				lParam;
		DWORD_PTR				wParam;

		CLONE_DESC* CloneDesc = { nullptr };
	}EVENT;


private:
	CEvent_Manager(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr, class CUI_Manager* pUIMgr);
	virtual ~CEvent_Manager() = default;

public:
	void AddEvent(EVENT tEvent) { m_Eventlist.push_back(tEvent); }	// 이벤트를 추가함

public:
	HRESULT Initialize();
	void Update();

private:
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CUI_Manager* m_pUI_Manager = { nullptr };

	// 어떤 이벤트들이 호출되었는지 저장하는 리스트
	list<EVENT>					m_Eventlist;

	// 삭제 이벤트들만을 담아두는 리스트
	list<class CGameObject*>	m_Deadlist;

private:
	void Excute(const EVENT tEvent);


public:
	static CEvent_Manager* Create(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr, class CUI_Manager* pUIMgr);
	virtual void Free() override;
};

END


