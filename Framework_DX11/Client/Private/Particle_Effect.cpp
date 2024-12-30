#include "stdafx.h"
#include "Particle_Effect.h"
#include "GameInstance.h"

CParticle_Effect::CParticle_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffect_Base(pDevice, pContext)
{
}

CParticle_Effect::CParticle_Effect(const CParticle_Effect& Prototype)
    : CEffect_Base(Prototype)
{
}

HRESULT CParticle_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Effect::Initialize(void* pArg)
{
    PARTICLE_EFFECT_DESC* pDesc = static_cast<PARTICLE_EFFECT_DESC*>(pArg);
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(*pDesc)))
        return E_FAIL;

    m_DefaultDesc = pDesc->DefaultDesc;
    m_InitDesc = m_DefaultDesc;

    m_eEffectType = EFFECT_TYPE::TYPE_PARTICLE;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
    m_pTransformCom->Set_Scaled(m_DefaultDesc.vScale.x, m_DefaultDesc.vScale.y, m_DefaultDesc.vScale.z);
    m_pTransformCom->Rotation(m_DefaultDesc.vRotation.x, m_DefaultDesc.vRotation.y, m_DefaultDesc.vRotation.z);

    return S_OK;
}

void CParticle_Effect::Priority_Update(_float fTimeDelta)
{
    if (false == m_isActive)
        m_isActive = true;

}

void CParticle_Effect::Update(_float fTimeDelta)
{
    if (false == m_isActive)
        return;

    _Vec3 vScale = m_pTransformCom->Get_Scaled();
    vScale += m_DefaultDesc.vOverallScaling * fTimeDelta;
    m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);

    __super::Set_WorldMatrix();

    CVIBuffer_Point_Instance::PARTICLE_MOVEMENT Movement = {};

    Movement.iState         = m_DefaultDesc.iComputeState;
    Movement.vPivot         = m_DefaultDesc.vPivot;
    Movement.fGravity       = m_DefaultDesc.fGravity;
    Movement.vMoveDir       = m_DefaultDesc.vMoveDir;
    Movement.fTimeDelta     = fTimeDelta;
    Movement.vOrbitAxis     = m_DefaultDesc.vOrbitAxis;
    Movement.fOrbitAngle    = m_DefaultDesc.fOrbitAngle;
    Movement.fTimeInterval  = m_DefaultDesc.fRandomTimeInterval;
    Movement.fRandomRatio   = m_DefaultDesc.fRandomMoveRatio;
    Movement.fAccelLimit    = m_DefaultDesc.fAccelLimit;
    Movement.fAccelSpeed    = m_DefaultDesc.fAccelSpeed;
    Movement.WorldMatrix    = m_WorldMatrix;

    if(true == m_pVIBufferCom->DispatchCS(m_pActionCS, Movement))
    {
        m_isDead = true;
        m_isActive = false;
    }

    if (0.f < m_DefaultDesc.fDuration && m_DefaultDesc.fDuration < m_fAccumulateTime)
        m_DefaultDesc.iComputeState &= ~CVIBuffer_Instancing::STATE_LOOP;

}

void CParticle_Effect::Late_Update(_float fTimeDelta)
{
    if (false == m_isActive)
        return;

    m_fAccumulateTime += fTimeDelta;

    m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_RenderDesc.iRenderGroup, this);
}

