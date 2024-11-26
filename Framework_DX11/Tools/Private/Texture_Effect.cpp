#include "stdafx.h"
#include "Texture_Effect.h"
#include "GameInstance.h"

#include "Controller_EffectTool.h"

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

    if (FAILED(Ready_Components(pDesc->TextDesc)))
        return E_FAIL;

    m_DefaultDesc = pDesc->DefaultDesc;

    m_eEffectType   = TYPE_TEXTURE;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);

    m_pTransformCom->Set_Scaled(m_DefaultDesc.vStartScale.x, m_DefaultDesc.vStartScale.y, m_DefaultDesc.vStartScale.z);

    m_InitDesc = *pDesc;

    return S_OK;
}

void CTexture_Effect::Priority_Update(_float fTimeDelta)
{
}

void CTexture_Effect::Update(_float fTimeDelta)
{
    m_DefaultDesc.fAlpha += fTimeDelta * m_DefaultDesc.fAlphaSpeed;

    _Vec3 vScale = m_pTransformCom->Get_Scaled();
    vScale += m_DefaultDesc.vScalingSpeed * fTimeDelta;

    if(0.f < vScale.x && 0.f < vScale.y && 0.f < vScale.z)
        m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);

    if(0.f < m_DefaultDesc.vDivide.x * m_DefaultDesc.vDivide.y)
        m_fCurrenrtIndex += fTimeDelta * m_DefaultDesc.fSpriteSpeed;

    __super::Set_WorldMatrix();

    _Vec3 vCurrentScale = _float3(m_WorldMatrix.Right().Length(), m_WorldMatrix.Up().Length(), m_WorldMatrix.Forward().Length());
    _Vec3 vPos = XMLoadFloat4x4(&m_WorldMatrix).r[3];
    _Vec3 vCamPos = m_pGameInstance->Get_CamPosition_Vec3();
    _Vec3 vDir = vPos - vCamPos;
    _Vec3 vLook = XMVector3Normalize(vDir);

    if (true == m_DefaultDesc.bPreserveRotation)
    {
        Preserve_Rotation_Billboard(vCurrentScale, vLook);
    }
    else
    {
        Billboard(vCurrentScale, vLook);
    }
}

void CTexture_Effect::Late_Update(_float fTimeDelta)
{
    m_fAccumulateTime += fTimeDelta;

    if (m_DefaultDesc.fDuration < m_fAccumulateTime || (m_DefaultDesc.vDivide.x * m_DefaultDesc.vDivide.y - 1.f) < m_fCurrenrtIndex)
    {
        if (true == m_DefaultDesc.bLoop)
            Reset();
        else
        {
            m_isActive = false;
        }
    }

    if (CRenderer::RG_END <= m_RenderDesc.iRenderGroup)
        return;
    m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_RenderDesc.iRenderGroup, this);
}

HRESULT CTexture_Effect::Render()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])
    {
        if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
            return E_FAIL;
    }

    if (nullptr != m_pTextureCom[TEXTURE_MASK_1])
    {
        if (FAILED(m_pTextureCom[TEXTURE_MASK_1]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_1", 0)))
            return E_FAIL;
    }

    if (nullptr != m_pTextureCom[TEXTURE_MASK_2])
    {
        if (FAILED(m_pTextureCom[TEXTURE_MASK_2]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_2", 0)))
            return E_FAIL;
    }

    if (nullptr != m_pTextureCom[TEXTURE_NORMAL])
    {
        if (FAILED(m_pTextureCom[TEXTURE_NORMAL]->Bind_ShadeResource(m_pShaderCom, "g_NormalTexture", 0)))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_DefaultDesc.vColor, sizeof m_DefaultDesc.vColor)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_DefaultDesc.vDivide, sizeof m_DefaultDesc.vDivide)))
        return E_FAIL;

    _int iTexIndex = (_int)m_fCurrenrtIndex;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iTexIndex", &iTexIndex, sizeof iTexIndex)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_DefaultDesc.fAlpha, sizeof m_DefaultDesc.fAlpha)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Begin(m_DefaultDesc.iShaderIndex)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CTexture_Effect::Set_Desc(const TEXTURE_EFFECT_DESC& desc)
{
    m_DefaultDesc = desc.DefaultDesc;
    m_RenderDesc = desc.RenderDesc;
    m_InitDesc.DefaultDesc = desc.DefaultDesc;
    m_InitDesc.RenderDesc = desc.RenderDesc;


    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
    m_pTransformCom->Set_Scaled(m_DefaultDesc.vStartScale.x, m_DefaultDesc.vStartScale.y, m_DefaultDesc.vStartScale.z);

    Reset();
}

void CTexture_Effect::Reset()
{
    m_fAccumulateTime = { 0.f };
    m_fCurrenrtIndex = { 0.f };
    m_isActive = true;

    m_DefaultDesc = m_InitDesc.DefaultDesc;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
    m_pTransformCom->Set_Scaled(m_DefaultDesc.vStartScale.x, m_DefaultDesc.vStartScale.y, m_DefaultDesc.vStartScale.z);
}

HRESULT CTexture_Effect::Save(_wstring strFilePath)
{
    if (strFilePath.back() == L'\0')
        strFilePath.resize(strFilePath.size() - 1);

    _wstring strResultPath = strFilePath + TEXT("\\") + m_strEffectName + TEXT(".TE");

    _char FilePath[MAX_PATH] = {};
    int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (sizeNeeded > 0 && sizeNeeded <= MAX_PATH)
    {
        WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, FilePath, MAX_PATH, nullptr, nullptr);
    }

    ofstream outfile(FilePath, ios::binary);

    if (!outfile.is_open())
        return E_FAIL;

    outfile.write(reinterpret_cast<const _char*>(m_InitDesc.szEffectName), sizeof(m_InitDesc.szEffectName));
    outfile.write(reinterpret_cast<const _char*>(&m_InitDesc.RenderDesc), sizeof(m_InitDesc.RenderDesc));
    outfile.write(reinterpret_cast<const _char*>(&m_InitDesc.DefaultDesc), sizeof(m_InitDesc.DefaultDesc));
    outfile.write(reinterpret_cast<const _char*>(&m_InitDesc.TextDesc), sizeof(m_InitDesc.TextDesc));

    outfile.close();

    return S_OK;
}

