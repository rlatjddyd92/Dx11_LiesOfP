#pragma once

#define BEGIN(SpaceName) namespace SpaceName {
#define END }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif


#define MSG_BOX(message)								\
::MessageBox(nullptr, message, L"error", MB_OK)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned int Destroy_Instance();				\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned int  ClassName::Destroy_Instance()				\
{														\
	unsigned int	iRefCnt = { 0 };					\
														\
	if (nullptr != m_pInstance)							\
	{													\
		iRefCnt = m_pInstance->Release();				\
		m_pInstance = nullptr;							\
	}													\
	return iRefCnt;										\
}

#define KEY_HOLD(key) CGameInstance::Get_Instance()->Get_KeyState(key) == KEY_STATE::HOLD
#define KEY_TAP(key) CGameInstance::Get_Instance()->Get_KeyState(key) == KEY_STATE::TAP
#define KEY_AWAY(key) CGameInstance::Get_Instance()->Get_KeyState(key) == KEY_STATE::AWAY




#define PhysX_RELEASE(x)	if(x != nullptr)	{ x->release(); x = nullptr;}