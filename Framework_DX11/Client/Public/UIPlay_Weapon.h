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

	void Initialize_Weapon_Component(vector<struct CUIPage::UIPART_INFO*>& vecOrigin);
	void Update_WeaponInfo(_int iFable_Count_Now, _float fTimeDelta);

private:
	void Switch_Weapon();
	void Switch_Mode();

	void Start_WeaponCell_Change(_float fTimeDelta);
	void Start_FableGauge_Use(_float fTimeDelta);

	void Update_DurableGauge(_float fTimeDelta);
	void Update_WeaponCell_Fx(_float fTimeDelta);
	void Update_FableGauge(_float fTimeDelta, _int iFable_Count_Now);


private:
	const CItem_Manager::ITEM* m_pItem_Blade = { nullptr };
	const CItem_Manager::ITEM* m_pItem_Handle = { nullptr };
	CPlayer::WEAPON_TYPE m_eType_Now = CPlayer::WEAPON_TYPE::WEP_END;

	_int iNow_Fable_Count = 0;
	_int iNow_SelectWeapon = 0;
	_int m_iWeapon_Equip_Symbol = 0;

	_bool m_bIsBladeWhite_Active = false;
	_Vec2 m_vFableWhite_ActionTime = { 0.f,0.5f };



	UPART* m_pSharedPointer_Center = { nullptr };
	vector<UPART*> m_vecSharedPointer_DurableGauge_Static;
	UPART* m_pSharedPointer_DurableGauge_Fill = { nullptr };

	vector<UPART*> m_vecSharedPointer_NormalWeapon_Static;
	vector<UPART*> m_vecSharedPointer_NormalWeapon_Fx;
	UPART* m_pSharedPointer_NormalWeapon_Blade = { nullptr };
	UPART* m_pSharedPointer_NormalWeapon_Handle = { nullptr };

	vector<UPART*> m_vecSharedPointer_SpecialWeapon_Static;
	UPART* m_pSharedPointer_SpecialWeapon = { nullptr };

	UPART* m_pSharedPointer_SelectNum = { nullptr };



	UPART* m_pSharedPointer_BladeFable_Symbol = { nullptr };
	UPART* m_pSharedPointer_HandleFable_Symbol = { nullptr };

	vector<UPART*> m_vecSharedPointer_BladeFable_Side_Frame;
	vector<UPART*> m_vecSharedPointer_BladeFable_Side_White;
	vector<UPART*> m_vecSharedPointer_BladeFable_Side_Fill;

	vector<UPART*> m_vecSharedPointer_BladeFable_Center_Frame;
	vector<UPART*> m_vecSharedPointer_BladeFable_Center_White;
	vector<UPART*> m_vecSharedPointer_BladeFable_Center_Fill;

	vector<UPART*> m_vecSharedPointer_BladeFable_KeySet_F;
	vector<UPART*> m_vecSharedPointer_BladeFable_KeySet_Combine;

	vector<UPART*> m_vecSharedPointer_HandleFable_Side_Frame;
	vector<UPART*> m_vecSharedPointer_HandleFable_Side_White;
	vector<UPART*> m_vecSharedPointer_HandleFable_Side_Fill;

	vector<UPART*> m_vecSharedPointer_HandleFable_Center_Frame;
	vector<UPART*> m_vecSharedPointer_HandleFable_Center_White;
	vector<UPART*> m_vecSharedPointer_HandleFable_Center_Fill;

	vector<UPART*> m_vecSharedPointer_HandleFable_KeySet_F;
	vector<UPART*> m_vecSharedPointer_HandleFable_KeySet_Combine;

public:
	static CUIPlay_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END