#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Inform : public CUIPage
{
public:
	enum class PART_GROUP
	{
		INFORM_Pos,
		INFORM_Region_Fx,
		INFORM_Region_Title,
		INFORM_Region_Line,
		INFORM_Region_Desc,


		INFORM_Common_Fx,
		INFORM_Dead,
		INFORM_Recovery,
		INFORM_BossKill,

		INFORM_Stargazer_Fx0,
		INFORM_Stargazer_Fx1,
		INFORM_Stargazer,


		INFORM_Heart,
		INFORM_Line_L,
		INFORM_Line_R,
		INFORM_Text_Back,
		INFORM_Text_Front,

		INFORM_PlayerDeath_Back,

		INFORM_PlayerDeath_ClockFrame,

		INFORM_PlayerDeath_XII,
		INFORM_PlayerDeath_I,
		INFORM_PlayerDeath_II,
		INFORM_PlayerDeath_III,
		INFORM_PlayerDeath_DebrisBig,
		INFORM_PlayerDeath_DebrisSmall,

		INFORM_PlayerDeath_ClockOuterRing,
		INFORM_PlayerDeath_ClockInnerRing,

		INFORM_PlayerDeath_HourNeedle,
		INFORM_PlayerDeath_MinitueNeedle,
		INFORM_PlayerDeath_ClockInGear,

		INFORM_PlayerDeath_LieORDie,


		GROUP_END
	};

	







protected:
	CUIPage_Inform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Inform(const CUIPage_Inform& Prototype);
	virtual ~CUIPage_Inform() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OpenAction() override;
	virtual void CloseAction() override;

	virtual CHECK_MOUSE Check_Page_Action(_float fTimeDelta) override;

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

	void Show_Region_Info(_wstring strName, _wstring strDesc, _float fTime_Emerge = 1.f, _float fTime_Show = 2.f);
	void Show_Inform(INFORM_MESSAGE eInform, _float fTime_Emerge = 1.f, _float fTime_Show = 2.f);
	void Show_Heart(_wstring strScript, _float fTime_Emerge = 1.f, _float fTime_Show = 2.f);
	void Show_Dead(_float fTime_Emerge = 1.f, _float fTime_Show = 2.f);

	void Show_PlayerDead_UI();

protected:
	void Update_Region(_float fTimeDelta);
	void Update_Inform(_float fTimeDelta);
	void Update_Heart(_float fTimeDelta);
	void Update_Dead(_float fTimeDelta);

	void Update_PlayerDead_UI(_float fTimeDelta);

	void Update_PlayerDead_Back(_float fTimeDelta, _int iPhase);
	void Update_PlayerDead_Static(_float fTimeDelta, _int iPhase);
	void Update_PlayerDead_Neddle(_float fTimeDelta, _int iPhase);
	void Update_PlayerDead_Debris(_float fTimeDelta, _int iPhase);
	void Update_PlayerDead_Message(_float fTimeDelta, _int iPhase);

protected:
	_float Check_Ratio(_Vec3* vLifeTime, _float fTimeDelta);

	_Vec3 m_vLifeTime_Region = { 0.f,0.f,0.f }; // now, emerge, show
	_Vec3 m_vLifeTime_Inform = { 0.f,0.f,0.f };
	_Vec3 m_vLifeTime_Heart = { 0.f,0.f,0.f };



	// 플레이어 사망 
	_Vec4 m_vPlayerDead_Time = { -1.f,0.f,0.f,0.f }; // <- 현재 Time, 등장하는 시간, 바늘과 중심기어를 빨강으로 변경하는 시간, 사라지는 시간




	_float m_fAngle_Clock_Hour_Start = 0.f;
	_float m_fAngle_Clock_Minitue_Start = 0.f;

	_float m_fAngle_Clock_Hour = m_fAngle_Clock_Hour_Start;
	_float m_fAngle_Clock_Minitue = m_fAngle_Clock_Minitue_Start;
	_float m_fAngle_Clock_Hour_Pos = m_fAngle_Clock_Hour_Start;
	_float m_fAngle_Clock_Minitue_Pos = m_fAngle_Clock_Minitue_Start;
	_float m_fAdjust_Clock_Angle = 90.f; // 우측 방향이 Direct의 0도 기준이므로 45도 회전하여 위 방향을 기준점으로 함 
	_float m_fAdjustLength = 0.f; // <- 바늘의 Adjust_End.y 값, 구한 벡터 방향으로 이 길이만큼 가야 함 



public:
	static CUIPage_Inform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END