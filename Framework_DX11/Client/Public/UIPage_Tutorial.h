#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)
class CUITutorial_Info;
class CUITutorial_Guide;
class CUITutorial_Timing;
class CUITutorial_Result;
class CUITutorial_Popup;

class CUIPage_Tutorial : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_CENTER,
		GROUP_INFO_FRAME,
		GROUP_INFO_MISSION,
		GROUP_INFO_SCORE,
		GROUP_GUIDE,
		GROUP_TIMING,
		GROUP_RESULT,
		GROUP_POPUP,
		GROUP_END
	};




protected:
	CUIPage_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Tutorial(const CUIPage_Tutorial& Prototype);
	virtual ~CUIPage_Tutorial() = default;

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

	// UI °´Ã¼ Á¦¾î 
	void NextMission();
	void NextGuide();
	void ShowResult();
	void OffResult();
	void ShowTiming(KEY eKey, _float fTime);

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

protected:
	void Initialize_Tutorial();
	void Update_Tutorial();


protected:
	class CUITutorial_Info* m_pInfo = { nullptr };
	class CUITutorial_Guide* m_pGuide = { nullptr };
	class CUITutorial_Timing* m_pTiming = { nullptr };
	class CUITutorial_Result* m_pResult = { nullptr };
	class CUITutorial_Popup* m_pPopup = { nullptr };

public:
	static CUIPage_Tutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END