HRESULT CParticle_Effect::Render()
{
    if (PT_LOCALSPREAD == m_DefaultDesc.eType || PT_LOCALMOVE == m_DefaultDesc.eType || PT_LOCALCONVERGE == m_DefaultDesc.eType)
    {
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
            return E_FAIL;
    }
    else
    {
        _Matrix WorldMatrix = XMMatrixIdentity();
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;
    }
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])
    {
        if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource_Struct(m_pShaderCom, "g_DiffuseTexture", 0)))
            return E_FAIL;
    }

    if (nullptr != m_pTextureCom[TEXTURE_MASK_1])
    {
        if (FAILED(m_pTextureCom[TEXTURE_MASK_1]->Bind_ShaderResource_Struct(m_pShaderCom, "g_MaskTexture_1", 0)))
            return E_FAIL;
    }

    if (nullptr != m_pTextureCom[TEXTURE_MASK_2])
    {
        if (FAILED(m_pTextureCom[TEXTURE_MASK_2]->Bind_ShaderResource_Struct(m_pShaderCom, "g_MaskTexture_2", 0)))
            return E_FAIL;
    }

    if (nullptr != m_pTextureCom[TEXTURE_NORMAL])
    {
        if (FAILED(m_pTextureCom[TEXTURE_NORMAL]->Bind_ShaderResource_Struct(m_pShaderCom, "g_NormalTexture", 0)))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_DefaultDesc.vTexDevide, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartScaling", &m_DefaultDesc.vStartScaling, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vScalingRatio", &m_DefaultDesc.vScalingRatio, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iState", &m_DefaultDesc.iGeomState, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_DefaultDesc.fRotationPerSecond, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fStartRotation", &m_DefaultDesc.fStartRotation, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSpeed", &m_DefaultDesc.fSpriteSpeed, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_ShaderResource_NonVTX(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffer(m_pShaderCom, "Particle_SRV")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(m_DefaultDesc.iShaderIndex)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Effect::Reset()
{
    m_DefaultDesc = m_InitDesc;
    m_isDead = false;
    m_isActive = false;
    m_fAccumulateTime = 0.f;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
    m_pTransformCom->Rotation(m_DefaultDesc.vRotation.x, m_DefaultDesc.vRotation.y, m_DefaultDesc.vRotation.z);
    m_pTransformCom->Set_Scaled(m_DefaultDesc.vScale.x, m_DefaultDesc.vScale.y, m_DefaultDesc.vScale.z);

    CVIBuffer_Point_Instance::PARTICLE_MOVEMENT Movement = {};

    m_pVIBufferCom->DispatchCS(m_pResetCS, Movement);
    m_pVIBufferCom->Reset();
}

void CParticle_Effect::Set_Loop(_bool bLoop)
{
    if (true == bLoop)
    {
        m_DefaultDesc.iComputeState |= CVIBuffer_Instancing::STATE_LOOP;
        m_InitDesc.iComputeState |= CVIBuffer_Instancing::STATE_LOOP;
        Reset();
    }
    else
    {
        m_DefaultDesc.iComputeState &= ~CVIBuffer_Instancing::STATE_LOOP;
        m_InitDesc.iComputeState &= ~CVIBuffer_Instancing::STATE_LOOP;
    }
}


HRESULT CParticle_Effect::Ready_Components(const PARTICLE_EFFECT_DESC& Desc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szDiffuseTexturTag))
    {
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szDiffuseTexturTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
            return E_FAIL;
    }

    if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szNormalTextureTag))
    {
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szNormalTextureTag, TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORMAL]))))
            return E_FAIL;
    }

    if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_1))
    {
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szMaskTextureTag_1, TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_1]))))
            return E_FAIL;
    }

    if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_2))
    {
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szMaskTextureTag_2, TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_2]))))
            return E_FAIL;
    }

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, Desc.strVIBufferTag,
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    _wstring strCSPrototypeTag = TEXT("");
    switch (Desc.DefaultDesc.eType)
    {
    case PT_SPREAD:
        strCSPrototypeTag = TEXT("Prototype_Component_Shader_Compute_Particle_Spread");
        break;

    case PT_MOVE:
        strCSPrototypeTag = TEXT("Prototype_Component_Shader_Compute_Particle_Move");
        break;

    case PT_CONVERGE:
        strCSPrototypeTag = TEXT("Prototype_Component_Shader_Compute_Particle_Converge");
        break;

    case PT_LOCALSPREAD:
        strCSPrototypeTag = TEXT("Prototype_Component_Shader_Compute_Particle_LocalSpread");
        break;

    case PT_LOCALMOVE:
        strCSPrototypeTag = TEXT("Prototype_Component_Shader_Compute_Particle_LocalMove");
        break;

    case PT_LOCALCONVERGE:
        strCSPrototypeTag = TEXT("Prototype_Component_Shader_Compute_Particle_LocalConverge");
        break;
    } 

    /* FOR.Com_ActionCS */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, strCSPrototypeTag,
        TEXT("Com_ActionCS"), reinterpret_cast<CComponent**>(&m_pActionCS))))
        return E_FAIL;

    /* FOR.Com_ResetCS */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_Reset"),
        TEXT("Com_ResetCS"), reinterpret_cast<CComponent**>(&m_pResetCS))))
        return E_FAIL;

    return S_OK;
}

CParticle_Effect* CParticle_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Effect* pInstance = new CParticle_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CParticle_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Effect::Clone(void* pArg)
{
    CParticle_Effect* pInstance = new CParticle_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CParticle_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Effect::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pActionCS);
    Safe_Release(m_pResetCS);

    for (size_t i = 0; i < TEXTURE_END; ++i)
    {
        Safe_Release(m_pTextureCom[i]);
    }

}
