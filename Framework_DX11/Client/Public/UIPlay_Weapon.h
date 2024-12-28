#pragma once

#include "Client_Defines.h"
#include "UIPlay_Component.h"
#include "Item_Manager.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPlay_Weapon : public CUIPlay_Component
{
protected:
	CUIPlay_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPlay_Weapon(const CUIPlay_Weapon& Prototype);
	virtual ~CUIPlay_Weapon() = default;

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

	void Update_WeaponInfo(_int iFable_Count_Now, _float fTimeDelta);

private:
	void Switch_Weapon();
	void Switch_Mode();
	void Set_Fable();

private:
	const CItem_Manager::ITEM* pItem = { nullptr };
	CPlayer::WEAPON_TYPE m_eType_Now = CPlayer::WEAPON_TYPE::WEP_END;




	vector<UPART*> m_vecSharedPointer_DurableGauge;

public:
	static CUIPlay_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END