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

    if (FAILED(Ready_Components(pDesc->TextureDesc)))
        return E_FAIL;

    m_ActionDesc = pDesc->ActionDesc;

    m_eEffectType   = TYPE_TEXTURE;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ActionDesc.vPos);

    m_pTransformCom->Set_Scaled(m_ActionDesc.vScale.x, m_ActionDesc.vScale.y, m_ActionDesc.vScale.z);

    m_pTransformCom->BillBoard();

    m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_ActionDesc.fRotationAngle));

    m_SaveDesc = *pDesc;

    return S_OK;
}

void CTexture_Effect::Priority_Update(_float fTimeDelta)
{
    if (true == m_isDead)
        return;

}

void CTexture_Effect::Update(_float fTimeDelta)
{
    if (true == m_isDead)
        return;

    m_pTransformCom->BillBoard();

    m_ActionDesc.fAlpha += fTimeDelta * m_ActionDesc.fAlphaSpeed;

    m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta * m_ActionDesc.fRotationSpeed);

    _Vec3 vScale = m_pTransformCom->Get_Scaled();
    vScale += m_ActionDesc.vScalingSpeed * fTimeDelta;
    m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);

    _Matrix ParentMatrix = XMMatrixIdentity();
    if (nullptr != m_pParentMatrix)
        ParentMatrix = *m_pParentMatrix;

    m_WorldMatrix = m_pTransformCom->Get_WorldMatrix() * ParentMatrix;
}

void CTexture_Effect::Late_Update(_float fTimeDelta)
{
    if (true == m_isDead)
        return;

    m_fAccumulateTime += fTimeDelta;
    m_fCurrenrtIndex += fTimeDelta * m_ActionDesc.fSpriteSpeed;

    if (m_ActionDesc.fDuration < m_fAccumulateTime)
    {
        m_isDead = true;
    }
    
    
    if(STATE_BLEND & m_ActionDesc.iState)
    {
        if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this)))
            return;
    }
    if (STATE_NONBLEND & m_ActionDesc.iState)
    {
        if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this)))
            return;
    }
    if (STATE_DISTORTION & m_ActionDesc.iState)
    {

    }

    if (STATE_BLUR & m_ActionDesc.iState)
    {

    }
}

HRESULT CTexture_Effect::Render()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pDiffuseTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pNormalTextureCom->Bind_ShadeResource(m_pShaderCom, "g_NormalTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pMaskTextureCom[0]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pMaskTextureCom[1]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture2", 0)))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_ActionDesc.vColor, sizeof m_ActionDesc.vColor)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_ActionDesc.vDivide, sizeof m_ActionDesc.vDivide)))
        return E_FAIL;

    _int iTexIndex = (_int)m_fCurrenrtIndex;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iTexIndex", &iTexIndex, sizeof iTexIndex)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_ActionDesc.fAlpha, sizeof m_ActionDesc.fAlpha)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Begin(m_ActionDesc.iShaderIndex)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CTexture_Effect::Reset()
{
    m_fAccumulateTime = { 0.f };
    m_fCurrenrtIndex = { 0.f };

    m_ActionDesc = m_SaveDesc.ActionDesc;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ActionDesc.vPos);
    m_pTransformCom->Set_Scaled(m_ActionDesc.vScale.x, m_ActionDesc.vScale.y, m_ActionDesc.vScale.z);
    m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_ActionDesc.fRotationAngle);

}

HRESULT CTexture_Effect::Ready_Components(TEXTURE_DESC Desc)
{
    if(FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Effect_Texture"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.strDiffuseTexturTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.strNomralTextureTag, TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.strMaskTextureTag_1, TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom[0]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.strMaskTextureTag_2, TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom[1]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
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
    Safe_Release(m_pDiffuseTextureCom);
    Safe_Release(m_pNormalTextureCom);
    Safe_Release(m_pMaskTextureCom[0]);
    Safe_Release(m_pMaskTextureCom[1]);
    Safe_Release(m_pShaderCom);
}
