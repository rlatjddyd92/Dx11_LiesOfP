#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Interface_Enums.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

/*

*/

class CPlayer_Stat_Manager : public CBase
{
public:
	typedef struct BUFF_INFO
	{
		_wstring strBuff_Name = TEXT("none");
		_int iTexture_Index = 0;
		_Vec4 vTexture_Color = { 0.f,0.f,0.f,0.f };
		_float fLifeTime_Now = 0.f;
		_float fLifeTime_Max = 0.f;
	}BUFF;

private:
	CPlayer_Stat_Manager(CGameInstance* pGameInstance);
	virtual ~CPlayer_Stat_Manager() = default;

public:

	const BUFF* Get_Buff_Info(BUFF_INDEX eIndex) { return m_vecBuff[_int(eIndex)]; }

private:
	HRESULT Initialize_Stat();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	vector<BUFF*> m_vecBuff;


public:
	static CPlayer_Stat_Manager* Create(CGameInstance* pGameInstance);
	virtual void Free() override;

};

END