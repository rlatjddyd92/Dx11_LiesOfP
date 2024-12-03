#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Sound.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CAttackObject abstract : public CGameObject
{
public:
	typedef struct
	{
		_Vec3 vPos{};
		_Vec3 vDir{};
	}ATKOBJ_DESC;

public:
	enum EFF_SOUND_TYPE { EFF_SOUND_EFFECT1, EFF_SOUND_END };

protected:
	CAttackObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackObject(const CGameObject& Prototype);
	virtual ~CAttackObject() = default;

public:
	class CCollider* Get_Collider() { return m_pColliderCom; }
	class CSound* Get_SoundCom(EFF_SOUND_TYPE eType) { return m_pSoundCom[eType]; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

	_float Get_AtkDmg() { return m_fDamageAmount * m_fDamageRatio; }

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

	//virtual void ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false) {};
	//virtual _bool is_EndAnim(_int iAnimIndex) { return false; }

public:
	virtual void Play_Sound(EFF_SOUND_TYPE eType, const TCHAR* pSoundKey, _uint iHandIndex = 1);

public:
	void Reset_OverLapCheck() { m_DamagedObjects.clear(); }

protected:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	class CSound*	m_pSoundCom[EFF_SOUND_END] = { nullptr, };
	_float			m_fEmissiveMask = {};

protected:
	_float			m_fDamageAmount = {};
	_float			m_fDamageRatio = { 1.f };

	_float			m_fLifeTime = {};
	_float			m_fLifeDuration = {};

	list<CGameObject*>		m_DamagedObjects;

protected:
	HRESULT Ready_Components();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END