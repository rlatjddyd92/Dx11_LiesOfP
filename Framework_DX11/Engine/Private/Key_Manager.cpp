#include"Key_Manager.h"

int g_arrKey[(_uint)KEY::LAST] =
{
	VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,

	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',

	VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9,
	'1', '2', '3', '4', '5', '6', '7', '8', '9',

	VK_MENU, VK_CONTROL, VK_LSHIFT, VK_SPACE, VK_RETURN, VK_ESCAPE,
	VK_LBUTTON, VK_RBUTTON, VK_TAB,
};

CKey_Manager::CKey_Manager()
{


}

HRESULT CKey_Manager::Initialize(HWND hWnd)
{
	for (int i = 0; i < (int)KEY::LAST; i++)
	{
		m_vecKey.push_back(KEYINFO{ KEY_STATE::NONE,false });
	}

	m_hWnd = hWnd;

	return S_OK;
}


void CKey_Manager::Update()
{
	// 윈도우 포커싱 알아내기
	//HWND hMainWnd = CCore::GetInst()->GetMainHwnd();
	HWND hWnd = GetFocus();

	// 윈도우 포커싱 중일때
	if (hWnd != nullptr)
	{
		for (size_t i = 0; i < (int)KEY::LAST; i++)
		{
			if (GetAsyncKeyState(g_arrKey[i]) & 0x8000)   // 해당 키가 눌렸다면?
			{
				if (m_vecKey[i].bPrev) // 이전에도 눌림
				{
					m_vecKey[i].eState = KEY_STATE::HOLD;
				}
				else // 이전엔 안눌렸다
				{
					m_vecKey[i].eState = KEY_STATE::TAP;
				}
				m_vecKey[i].bPrev = true;
			}

			else // 해당키가 안눌림
			{
				if (m_vecKey[i].bPrev) // 이전엔 눌림
				{
					m_vecKey[i].eState = KEY_STATE::AWAY;
				}
				else // 이전에도 안눌렸다
				{
					m_vecKey[i].eState = KEY_STATE::NONE;
				}
				m_vecKey[i].bPrev = false;
			}
		}
	}

	else // 포커싱 중 아닐때
	{
		for (size_t i = 0; i < (_uint)KEY::LAST; i++)
		{
			m_vecKey[i].bPrev = false;


			if (m_vecKey[i].eState == KEY_STATE::TAP || m_vecKey[i].eState == KEY_STATE::HOLD)
			{
				m_vecKey[i].eState = KEY_STATE::AWAY;
			}

			else if (m_vecKey[i].eState == KEY_STATE::AWAY)
			{
				m_vecKey[i].eState = KEY_STATE::NONE;
			}

		}
	}
}

CKey_Manager* CKey_Manager::Create(HWND hWnd)
{
	CKey_Manager* pInstance = new CKey_Manager();

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CKey_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CKey_Manager::Free()
{
	__super::Free();
}
