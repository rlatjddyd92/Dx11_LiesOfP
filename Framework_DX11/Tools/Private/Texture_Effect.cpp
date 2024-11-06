#include "stdafx.h"
#include "Texture_Effect.h"
#include "GameInstance.h"

CTexture_Effect::CTexture_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffect_Base(pDevice, pContext)
{
}

CTexture_Effect::CTexture_Effect(const CTexture_Effect& Prototype)
    : CEffect_Base(Prototype)
{
}


HRESULT CTexture_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTexture_Effect::Initialize(void* pArg)
{
    TEXTURE_EFFECT_DESC* pDesc = static_cast<TEXTURE_EFFECT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc->strTexturTag)))
        return E_FAIL;

    m_iState        = pDesc->iState;
    m_iShaderIndex  = pDesc->iShaderIndex;
    m_fDuration     = pDesc->fDuration;
    m_vColor        = pDesc->vColor;
    m_fAlpha        = pDesc->fAlpha;
    m_vDivide       = pDesc->vDivide;
    m_fSpriteSpeed  = pDesc->fSpriteSpeed;
    m_fScalingSpeed = pDesc->fScalingSpeed;
    m_fRotationSpeed = pDesc->fRotationAngle;
    m_fAlphaSpeed = pDesc->fAlphaSpeed;

    m_eEffectType   = TYPE_TEXTURE;
    m_fStartAlpha   = m_fAlpha;
    

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_vStartScale = pDesc->vScale;


    m_pTransformCom->BillBoard();

    m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(pDesc->fRotationAngle));


    return S_OK;
}

void CTexture_Effect::Priority_Update(_float fTimeDelta)
{
}

void CTexture_Effect::Update(_float fTimeDelta)
{
    if (STATE_SCALING & m_iState)
    {
        _Vec3 vScale = m_pTransformCom->Get_Scaled();

        if (vScale.x > 0.f && vScale.y > 0.f && vScale.z > 0.f)
        {
            m_pTransformCom->Set_Scaled(vScale.x + fTimeDelta * m_fScalingSpeed, vScale.y + fTimeDelta * m_fScalingSpeed, vScale.z + fTimeDelta * m_fScalingSpeed);

            if (STATE_LOOP & m_iState && m_fDuration < m_fAccumulateTime)
            {
                m_pTransformCom->Set_Scaled(m_vStartScale.x, m_vStartScale.y, m_vStartScale.z);
                m_fAccumulateTime = 0.f;
            }
        }
        else
        {
            if (STATE_LOOP & m_iState)
            {
                m_pTransformCom->Set_Scaled(m_vStartScale.x, m_vStartScale.y, m_vStartScale.z);
            }
        }
    }

    if (STATE_ROTATION & m_iState)
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta, m_fRotationSpeed);
    }

    if (STATE_FADEIN & m_iState)
    {
        if (0.f < m_fAlpha)
            m_fAlpha -= fTimeDelta * m_fAlphaSpeed;
        else
        {
            if (STATE_LOOP & m_iState)
                m_fAlpha = m_fStartAlpha;
        }
    }
    else if (STATE_FADEOUT & m_iState)
    {
        if (m_fAlpha < 1.f)
            m_fAlpha += fTimeDelta * m_fAlphaSpeed;
        else
        {
            if (STATE_LOOP & m_iState)
                m_fAlpha = m_fStartAlpha;
        }
    }

    m_pTransformCom->BillBoard();
}

void CTexture_Effect::Late_Update(_float fTimeDelta)
{
    m_fAccumulateTime += fTimeDelta;
    m_fCurrenrtIndex += fTimeDelta * m_fSpriteSpeed;

    if (STATE_LOOP != (STATE_LOOP & m_iState))
    {
        if (m_fDuration < m_fAccumulateTime)
            m_isDead = true;

        if (1.f < (m_fCurrenrtIndex / (m_vDivide.x * m_vDivide.y - 1.f)))
            m_isDead = true;
    }

    if (STATE_DISTORTION & m_iState)
    {

    }
    else if (STATE_BLUR & m_iState)
    {

    }
    else
    {
        if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this)))
            return;
    }
}

HRESULT CTexture_Effect::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof m_vColor)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_vDivide, sizeof m_vDivide)))
        return E_FAIL;

    _int iTexIndex = (_int)m_fCurrenrtIndex;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iTexIndex", &iTexIndex, sizeof iTexIndex)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fAlpha, sizeof m_fAlpha)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_iShaderIndex)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CTexture_Effect::Set_Desc(const TEXTURE_EFFECT_DESC& desc)
{
    Reset();

    m_iState        = desc.iState;
    m_iShaderIndex  = desc.iShaderIndex;
    m_fDuration     = desc.fDuration;
    m_vColor        = desc.vColor;
    m_vDivide       = desc.vDivide;
    m_fSpriteSpeed  = desc.fSpriteSpeed;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc.vPos);
    m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), desc.fRotationAngle);
    m_fRotationSpeed = desc.fRotationSpeed;
    m_pTransformCom->Set_Scaled(desc.vScale.x, desc.vScale.y, desc.vScale.z);
    m_fScalingSpeed  = desc.fScalingSpeed;
    m_fAlpha         = desc.fAlpha;
    m_fAlphaSpeed    = desc.fAlphaSpeed;
}

CTexture_Effect::TEXTURE_EFFECT_DESC CTexture_Effect::Get_Desc()
{
    TEXTURE_EFFECT_DESC desc = {};

    desc.iState = m_iState;
    desc.iShaderIndex = m_iShaderIndex;
    desc.fDuration = m_fDuration;
    desc.vColor = m_vColor;
    desc.vDivide = m_vDivide;
    desc.fSpriteSpeed = m_fSpriteSpeed;
    desc.fRotationSpeed = m_fRotationSpeed;
    desc.fScalingSpeed = m_fScalingSpeed;
    desc.fAlpha = m_fAlpha;
    desc.fAlphaSpeed = m_fAlphaSpeed;

    return desc;
}

void CTexture_Effect::Reset()
{
    m_fAccumulateTime = { 0.f };
    m_fCurrenrtIndex = { 0.f };
}

HRESULT CTexture_Effect::Ready_Components(const _wstring strTexturTag)
{
    if(FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Effect_Texture"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL, strTexturTag, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CTexture_Effect* CTexture_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTexture_Effect* pInstance = new CTexture_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CTexture_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTexture_Effect::Clone(void* pArg)
{
    CTexture_Effect* pInstance = new CTexture_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CTexture_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTexture_Effect::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
