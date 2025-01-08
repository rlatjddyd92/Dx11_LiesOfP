#include "stdafx.h"
#include "AObj_ThunderSpread.h"

#include "GameInstance.h"
#include "Pawn.h"
#include "Player.h"

#include "Effect_Manager.h"

CAObj_ThunderSpread::CAObj_ThunderSpread(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_ThunderSpread::CAObj_ThunderSpread(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_ThunderSpread::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_ThunderSpread::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    m_vMoveDir = pDesc->vDir;

    m_pTransformCom->LookAt_Dir(_Vec4{ m_vMoveDir });

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pOwner = pDesc->pOwner;
    Safe_AddRef(m_pOwner);

    m_fDamageAmount = 250.f;

    m_fLifeDuration = 0.3f;
    m_fSpeed = 20.f;

    m_pColliderCom->IsActive(true);

    m_pColliderCom->Set_Owner(this);

    m_strObjectTag = TEXT("MonsterWeapon");

    return S_OK;
}

void CAObj_ThunderSpread::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_ThunderSpread::Update(_float fTimeDelta)
{
    if (m_fLifeTime >= m_fLifeDuration)
    {
        if (m_pEffect->Get_Loop())
        {
            m_pEffect->Set_Loop(false);
            m_fLifeTime = 0.f;
        }
        else if (m_pEffect->Get_Dead())
        {
            m_isDead = true;
            m_pSoundCom[EFF_SOUND_EFFECT1]->Stop();
        }
    }
    else
    {
        m_fLifeTime += fTimeDelta;
    }

    //¿òÁ÷ÀÓ
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vMoveDir * m_fSpeed * fTimeDelta);


    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    m_pEffect->Update(fTimeDelta);
}

void CAObj_ThunderSpread::Late_Update(_float fTimeDelta)
{
    if (m_pOwner->Get_Dead())
    {
        m_isDead = true;
    }

    m_pEffect->Late_Update(fTimeDelta);
    if (m_fLifeTime < m_fLifeDuration)
    {
        m_pGameInstance->Add_ColliderList(m_pColliderCom);
#ifdef DEBUG
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_ThunderSpread::Render()
{
#ifdef DEBUG
    m_pColliderCom->Render();
#endif // DEBUG
    return S_OK;
}

HRESULT CAObj_ThunderSpread::Render_LightDepth()
{
    return S_OK;
}

void CAObj_ThunderSpread::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pOwner->Get_IsDieState())
    {
        return;
    }
    //pOther check
    if (pOther->Get_Tag() == TEXT("Player"))
    {
        _bool bOverlapCheck = false;
        for (auto& pObj : m_DamagedObjects)
        {
            if (pObj == pOther)
            {
                bOverlapCheck = true;
                break;
            }
        }

        if (!bOverlapCheck)
        {
            m_DamagedObjects.push_back(pOther);
            _bool bHitCheck = pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, _Vec3{}, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            
            if (bHitCheck)
            {
                CPlayer* pPlayer = static_cast<CPlayer*>(pOther);

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Impact"),
                    _Vec3{ pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) + _Vec3{0.f, 1.f, 0.f} }, m_vMoveDir);

            }
            static_cast<CPlayer*>(pOther)->Calc_DebuffGain(CPawn::DEBUFF_ELEC, 20.f);
            m_pEffect->Set_Loop(false);
        }
    }
}

void CAObj_ThunderSpread::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_ThunderSpread::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_ThunderSpread::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC      ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 0.6f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, 0.3f, 0.f);
    ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderSpread"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 0.5f, 1.f));

    m_pEffect->Set_Loop(true);

    return S_OK;
}

CAObj_ThunderSpread* CAObj_ThunderSpread::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_ThunderSpread* pInstance = new CAObj_ThunderSpread(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_ThunderSpread"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_ThunderSpread::Clone(void* pArg)
{
    CAObj_ThunderSpread* pInstance = new CAObj_ThunderSpread(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_ThunderSpread"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_ThunderSpread::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        m_pEffect->Set_Cloned(false);
        Safe_Release(m_pEffect);
    }
}
