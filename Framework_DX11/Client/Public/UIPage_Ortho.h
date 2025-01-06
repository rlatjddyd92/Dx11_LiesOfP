#pragma once

#include "Client_Defines.h"
#include "UIPage.h"


BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Ortho : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_HP_FRAME,
		GROUP_HP_FILL,
		GROUP_HP_COUNT,
		GROUP_FOCUS,
		GROUP_SPECIAL_HIT,
		GROUP_END
	};

	typedef struct ORTHO_HOST
	{
		UI_ORTHO_OBJ_TYPE eType = UI_ORTHO_OBJ_TYPE::ORTHO_END;
		CGameObject* pHost = { nullptr };
		_bool bIsActive = true;

		_bool bDamegeCount = true;
		_float fAngleFor_Weakness_Alpha = 360.f;

	}OR_HOST;

	

	typedef struct ORTHO_RENDER_CTRL
	{
		_float fDistance_From_Cam = 0.f;
		_Vec2 fPosition = { 0.f,0.f };
		PART_GROUP eType = PART_GROUP::GROUP_END;
		_float fRatio = 0.f;
		_float fWeaknessRatio = 0.f;
		_wstring strText = {};
		_int iTexture = -1;
		_bool bIsWeakness = false;


	}OR_RENDER;

	struct Order_Ortho_UI_Render {
		bool operator()(const ORTHO_RENDER_CTRL* First, const ORTHO_RENDER_CTRL* Second)
		{
			if (First->fDistance_From_Cam < Second->fDistance_From_Cam)
				return true;
			else if ((First->fDistance_From_Cam == Second->fDistance_From_Cam)&&(_int(First->eType) > _int(Second->eType)))
				return true;

			return false; // 카메라와 거리가 더 먼 것이 먼저 그려지도록
		}
	};

protected:
	CUIPage_Ortho(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Ortho(const CUIPage_Ortho& Prototype);
	virtual ~CUIPage_Ortho() = default;

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

	void Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE eType, void* pObj);

	HRESULT Render_Ortho_UI();

	void Set_OnOff_OrthoUI(_bool bIsOn, void* pObj);
	

private:
	void Initialize_Ortho_Info(); // <- 직교 UI 사용을 위한 기본 세팅을 진행한다 

	void CheckHost(_float fTimeDelta); // <- 모든 객체를 돌면서 그려야 하는 것이 있는 지 체크 

	// 나중을 위해 직교 UI 종류 별로 함수 나눌 것 
	void Make_Monster_HP_Bar(CGameObject* pHost, _float fTimeDelta, _float fDistance, UI_ORTHO_OBJ_TYPE eType, _float* WeakRatio);
	void Make_Monster_Focusing(CGameObject* pHost, _float fTimeDelta, _float fDistance, UI_ORTHO_OBJ_TYPE eType);
	void Make_Monster_SpecialHit(CGameObject* pHost, _float fTimeDelta, _float fDistance, UI_ORTHO_OBJ_TYPE eType);








	_bool Make_OrthoGraphy_Position(CGameObject* pHost, PART_GROUP eGroup, _Vec2* fPosition, UI_ORTHO_OBJ_TYPE eType); // <- 직교 좌표를 뽑는다, 직교 UI 타입 별 보정치도 적용한다 
	_float Check_Distance_From_Cam(CGameObject* pHost);

protected:
	list<OR_HOST*> m_Ortho_Host_list; // <- 직교 UI 가 필요한 대상 목록
	priority_queue<OR_RENDER*, vector<OR_RENDER*>, Order_Ortho_UI_Render> m_queue_Ortho_Render_Ctrl; // <- 이번 프레임에 실제 렌더 해야 하는 내용
	vector<vector<_Vec3>> m_vecOrtho_Adjust; // <- 직교 좌표 뽑을 때 대상 객체 Pos에서 어디로 얼마나 떨어진 위치에 그릴 것인지 정해놓는다 

	_float m_fTimeDelta = 0.f;

public:
	static CUIPage_Ortho* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END