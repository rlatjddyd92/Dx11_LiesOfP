#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CPlayerCamera :
	public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC {
		class CPlayer* pPlayer = { nullptr };
		_float			fSpeed = {};
	}CAMERA_PLAYER_DESC;

private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CCamera& Prototype);
	virtual ~CPlayerCamera() = default;

public:
	_bool Get_IsLerpEnd() { return m_isLerpEnd; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Control_Player(_float fTimeDelta);
	void Control_Camera(_float fTimeDelta);

	void Setting_CameraControl(_float3 vStartPos, _float3 vEndPos, _float3 vCameraAt, _float fFovy = 0.f, _bool isKeepAt = true);
	void Setting_CameraControl(_fvector vStartPos, _fvector vEndPos, _fvector vCameraAt, _float fFovy = 0.f, _bool isKeepAt = true);
	void Start_CameraControl();
	void Start_CameraLerp(_float fLerpDuration);
	void End_CameraControl();


private:
	HRESULT					Ready_Components();

private:
	class CPlayer*			m_pPlayer = { nullptr };

	_float					m_fSpeed = {};
	_float					m_fRadian = {};

	_float					m_fCurrentRotationX = {};
	_float					m_fCurrentRotationY = {};

	_Vec3					m_vOffset = {};

	POINT					m_ptOldMousePos = {};

private:
	_bool					m_isControl = { false };
	_bool					m_isLerp = { false };
	_bool					m_isLerpEnd = { false };
	_bool					m_isKeepAt = { false };

private:
	void CameraShaking(_matrix WorldMatrix);

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END