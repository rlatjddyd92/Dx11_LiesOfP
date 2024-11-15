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
		GROUP_FOCUS,
		GROUP_SPECIAL_HIT,
		GROUP_END
	};

	typedef struct ORTHO_HOST
	{
		UI_ORTHO_TYPE eType = UI_ORTHO_TYPE::ORTHO_END;
		const CGameObject* pHost = { nullptr };
	}OR_HOST;

	typedef struct ORTHO_RENDER_CTRL
	{
		_float fDistance_From_Cam = 0.f;
		UPART* pPartInfo = { nullptr };

		bool operator<(const ORTHO_RENDER_CTRL& other) const 
		{
			return fDistance_From_Cam < other.fDistance_From_Cam; // 카메라와 거리가 더 먼 것이 먼저 그려지도록
		}
	}OR_RENDER;

	/*
	0. 객체가 생성될 때 포인터를 전달 받음 
	1. 전달 받은 목록을 순회하면서 다음 내용 진행 
	 1) 렌더 여부 결정 
	 2) 렌더 진행할 직교 UI 객체 결정 
	 3) 직교 투영 진행 후 위치 잡기 
	 4) 렌더 진행할 내용 모두 정하여 OR_RENDER에 등록 
	 5) OR_RENDER vector는 20개를 미리 만들어 둠, 만일 한 프레임에 그려야 하는 게 이것을 넘어서면 10개 더 만들기 
	2. OR_RENDER에 등록된 내용을 다음 조건으로 Stable_sort 
	 1) Render 여부로 정렬 (true가 앞에 오도록)
	 2) 카메라와 거리가 가까울 수록 나중에 그려지도록
	3. UIRender_client에서 정보 참조하여 그림
	4. 그린 후 OR_RENDER의 render를 off 처리

	
	*/


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

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

	priority_queue<OR_RENDER*>* Get_Ortho_Render_Ctrl() { return &m_queue_Ortho_Render_Ctrl; } // <- Render_Client에서 정보 이용 후 pop이 가능해야 함 (안 그러면 다음 것을 못 사용함)

	void Register_Pointer_Into_OrthoUIPage(UI_ORTHO_TYPE eType, const void* pObj); 

private:
	void Initialize_Ortho_Info(); // <- 직교 UI 사용을 위한 기본 세팅을 진행한다 
	void Make_OrthoGraphy_Position(const CGameObject* pHost, PART_GROUP eGroup, _float2* fPosition); // <- 직교 좌표를 뽑는다, 직교 UI 타입 별 보정치도 적용한다 

protected:
	list<OR_HOST*> m_Ortho_Host_list; // <- 직교 UI 가 필요한 대상 목록
	priority_queue<OR_RENDER*> m_queue_Ortho_Render_Ctrl; // <- 이번 프레임에 실제 렌더 해야 하는 내용
	vector<UG_CTRL*> m_vecOrtho_Group_Ctrl; // <- 직교 UI 조정을 위한 콘트롤 구조체 원본
	vector<_float3> m_vecOrtho_Adjust; // <- 직교 좌표 뽑을 때 대상 객체 Pos에서 어디로 얼마나 떨어진 위치에 그릴 것인지 정해놓는다 

public:
	static CUIPage_Ortho* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END