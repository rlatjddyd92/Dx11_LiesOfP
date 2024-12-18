#include "stdafx.h"

#include "Machine_EffectObj.h"
#include "GameInstance.h"
#include "Effect_Container.h"
#include "Effect_Manager.h"

CMachine_EffectObj::CMachine_EffectObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice , pContext }
{
}

CMachine_EffectObj::CMachine_EffectObj(const CMachine_EffectObj& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMachine_EffectObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMachine_EffectObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pMachineEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("MonsteryMachine_Active"), m_pTransformCom->Get_WorldMatrix_Ptr(),
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_pMachineEffect->Set_Loop(true);

	return S_OK;
}

void CMachine_EffectObj::Priority_Update(_float fTimeDelta)
{
	m_pMachineEffect->Priority_Update(fTimeDelta);
}

void CMachine_EffectObj::Update(_float fTimeDelta)
{
	m_pMachineEffect->Update(fTimeDelta);
}

void CMachine_EffectObj::Late_Update(_float fTimeDelta)
{
	_bool isEffectRender = false;

	if (m_pGameInstance->Is_Active_Octree() == false || m_pGameInstance->Is_In_FrustumCulledOctree(m_WorldOctreeIndex))
	{
		if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50.f))
		{
			isEffectRender = true;
		}
	}

	m_pMachineEffect->Late_Update(fTimeDelta);
}

HRESULT CMachine_EffectObj::Render()
{
    return S_OK;
}

CMachine_EffectObj* CMachine_EffectObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMachine_EffectObj* pInstance = new CMachine_EffectObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMachine_EffectObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMachine_EffectObj::Clone(void* pArg)
{
	CMachine_EffectObj* pInstance = new CMachine_EffectObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMachine_EffectObj::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		m_pMachineEffect->Set_Cloned(false);
		Safe_Release(m_pMachineEffect);
	}
}