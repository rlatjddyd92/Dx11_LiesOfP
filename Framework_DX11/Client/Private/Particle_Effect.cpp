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
    
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_DefaultDesc = pDesc->InitDesc.DefaultDesc;
    m_OrbitDesc = pDesc->InitDesc.OrbitDesc;
    m_RandomDesc = pDesc->InitDesc.RandomDesc;
    m_AccelDesc = pDesc->InitDesc.AccelDesc;

    Set_Transform(pDesc->InitDesc.TransformDesc);

    m_eEffectType = EFFECT_TYPE::TYPE_PARTICLE;

    m_SaveDesc = *pDesc;

    return S_OK;
}

void CParticle_Effect::Priority_Update(_float fTimeDelta)
{
    if (true == m_isDead)
        return;

}

void CParticle_Effect::Update(_float fTimeDelta)
{
    if (true == m_isDead)
        return;

    _bool bOver = { false };
    switch (m_DefaultDesc.eType)
    {
    case TYPE_SPREAD:
        bOver = m_pVIBufferCom->Spread(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, m_DefaultDesc.fGravity, fTimeDelta,
            m_OrbitDesc.vOrbitAxis, XMConvertToRadians(m_OrbitDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_MOVE:
        bOver = m_pVIBufferCom->Move(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vMoveDir, m_DefaultDesc.fGravity, fTimeDelta, m_DefaultDesc.vPivot,
            m_OrbitDesc.vOrbitAxis, XMConvertToRadians(m_OrbitDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_CONVERGE:
        bOver = m_pVIBufferCom->Converge(m_DefaultDesc.iState, m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, fTimeDelta,
            m_OrbitDesc.vOrbitAxis, XMConvertToRadians(m_OrbitDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_SPREAD_INDEPENDENT:
        bOver = m_pVIBufferCom->Spread_Independent(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, m_DefaultDesc.fGravity, fTimeDelta,
            m_OrbitDesc.vOrbitAxis, XMConvertToRadians(m_OrbitDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
            break;

    case TYPE_MOVE_INDEPENDENT:
        bOver = m_pVIBufferCom->Move_Independent(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vMoveDir, m_DefaultDesc.fGravity, fTimeDelta, m_DefaultDesc.vPivot,
            m_OrbitDesc.vOrbitAxis, XMConvertToRadians(m_OrbitDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_CONVERGE_INDEPENDENT:
        bOver = m_pVIBufferCom->Converge_Independent(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, fTimeDelta,
            m_OrbitDesc.vOrbitAxis, XMConvertToRadians(m_OrbitDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;
    }

    _Matrix ParentMatrix = XMMatrixIdentity();
    if (nullptr != m_pParentMatrix)
        ParentMatrix = *m_pParentMatrix;

    m_WorldMatrix = m_pTransformCom->Get_WorldMatrix() * ParentMatrix;

    if (true == bOver)
        m_isDead = true;
}

void CParticle_Effect::Late_Update(_float fTimeDelta)
{
    if (true == m_isDead)
        return;

    if (RS_BLUR == (RS_BLUR & m_DefaultDesc.iRenderState))
    {

    }

    if (RS_NONBLEND == (RS_NONBLEND & m_DefaultDesc.iRenderState))
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    }

    if(RS_NONLIGHT == (RS_NONLIGHT & m_DefaultDesc.iRenderState))
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
    }
}

HRESULT CParticle_Effect::Render()
{
    if(TYPE_CONVERGE == m_DefaultDesc.eType || TYPE_SPREAD == m_DefaultDesc.eType || TYPE_MOVE == m_DefaultDesc.eType)
    {
        if(FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
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

    if (FAILED(m_pDiffuseTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pNormalTextureCom->Bind_ShadeResource(m_pShaderCom, "g_NormalTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_DefaultDesc.vTexDevide, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vScaling", &m_DefaultDesc.vScaling, sizeof(_Vec2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iState", &m_DefaultDesc.iParticleState, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_DefaultDesc.fAngle, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fStartRotation", &m_DefaultDesc.fStartRotation, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSpeed", &m_DefaultDesc.fSpriteSpeed, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_DefaultDesc.iShaderIndex)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Effect::Reset()
{
    m_DefaultDesc = m_SaveDesc.InitDesc.DefaultDesc;
    m_OrbitDesc = m_SaveDesc.InitDesc.OrbitDesc;
    m_RandomDesc = m_SaveDesc.InitDesc.RandomDesc;
    m_AccelDesc = m_SaveDesc.InitDesc.AccelDesc;
    Set_Transform(m_SaveDesc.InitDesc.TransformDesc);

    m_pVIBufferCom->Reset();
}

void CParticle_Effect::Set_Transform(TRANSFORM_DESC& Desc)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.vPos);
    m_pTransformCom->Rotation(Desc.vRotation.x, Desc.vRotation.y, Desc.vRotation.z);
    m_pTransformCom->Set_Scaled(Desc.vScale.x, Desc.vScale.y, Desc.vScale.z);
}

HRESULT CParticle_Effect::Ready_Components(PARTICLE_EFFECT_DESC* pDesc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_DiffuseTexture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, pDesc->InitDesc.TextureDesc.strDiffuseTexturTag,
        TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
        return E_FAIL;

    /* FOR.Com_NormalTexture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, pDesc->InitDesc.TextureDesc.strNomralTextureTag,
        TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, pDesc->strVIBufferTag,
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
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
    
    Safe_Release(m_pDiffuseTextureCom);
    Safe_Release(m_pNormalTextureCom);

    Safe_Release(m_pVIBufferCom);
}