HRESULT CTexture_Effect::Ready_Components(const TEXT_DESC& Desc)
{
    if(FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Effect_Texture"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (0 != wcscmp(NONE_TEXT, Desc.szDiffuseTexturTag))
    {
        if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.szDiffuseTexturTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
            return E_FAIL;
    }

    if (0 != wcscmp(NONE_TEXT, Desc.szNormalTextureTag))
    {
        if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.szNormalTextureTag, TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORMAL]))))
            return E_FAIL;
    }

    if (0 != wcscmp(NONE_TEXT, Desc.szMaskTextureTag_1))
    {
        if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.szMaskTextureTag_1, TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_1]))))
            return E_FAIL;
    }

    if (0 != wcscmp(NONE_TEXT, Desc.szMaskTextureTag_2))
    {
        if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.szMaskTextureTag_2, TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_2]))))
            return E_FAIL;
    }

    return S_OK;
}

void CTexture_Effect::Preserve_Rotation_Billboard(_Vec3 vCurrentScale, _Vec3 vLook)
{
    _Vec3 vBeforeLook = XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[2]);

    _Vec3 vAxis = XMVector3Normalize(XMVector3Cross(vBeforeLook, vLook));
    _float fRadian = acos(XMVectorGetX(XMVector3Dot(vBeforeLook, vLook)));
    _Matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

    _Vec4 vRight = XMVector3TransformNormal(m_WorldMatrix.Right(), RotationMatrix);
    _Vec4 vUp = XMVector3TransformNormal(m_WorldMatrix.Up(), RotationMatrix);

    _Matrix LocalRotationMatrix = XMMatrixRotationAxis(vLook,
        XMConvertToRadians(m_DefaultDesc.fStarRotation + m_DefaultDesc.fRotationPerSecond * m_fAccumulateTime));

    vRight = XMVector3TransformNormal(vRight, LocalRotationMatrix);
    vUp = XMVector3TransformNormal(vUp, LocalRotationMatrix);

    XMStoreFloat3((_float3*)&m_WorldMatrix.m[0][0], vRight * vCurrentScale.x);
    XMStoreFloat3((_float3*)&m_WorldMatrix.m[1][0], vUp * vCurrentScale.y);
    XMStoreFloat3((_float3*)&m_WorldMatrix.m[2][0], vLook * vCurrentScale.z);
}

void CTexture_Effect::Billboard(_Vec3 vCurrentScale, _Vec3 vLook)
{
    _Vec4 vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _Vec4 vUp = XMVector3Cross(vLook, vRight);

    _Matrix LocalRotationMatrix = XMMatrixRotationAxis(vLook,
        XMConvertToRadians(m_DefaultDesc.fStarRotation + m_DefaultDesc.fRotationPerSecond * m_fAccumulateTime));

    vRight = XMVector3TransformNormal(vRight, LocalRotationMatrix);
    vUp = XMVector3TransformNormal(vUp, LocalRotationMatrix);

    XMStoreFloat3((_float3*)&m_WorldMatrix.m[0][0], vRight * vCurrentScale.x);
    XMStoreFloat3((_float3*)&m_WorldMatrix.m[1][0], vUp * vCurrentScale.y);
    XMStoreFloat3((_float3*)&m_WorldMatrix.m[2][0], vLook * vCurrentScale.z);
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
    Safe_Release(m_pShaderCom);

    for (size_t i = 0; i < TEXTURE_END; ++i)
    {
        Safe_Release(m_pTextureCom[i]);
    }
}
