#include "stdafx.h"
#include "SimonManus_2P_Aura.h"
#include "GameInstance.h"
#include "Particle_Effect.h"

CSimonManus_2P_Aura::CSimonManus_2P_Aura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CSimonManus_2P_Aura::CSimonManus_2P_Aura(const CSimonManus_2P_Aura& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CSimonManus_2P_Aura::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSimonManus_2P_Aura::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Componet()))
        return E_FAIL;

    SIMONMANUS_2P_AURA_DESC* pDesc = static_cast<SIMONMANUS_2P_AURA_DESC*>(pArg);

    m_pModelCom = pDesc->pModelCom;
    Safe_AddRef(m_pModelCom);

    m_pCutSceneModelCom = pDesc->pCutSceneModelCom;
    Safe_AddRef(m_pCutSceneModelCom);

    m_pManus_TransformCom = pDesc->pManus_TransformCom;
    Safe_AddRef(m_pManus_TransformCom);

    m_vTextureSize = pDesc->vTextureSize;

    m_iGeomState = CParticle_Effect::PS_GROW | CParticle_Effect::PS_ROTATION;
    m_iShaderIndex = 22;

    m_vTexDevide = { 2.f, 2.f };
    m_fRotationPerSecond = 90.f;
    m_vTextureSize = { 2048.f, 2048.f };

    return S_OK;
}

void CSimonManus_2P_Aura::Priority_Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;
}

void CSimonManus_2P_Aura::Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;

    CVIBuffer_Instancing::PARTICLE_MOVEMENT Movement = {};

    Movement.iState = CVIBuffer_Instancing::STATE_RANDOM | CVIBuffer_Instancing::STATE_LOOP | CVIBuffer_Instancing::STATE_DECEL;
    Movement.vPivot = _Vec4(0.f, 0.f, 0.f, 1.f);
    Movement.fGravity = 0.f;
    Movement.vMoveDir = _Vec4(0.f, 1.f, 0.f, 0.f);
    Movement.fTimeDelta = fTimeDelta;
    Movement.vOrbitAxis = _Vec3(0.f, 1.f, 0.f);
    Movement.fOrbitAngle = 90.f;
    Movement.fTimeInterval = 1.f;
    Movement.fRandomRatio = 1.f;
    Movement.fAccelLimit = 1.f;
    Movement.fAccelSpeed = 0.f;
    Movement.WorldMatrix = m_pManus_TransformCom->Get_WorldMatrix();

    CVIBuffer_Dissolve_Instance::DISSOLVE_DATA Data = {};
    Data.fThreshold = 0.15f;
    Data.iModelType = CModel::TYPE_ANIM;
    Data.vTextureSize = m_vTextureSize;

    if (true == m_bCutScene)
    {
        if (true == m_pCutSceneVIBufferCom->DispatchCS(m_pActionCS, m_pEmissiveTextureCom, m_pCutSceneModelCom, Movement, Data))
        {
            m_bOn = false;
        }
    }
    else
    {
        if (true == m_pVIBufferCom->DispatchCS(m_pActionCS, m_pEmissiveTextureCom, m_pModelCom, Movement, Data))
        {
            m_bOn = false;
        }
    }
}

void CSimonManus_2P_Aura::Late_Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CSimonManus_2P_Aura::Render()
{
    _Matrix WorldMatrix = XMMatrixIdentity();
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource_Struct(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_vTexDevide, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartScaling", &m_vStartScaling, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vScalingRatio", &m_vScalingRatio, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iState", &m_iGeomState, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_fRotationPerSecond, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fStartRotation", &m_fStartRotation, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSpeed", &m_fSpriteSpeed, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShaderResource_NonVTX(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
        return E_FAIL;

    if (true == m_bCutScene)
    {
        if (FAILED(m_pCutSceneVIBufferCom->Bind_Buffer(m_pShaderCom, "Particle_SRV")))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Begin(m_iShaderIndex)))
            return E_FAIL;
        if (FAILED(m_pCutSceneVIBufferCom->Render()))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pVIBufferCom->Bind_Buffer(m_pShaderCom, "Particle_SRV")))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Begin(m_iShaderIndex)))
            return E_FAIL;
        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

void CSimonManus_2P_Aura::Reset()
{
    CVIBuffer_Instancing::PARTICLE_MOVEMENT Movement = {};
    CVIBuffer_Dissolve_Instance::DISSOLVE_DATA Data = {};

    if (true == m_bCutScene)
    {
        m_pCutSceneVIBufferCom->DispatchCS(m_pResetCS, m_pEmissiveTextureCom, m_pCutSceneModelCom, Movement, Data);
        m_pCutSceneVIBufferCom->Reset();
    }
    else
    {
        m_pVIBufferCom->DispatchCS(m_pResetCS, m_pEmissiveTextureCom, m_pModelCom, Movement, Data);
        m_pVIBufferCom->Reset();
    }
    

}

HRESULT CSimonManus_2P_Aura::Ready_Componet()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxDiffuseInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Dissolve_SimonManusP2_Aura"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer_CutScene */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Dissolve_SimonManusP2_CutScene_Aura"),
        TEXT("Com_VIBuffer_CutScene"), reinterpret_cast<CComponent**>(&m_pCutSceneVIBufferCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_SubUV_Aura_01_2x2_SC_LGS"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_Compute_Move */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Dissolve_Move"),
        TEXT("Com_Compute_Move"), reinterpret_cast<CComponent**>(&m_pActionCS))))
        return E_FAIL;

    /* FOR.Com_Compute_Reset */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Dissolve_Reset"),
        TEXT("Com_Compute_Reset"), reinterpret_cast<CComponent**>(&m_pResetCS))))
        return E_FAIL;

    /* FOR.Com_Texture */
    m_pEmissiveTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Anim/Monster/Boss/SimonManus/T_CH_MOB_SimonManus_Body_A02_Renew_E.dds"), 1);
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_SubUV_Aura_01_2x2_SC_LGS"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    //    return E_FAIL;

    return S_OK;
}

CSimonManus_2P_Aura* CSimonManus_2P_Aura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSimonManus_2P_Aura* pInstance = new CSimonManus_2P_Aura(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CSimonManus_2P_Aura"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSimonManus_2P_Aura::Clone(void* pArg)
{
    CSimonManus_2P_Aura* pInstance = new CSimonManus_2P_Aura(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CSimonManus_2P_Aura"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSimonManus_2P_Aura::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pCutSceneVIBufferCom);
    
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pActionCS);
    Safe_Release(m_pResetCS);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pCutSceneModelCom);

    Safe_Release(m_pManus_TransformCom);

    Safe_Release(m_pEmissiveTextureCom);
}
