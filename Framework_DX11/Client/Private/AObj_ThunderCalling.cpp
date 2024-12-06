#include "stdafx.h"
#include "AObj_ThunderCalling.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_ThunderCalling::CAObj_ThunderCalling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_ThunderCalling::CAObj_ThunderCalling(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_ThunderCalling::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_ThunderCalling::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pCopyPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag));

    m_fDamageAmount = 20.f;

    
    //m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"), &g_fEffectVolume);

    m_strObjectTag = TEXT("MonsterWeapon");

    m_fEffectAliveTime = 1.5f;

    m_iThunderTime = 3.f;

    return S_OK;
}

void CAObj_ThunderCalling::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_ThunderCalling::Update(_float fTimeDelta)
{
    m_fLifeTime += fTimeDelta;
    if (m_bEffectAlive)
    {
        if (m_fLifeTime >= m_fEffectAliveTime)
        {
            m_fLifeTime = 0.f;
            m_bEffectAlive = false;
        }
    }
    else
    {
        if (m_fLifeTime >= m_iThunderTime)
        {
            CAttackObject::ATKOBJ_DESC Desc{};
            Desc.vPos = _Vec3{ m_pCopyPlayerTransformCom->Get_State(CTransform::STATE_POSITION) };

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack_Extra"), TEXT("Prototype_GameObject_Thunder"), &Desc);

            m_fLifeTime = 0.f;
            ++m_iThunderCnt;
        }
    }

    if (m_iThunderCnt >= 3)
    {
        m_isDead = true;
    }
    
    m_pEffect->Update(fTimeDelta);
}

void CAObj_ThunderCalling::Late_Update(_float fTimeDelta)
{
    m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CAObj_ThunderCalling::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_ThunderCalling::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_ThunderCalling::OnCollisionEnter(CGameObject* pOther)
{
}

void CAObj_ThunderCalling::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_ThunderCalling::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_ThunderCalling::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;


    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_LightningCalling"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffect->Reset_Effects();

    return S_OK;
}

CAObj_ThunderCalling* CAObj_ThunderCalling::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_ThunderCalling* pInstance = new CAObj_ThunderCalling(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_ThunderCalling"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_ThunderCalling::Clone(void* pArg)
{
    CAObj_ThunderCalling* pInstance = new CAObj_ThunderCalling(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_ThunderCalling"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_ThunderCalling::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        m_pEffect->Set_Cloned(false);
        Safe_Release(m_pEffect);
    }
}
