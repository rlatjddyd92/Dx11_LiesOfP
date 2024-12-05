#include "stdafx.h"
#include "AObj_LightningBall.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_LightningBall::CAObj_LightningBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_LightningBall::CAObj_LightningBall(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_LightningBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_LightningBall::Initialize(void* pArg)
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

    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"), &g_fEffectVolume);

    m_strObjectTag = TEXT("MonsterWeapon");

    m_fLifeTime = 0.f;

    m_fUpperTime = 2.f;
    m_iThunderTime = 5.f;
    m_fChangeTime = 1.f;

    m_fUpSpeed = 0.05f;
    m_vUp = _Vec3{0, 1, 0};

    return S_OK;
}

void CAObj_LightningBall::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_LightningBall::Update(_float fTimeDelta)
{
    m_fLifeTime += fTimeDelta;
    if (m_bEffectAlive)
    {
        if (m_fLifeTime >= m_fUpperTime)
        {
            if (m_pEffect->Get_Active())
            {
                m_fLifeTime = 0.f;
                
                m_pEffect->Set_Dead(true);
                const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
                m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_BlackBall_Big"), pParetnMatrix,
                    nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

                m_pEffect->Set_Loop(true);

                m_bChangeState = true;

                m_bEffectAlive = false;
            }
        }
        else
        {
            _Vec3 vPos = { m_pTransformCom->Get_State(CTransform::STATE_POSITION) };
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + m_vUp * m_fUpSpeed);
            if (!m_bChangeState)//이펙트 변환
            {
                if (m_fLifeTime >= m_fChangeTime)
                {
                    m_pEffect->Set_Dead(true);
                    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
                    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_BlackBall_Explosion"), pParetnMatrix,
                        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

                    m_pEffect->Reset_Effects();
                    m_bChangeState = true;
                }
            }
        }
    }
    else
    {
        if (m_fLifeTime >= m_iThunderTime)
        {
            //번개 오브젝트 생t성Prototype_GameObject_ThunderCalling
            //m_pCopyPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

            CAttackObject::ATKOBJ_DESC Desc{};
            Desc.vPos = _Vec3{ m_pTransformCom->Get_State(CTransform::STATE_POSITION) };
            Desc.vDir = _Vec3{ m_pCopyPlayerTransformCom->Get_State(CTransform::STATE_POSITION) } - Desc.vPos;
            Desc.vDir.Normalize();

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack_Extra"), TEXT("Prototype_GameObject_LightningSpear"), &Desc);

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

void CAObj_LightningBall::Late_Update(_float fTimeDelta)
{
    m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CAObj_LightningBall::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_LightningBall::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_LightningBall::OnCollisionEnter(CGameObject* pOther)
{
}

void CAObj_LightningBall::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_LightningBall::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_LightningBall::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;


    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_BlackBall_Expand"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffect->Set_Loop(true);

    return S_OK;
}

CAObj_LightningBall* CAObj_LightningBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_LightningBall* pInstance = new CAObj_LightningBall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_LightningBall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_LightningBall::Clone(void* pArg)
{
    CAObj_LightningBall* pInstance = new CAObj_LightningBall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_LightningBall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_LightningBall::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
}
