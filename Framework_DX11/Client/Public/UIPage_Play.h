#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Play : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_HP_STATIC,
		GROUP_HP_FRAME,
		GROUP_HP_FILL,
		GROUP_ST_STATIC,
		GROUP_ST_FRAME,
		GROUP_ST_FILL,
		GROUP_SP0_FRAME,
		GROUP_SP0_FILL,
		GROUP_SP1_FRAME,
		GROUP_SP1_FILL,
		GROUP_SP2_FRAME,
		GROUP_SP2_FILL,
		GROUP_SP3_FRAME,
		GROUP_SP3_FILL,
		GROUP_SP4_FRAME,
		GROUP_SP4_FILL,
		GROUP_END
	};

protected:
	CUIPage_Play(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Play(const CUIPage_Play& Prototype);
	virtual ~CUIPage_Play() = default;

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

	void SetRatio_HPBarMax(_float fRatio) { HP_Gauge_Frame->fRatio = fRatio; }
	void SetRatio_HPBarFill(_float fRatio) { HP_Gauge_Fill->fRatio = fRatio; }

public:
	

private:
	// 체력바 조정
	UG_CTRL* HP_Gauge_Frame;
	UG_CTRL* HP_Gauge_Fill;

	// 스태미나 조정
	UG_CTRL* Stamina_Gauge_Frame;
	UG_CTRL* Stamina_Gauge_Fill;

	// 특수 스킬 바 조정
	vector<UG_CTRL*> vecSpecial_Gauge_Frame;
	vector<UG_CTRL*> vecSpecial_Gauge_Fill;










public:
	static CUIPage_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END