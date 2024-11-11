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
    m_iNumInstance = pDesc->BufferDesc.iNumInstance;
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
}

void CParticle_Effect::Update(_float fTimeDelta)
{
    __super::Set_WorldMatrix();

    _bool bOver = { false };

    CVIBuffer_Point_Instance::PARTICLE_MOVEMENT Movement = {};

    Movement.iState = m_DefaultDesc.iState;
    Movement.vPivot = m_DefaultDesc.vPivot;
    Movement.fGravity = m_DefaultDesc.fGravity;
    Movement.vMoveDir = m_DefaultDesc.vMoveDir;
    Movement.fTimeDelta = fTimeDelta;
    Movement.vOrbitAxis = m_OrbitDesc.vOrbitAxis;
    Movement.fOrbitAngle = m_OrbitDesc.fAngle;
    Movement.fTimeInterval = m_RandomDesc.fTimeInterval;
    Movement.fRandomRatio = m_RandomDesc.fRandomRatio;
    Movement.fAccelLimit = m_AccelDesc.fAccelLimit;
    Movement.fAccelSpeed = m_AccelDesc.fAccelSpeed;
    Movement.WorldMatrix = m_WorldMatrix;

    switch (m_DefaultDesc.eType)
    {
    case TYPE_SPREAD:
        m_pVIBufferCom->DispatchCS(m_pSpreadCS, Movement);
        break;

    case TYPE_MOVE:
        m_pVIBufferCom->DispatchCS(m_pMoveCS, Movement);
        break;

    case TYPE_CONVERGE:
        m_pVIBufferCom->DispatchCS(m_pConvergeCS, Movement);
        break;

    case TYPE_SPREAD_INDEPENDENT:
        m_pVIBufferCom->DispatchCS(m_pSpreadCS_World, Movement);
        break;

    case TYPE_MOVE_INDEPENDENT:
        m_pVIBufferCom->DispatchCS(m_pMoveCS_World, Movement);
        break;

    case TYPE_CONVERGE_INDEPENDENT:
        m_pVIBufferCom->DispatchCS(m_pConvergeCS_World, Movement);
        break;
    }

    //if (true == bOver)
    //    m_isDead = true;

}

void CParticle_Effect::Late_Update(_float fTimeDelta)
{
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
        m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
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

    if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource_Struct(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pNormalTextureCom->Bind_ShaderResource_Struct(m_pShaderCom, "g_NormalTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pMaskTextureCom->Bind_ShaderResource_Struct(m_pShaderCom, "g_MaskTexture", 0)))
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
    m_DefaultDesc = m_SaveDesc.InitDesc.DefaultDesc;
    m_OrbitDesc = m_SaveDesc.InitDesc.OrbitDesc;
    m_RandomDesc = m_SaveDesc.InitDesc.RandomDesc;
    m_AccelDesc = m_SaveDesc.InitDesc.AccelDesc;
    Set_Transform(m_SaveDesc.InitDesc.TransformDesc);
    m_isFirst = false;

    CVIBuffer_Point_Instance::PARTICLE_MOVEMENT Movement = {};

    m_pVIBufferCom->DispatchCS(m_pResetCS, Movement);
}

HRESULT CParticle_Effect::Save(_wstring strFilePath)
{
    if (strFilePath.back() == L'\0')
        strFilePath.resize(strFilePath.size() - 1);

    _wstring strResultPath = strFilePath + TEXT("\\") + m_strEffectName + TEXT(".PE");

    _char FilePath[MAX_PATH] = {};
    int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (sizeNeeded > 0 && sizeNeeded <= MAX_PATH)
    {
        WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, FilePath, MAX_PATH, nullptr, nullptr);
    }

    ofstream outfile(FilePath, ios::binary);

    if (!outfile.is_open())
        return E_FAIL;
    
    outfile.write(reinterpret_cast<const _char*>(m_SaveDesc.strEffectName), sizeof(m_SaveDesc.strEffectName));
    outfile.write(reinterpret_cast<const _char*>(&m_SaveDesc.BufferDesc), sizeof(m_SaveDesc.BufferDesc));
    outfile.write(reinterpret_cast<const _char*>(&m_SaveDesc.InitDesc), sizeof(m_SaveDesc.InitDesc));
    
    outfile.close();


    return S_OK;
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
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_DiffuseTexture */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, pDesc->InitDesc.TextureDesc.strDiffuseTexturTag,
        TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
        return E_FAIL;
    
    /* FOR.Com_NormalTexture */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, pDesc->InitDesc.TextureDesc.strNomralTextureTag,
        TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
        return E_FAIL;

    /* FOR.Com_MaskTexture */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, pDesc->InitDesc.TextureDesc.strMaskTextureTag,
        TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
        return E_FAIL;

    CVIBuffer_Point_Instance::INSTANCE_DESC desc = {};
    desc.iNumInstance = pDesc->BufferDesc.iNumInstance;
    desc.vCenter = pDesc->BufferDesc.vCenter;
    desc.vExceptRange = pDesc->BufferDesc.vExceptRange;
    desc.vLifeTime = pDesc->BufferDesc.vLifeTime;
    desc.vMaxColor = pDesc->BufferDesc.vMaxColor;
    desc.vMinColor = pDesc->BufferDesc.vMinColor;
    desc.vRange = pDesc->BufferDesc.vRange;
    desc.vSize = pDesc->BufferDesc.vSize;
    desc.vSpeed = pDesc->BufferDesc.vSpeed;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &desc)))
        return E_FAIL;

    /* FOR.Com_SpreadCS */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Particle_Spread"),
        TEXT("Com_SpreadCS"), reinterpret_cast<CComponent**>(&m_pSpreadCS))))
        return E_FAIL;

    /* FOR.Com_MoveCS */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Particle_Move"),
        TEXT("Com_MoveCS"), reinterpret_cast<CComponent**>(&m_pMoveCS))))
        return E_FAIL;

    /* FOR.Com_ConvergeCS */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Particle_Converge"),
        TEXT("Com_ConvergeCS"), reinterpret_cast<CComponent**>(&m_pConvergeCS))))
        return E_FAIL;

    /* FOR.Com_SpreadCS_World */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Particle_Spread_World"),
        TEXT("Com_SpreadCS_World"), reinterpret_cast<CComponent**>(&m_pSpreadCS_World))))
        return E_FAIL;

    /* FOR.Com_MoveCS_World */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Particle_Move_World"),
        TEXT("Com_MoveCS_World"), reinterpret_cast<CComponent**>(&m_pMoveCS_World))))
        return E_FAIL;

    /* FOR.Com_ConvergeCS_World */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Particle_Converge_World"),
        TEXT("Com_ConvergeCS_World"), reinterpret_cast<CComponent**>(&m_pConvergeCS_World))))
        return E_FAIL;

    /* FOR.Com_ResetCS */
    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Particle_Reset"),
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
    
    Safe_Release(m_pDiffuseTextureCom);
    Safe_Release(m_pNormalTextureCom);
    Safe_Release(m_pMaskTextureCom);

    Safe_Release(m_pVIBufferCom);
    
    Safe_Release(m_pSpreadCS);
    Safe_Release(m_pMoveCS);
    Safe_Release(m_pConvergeCS);
    Safe_Release(m_pSpreadCS_World);
    Safe_Release(m_pMoveCS_World);
    Safe_Release(m_pConvergeCS_World);
    Safe_Release(m_pResetCS);
}
