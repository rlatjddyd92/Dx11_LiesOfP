#include "stdafx.h"
#include "Particle_Test.h"
#include "GameInstance.h"

CParticle_Test::CParticle_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CParticle_Test::CParticle_Test(const CParticle_Test& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CParticle_Test::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Test::Initialize(void* pArg)
{
    PARTICLE_TEST_DESC* pDesc = static_cast<PARTICLE_TEST_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_DefaultDesc = pDesc->DefaultDesc;
    m_RevolveDesc = pDesc->RevolveDesc;
    m_RandomDesc = pDesc->RandomDesc;
    m_AccelDesc = pDesc->AccelDesc;

    Set_Transform(pDesc->TransformDesc);

    return S_OK;
}

void CParticle_Test::Priority_Update(_float fTimeDelta)
{
}

void CParticle_Test::Update(_float fTimeDelta)
{
    _bool bOver = { false };
    switch (m_DefaultDesc.eType)
    {
    case TYPE_SPREAD:
        bOver = m_pVIBufferCom->Spread(m_DefaultDesc.iState, m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, m_DefaultDesc.fGravity, fTimeDelta,
            m_RevolveDesc.vRevolveAxis, XMConvertToRadians(m_RevolveDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_MOVE:
        bOver = m_pVIBufferCom->Move(m_DefaultDesc.iState, m_DefaultDesc.fRenderRatio, m_DefaultDesc.vMoveDir, m_DefaultDesc.fGravity, fTimeDelta, m_DefaultDesc.vPivot,
            m_RevolveDesc.vRevolveAxis, XMConvertToRadians(m_RevolveDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_CONVERGE:
        bOver = m_pVIBufferCom->Converge(m_DefaultDesc.iState, m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, fTimeDelta,
            m_RevolveDesc.vRevolveAxis, XMConvertToRadians(m_RevolveDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_SPREAD_INDEPENDENT:
        bOver = m_pVIBufferCom->Spread_Independent(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, m_DefaultDesc.fGravity, fTimeDelta,
            m_RevolveDesc.vRevolveAxis, XMConvertToRadians(m_RevolveDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
            break;

    case TYPE_MOVE_INDEPENDENT:
        bOver = m_pVIBufferCom->Move_Independent(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vMoveDir, m_DefaultDesc.fGravity, fTimeDelta, m_DefaultDesc.vPivot,
            m_RevolveDesc.vRevolveAxis, XMConvertToRadians(m_RevolveDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;

    case TYPE_CONVERGE_INDEPENDENT:
        bOver = m_pVIBufferCom->Converge_Independent(m_DefaultDesc.iState, m_pTransformCom->Get_WorldMatrix(), m_DefaultDesc.fRenderRatio, m_DefaultDesc.vPivot, fTimeDelta,
            m_RevolveDesc.vRevolveAxis, XMConvertToRadians(m_RevolveDesc.fAngle),
            m_RandomDesc.fTimeInterval, m_RandomDesc.fRandomRatio,
            m_AccelDesc.fAccelLimit, m_AccelDesc.fAccelSpeed);
        break;
    }

    //if (true == bOver)
    //    m_isDead = true;
}

void CParticle_Test::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_Test::Render()
{
    if(TYPE_CONVERGE == m_DefaultDesc.eType || TYPE_SPREAD == m_DefaultDesc.eType || TYPE_MOVE == m_DefaultDesc.eType)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Test::Set_Transform(TRANSFORM_DESC& Desc)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.vPos);
    m_pTransformCom->Rotation(XMConvertToRadians(Desc.vRotation.x), XMConvertToRadians(Desc.vRotation.y), XMConvertToRadians(Desc.vRotation.z));
    m_pTransformCom->Set_Scaled(Desc.vScale.x, Desc.vScale.y, Desc.vScale.z);
}

HRESULT CParticle_Test::Ready_Components(PARTICLE_TEST_DESC* pDesc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Snow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    CVIBuffer_Point_Instance::INSTANCE_DESC desc = {};
    desc.iNumInstance = pDesc->iNumInstance;
    desc.vCenter = pDesc->vCenter;
    desc.vExceptRange = pDesc->vExceptRange;
    desc.vLifeTime = pDesc->vLifeTime;
    desc.vMaxColor = pDesc->vMaxColor;
    desc.vMinColor = pDesc->vMinColor;
    desc.vRange = pDesc->vRange;
    desc.vSize = pDesc->vSize;
    desc.vSpeed = pDesc->vSpeed;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &desc)))
        return E_FAIL;

    return S_OK;
}

CParticle_Test* CParticle_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Test* pInstance = new CParticle_Test(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CParticle_Test"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Test::Clone(void* pArg)
{
    CParticle_Test* pInstance = new CParticle_Test(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CParticle_Test"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Test::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
