#include "UI_Manager.h"
#include "GameInstance.h"

#include "UIObject.h"

CUI_Manager::CUI_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize()
{
	return S_OK;
}

void CUI_Manager::Update()
{
	// 포커싱 된 UI를 찾는다
	m_pFocusUI = Get_FocusUI();

	if (nullptr == m_pFocusUI)
		return;

	CUIObject* pTargetUI = Get_TargetUI(m_pFocusUI);

	bool bLbtnTap = KEY_TAP(KEY::LBUTTON);
	bool bLbtnAway = KEY_AWAY(KEY::LBUTTON);   // 딱 뗀 시점

	if (pTargetUI)
	{
		if (bLbtnTap)
		{
			pTargetUI->MouseLBtnDown();
			pTargetUI->m_bLbtnDown = true;
		}
		else if (bLbtnAway)
		{
			pTargetUI->MouseLBtnUp();

			if (pTargetUI->m_bLbtnDown)
				pTargetUI->MouseClick();

			pTargetUI->m_bLbtnDown = false;
		}
	}
}


void CUI_Manager::Set_FocusUI(CUIObject* _pUI)
{
	m_pFocusUI = _pUI;
}

// 현재 보고있는? 집중하고있는? UI
CUIObject* CUI_Manager::Get_FocusUI()
{
	//CScene* pCureScene = CSceneMgr::CreateSingleTonInst()->GetCurScene();

	list<CGameObject*>& UIList = m_pGameInstance->Get_RenderList(CRenderer::RG_UI);

	_bool bLbtnTap = KEY_TAP(KEY::LBUTTON);
	_bool bLbtnAway = KEY_AWAY(KEY::LBUTTON);

	_bool bLbtnHold = KEY_HOLD(KEY::LBUTTON);


	if (0 == UIList.size())
	{
		m_pFocusUI = nullptr;
	}

	CUIObject* pFocusUI = m_pFocusUI;

	list<CUIObject*> queue;
	CUIObject* pUI = nullptr;
	CUIObject* pTargetUI = nullptr;

	list<CGameObject*>::iterator targetiter = UIList.end();
	list<CGameObject*>::iterator iter = UIList.begin();

	if (!bLbtnTap)  // 내가 누른적이 없다면 포커싱은 안주되, MouseOn은 호출해줘야함..
	{
		for (; iter != UIList.end(); ++iter)
		{
			pUI = ((CUIObject*)*iter);  // 최상위 부모
			queue.push_back(pUI); // 그 부모를 큐에 넣고

			while (!queue.empty())
			{
				pUI = queue.front();
				queue.pop_front();

				if (pUI->IsMouseOn())
				{
					pTargetUI = pUI;

					if (nullptr != pTargetUI)
						pTargetUI->MouseOn();
				}

				else if (!pUI->IsMouseOn())
				{
					pTargetUI = pUI;

					if (nullptr != pTargetUI)
						pTargetUI->MouseOut();
				}

				// 그 부모 pUI의 자식UI벡터를 vecChild한테 줌
				const list<CUIObject*>& ChildUIList = pUI->Get_ChildUIList();

				// queue에 자식 UI추가
				for (auto& iter : ChildUIList)
				{
					queue.push_back(iter);
				}
			}
		}

		return pFocusUI;
	}

	if (bLbtnTap || bLbtnHold)
	{
		for (iter; iter != UIList.end(); ++iter)
		{
			pUI = ((CUIObject*)(*iter));

			_bool m = pUI->IsMouseOn();

			if (pUI->IsActive() && pUI->IsMouseOn()) // && pUI->GetActivate()
			{
				targetiter = iter;
			}
		}
	}

	if (targetiter == UIList.end())
	{
		return pFocusUI;
	}


	pFocusUI = (CUIObject*)(*targetiter);


	UIList.erase(targetiter);
	UIList.push_back(pFocusUI);

	return pFocusUI;
}

CUIObject* CUI_Manager::Get_TargetUI(CUIObject* _pFocusUI)
{
	CUIObject* pTargetUI = nullptr;

	bool bLbtnAway = KEY_AWAY(KEY::LBUTTON);;   // 딱 뗀 시

	static list<CUIObject*> queue;
	static vector<CUIObject*> vecNoneTarget;


	if (nullptr != _pFocusUI)
		queue.push_back(_pFocusUI);


	while (!queue.empty())			// 안에 들어있는 자식들이 있을때까지 돌고
	{
		CUIObject* pUI = queue.front();
		queue.pop_front();

		if (pUI->IsMouseOn())
		{
			if (nullptr != pTargetUI)
			{
				vecNoneTarget.push_back(pTargetUI);
			}
			pTargetUI = pUI;
		}
		else
		{
			vecNoneTarget.push_back(pUI);
		}


		const list<CUIObject*>& ChildUIList = pUI->Get_ChildUIList();

		for (auto& iter : ChildUIList)
		{
			queue.push_back(iter);
		}
	}

	vecNoneTarget.clear();

	return pTargetUI;
}

CUI_Manager* CUI_Manager::Create()
{
	CUI_Manager* pInstance = new CUI_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
