#include "stdafx.h"
#include "AObj_GodHands.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_GodHands::CAObj_GodHands(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_GodHands::CAObj_GodHands(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_GodHands::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_GodHands::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);
    
    m_vOriginPos = pDesc->vPos;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginPos + _Vec3{ 0.f, 20.f, 0.f });

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fDamageAmount = 270.f;

    m_fLifeDuration = 0.6f;
    m_fHandActTime = 3.9f;
    m_fSpeed = 2.2f;
    m_vMoveDir = _Vec3{0.f, -1.f, 0.f};

    m_pColliderCom->IsActive(true);

    m_bAttack = false;

    m_strObjectTag = TEXT("MonsterWeapon");
    m_pModelCom->SetUp_Animation(0);
    return S_OK;
}

void CAObj_GodHands::Priority_Update(_float fTimeDelta)
{
    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Priority_Update(fTimeDelta);
    }
}

void CAObj_GodHands::Update(_float fTimeDelta)
{
    if(!m_bEnd)
    {
        switch (m_iDropPhase)
        {
        case 0:     //사인
            if (m_pEffects[STATE_SIGN]->Get_Dead())
            {
                if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
                {
                    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SimonManus_SK_FX_Ergo_GodHand_Wind_01.wav"), &g_fEffectVolume);
                }

                ++m_iDropPhase;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginPos + _Vec3{ 0.f, 20.f, 0.f });
                m_pEffects[STATE_DROP]->Set_Loop(true);
                m_bSummoned = true;
            }
            break;

        case 1:     //손 및, 드랍 이펙트 
            m_pTransformCom->Set_State(CTransform::STATE_POSITION,
                m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vMoveDir * m_fSpeed * fTimeDelta);

            m_fLifeTime += fTimeDelta;

            if (m_fLifeTime >= m_fHandActTime)
            {
                m_pModelCom->SetUp_NextAnimation(1, false, 0.3f);
                m_pEffects[STATE_DROP]->Set_Loop(false);
            }

            if (m_fLifeTime >= m_fHandActTime + 0.15f)
            {
                if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
                {
                    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SimonManus_SK_FX_Ergo_GodHand_Exp_01.wav"), &g_fEffectVolume);
                }

                ++m_iDropPhase;
                m_pEffects[STATE_BLAST]->Reset_Effects();
                m_bSummoned = false;
                m_bAttack = true;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginPos);
            }
            break;

        case 2:     //폭발
            if (m_pEffects[STATE_BLAST]->Get_Dead())
            {
                m_bEnd = true;
            }
            break;

        default:
            break;
        }
    }
    else
    {
        m_isDead = true;
    }




    if (m_bSummoned)
    {
        m_pModelCom->Play_Animation(fTimeDelta);
    }

    if (m_bAttack)
    {
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }

    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Update(fTimeDelta);
    }
}

void CAObj_GodHands::Late_Update(_float fTimeDelta)
{
    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Late_Update(fTimeDelta);
    }

    if (m_bSummoned)
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    }

    if (m_bAttack)
    {
        m_pGameInstance->Add_ColliderList(m_pColliderCom);
#ifdef DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_GodHands::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);


        if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
                return E_FAIL;
        }

        // EMISSIVE
        if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::EMISSIVE))
        {
            m_fEmissiveMask = 1.f;
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmessiveTexture", EMISSIVE, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", EMISSIVE, (_uint)i)))
                return E_FAIL;
        }
        else
        {
            m_fEmissiveMask = 0.f;
        }
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissiveMask, sizeof(_float))))
            return E_FAIL;

        if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(2)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;
    }

    m_fEmissiveMask = 0.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissiveMask, sizeof(_float))))
        return E_FAIL;
#ifdef _DEBUG
    if (nullptr != m_pColliderCom)
        m_pColliderCom->Render();
#endif

    return S_OK;
}

HRESULT CAObj_GodHands::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_GodHands::OnCollisionEnter(CGameObject* pOther)
{
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
            pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, _Vec3{}, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
    }
}

void CAObj_GodHands::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_GodHands::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_GodHands::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_GodHand"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 3.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);


    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffects[STATE_SIGN] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_SummonHand_Sign"), pParetnMatrix,
        nullptr, _Vec3(0.f, -20.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffects[STATE_DROP] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_SummonHand_Down"), pParetnMatrix,
        nullptr, _Vec3(0.f, -6.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffects[STATE_BLAST] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_SummonHand_Impact"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    
    m_pEffects[STATE_SIGN]->Reset_Effects();

    return S_OK;
}

CAObj_GodHands* CAObj_GodHands::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_GodHands* pInstance = new CAObj_GodHands(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_GodHands"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_GodHands::Clone(void* pArg)
{
    CAObj_GodHands* pInstance = new CAObj_GodHands(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_GodHands"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_GodHands::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        for (auto& pEffect : m_pEffects)
        {
            pEffect->Set_Cloned(false);
            Safe_Release(pEffect);
        }
    }
}
