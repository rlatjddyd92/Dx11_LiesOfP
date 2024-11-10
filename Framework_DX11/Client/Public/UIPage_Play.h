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

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }

protected:
	virtual HRESULT Ready_UIPart() override;











public:
	static CUIPage_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END