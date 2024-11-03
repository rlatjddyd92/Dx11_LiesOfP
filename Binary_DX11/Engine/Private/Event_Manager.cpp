#include "../Public/Event_Manager.h"
#include "../Public/GameObject.h"

#include "Level_Manager.h"
#include "Object_Manager.h"
#include "UI_Manager.h"

#include "Level.h"
#include "UIObject.h"

CEvent_Manager::CEvent_Manager(CLevel_Manager* pLevelMgr, CObject_Manager* pObjectMgr, CUI_Manager* pUIMgr)
	: m_pLevel_Manager(pLevelMgr)
	, m_pObject_Manager(pObjectMgr)
	, m_pUI_Manager(pUIMgr)
{
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

// 일어난 이벤트들을 전부 담아두기 위함
// 렌더까지 끝난 후 가장 마지막 프레임에 실행됨
void CEvent_Manager::Update()
{
	// 삭제 이벤트는 한 프레임이 밀림
	if (m_Deadlist.size() > 0)
	{
		for (auto& pDeadObject : m_Deadlist)
			Safe_Release(pDeadObject);
		m_Deadlist.clear();
	}

	// 실제 일어난 이벤트들을 처리함
	for (auto& Event = m_Eventlist.begin(); Event != m_Eventlist.end();)
	{

		Excute(*Event);

		if (nullptr != Event->CloneDesc)
		{
			Safe_Delete(Event->CloneDesc->pArg);
			Safe_Delete(Event->CloneDesc);
		}

		Event = m_Eventlist.erase(Event);
	}
	m_Eventlist.clear();



	/* 이런 순환인거임

	1. 업데이트 도중에 어디선가 Add_Event로 이벤트가 추가됨
	2. 삭제되는 오브젝트들 리스트를 먼저 검사하고 실제로 삭제함 -> 한 프레임 밀릴거임
	3. 그 뒤에 생성, 레벨 전환 등의 이벤트를  Excute로 실행함
	4. 하지만 삭제 이벤트는 Excute에서 실행하는게 아니라 삭제 리스트에 넣어주는 작업만을 진행함
	5. 다음 프레임 Update에서 삭제 리스트에 있는 것을 실질적으로 지워줌

	*/
}


// 리스트에 담아둔 이벤트들을 실제로 실행하는 함수
void CEvent_Manager::Excute(const EVENT tEvent)
{
	switch ((UINT)tEvent.eEven)
	{
	case (UINT)EVENT_TYPE::CREATE_OBJECT:
	{
		// lParam : 안씀
		// wParam : 안씀
		// pArg   : Clone_Desc 정보 -> 우리가 clone할 정보
		const CLONE_DESC* CloneDesc = tEvent.CloneDesc;

		m_pObject_Manager->Add_CloneObject_ToLayer(CloneDesc->iLevelIndex, CloneDesc->strLayerTag, CloneDesc->strPrototypeTag, CloneDesc->pArg);
	}
	break;

	case (UINT)EVENT_TYPE::DELETE_OBJECT:
	{
		// lParam : Object adress		-> 삭제할 오브젝트의 주소
		// wParam: Group Type			-> 오브젝트가 어떤 그룹인지(근데 안 씀)

		CGameObject* pDeadObj = (CGameObject*)tEvent.lParam;

		// 혹시 이중으로 들어가는거 방지
		if (!pDeadObj->Get_Dead())
		{		
			// Object를 Dead상태로 변경
			// 여기서 한 프레임 밀리니까 Dead에 관한 처리도 가능해짐
			pDeadObj->Set_Dead(true);

			// 삭제예정 오브젝트들을 모아둔다
			m_Deadlist.push_back(pDeadObj);
		}

		// 없어지면 foucs 해제
		if (nullptr != dynamic_cast<CUIObject*>(pDeadObj))	// UI란 소리고
		{
			m_pUI_Manager->Set_FocusUI(nullptr);
		}

	}
	break;


	case (UINT)EVENT_TYPE::CHANGE_LEVEL:
	{
		// lParam:   Level Index
		// wParam :  Next Scene Type
		m_pLevel_Manager->Change_Level((_uint)tEvent.lParam, (CLevel*)tEvent.wParam);

		//m_pUI_Manager->SetFocusUI(nullptr);
	}
	break;

	case (UINT)EVENT_TYPE::CHANGE_STATE:
	{
		// lParam : AI
		// wParam : Next Type

	}
	break;
	}
}



CEvent_Manager* CEvent_Manager::Create(CLevel_Manager* pLevelMgr, CObject_Manager* pObjectMgr, CUI_Manager* pUIMgr)
{
	CEvent_Manager* pInstance = new CEvent_Manager(pLevelMgr, pObjectMgr, pUIMgr);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CEvent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pUI_Manager);


	for (auto& pDeadObject : m_Deadlist)
		Safe_Release(pDeadObject);
	m_Deadlist.clear();

	m_Eventlist.clear();
}



