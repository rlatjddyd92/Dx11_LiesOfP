#include "stdafx.h"
#include "Dissolve_SimonManus_Dead.h"
#include "GameInstance.h"

CDissolve_SimonManus_Dead::CDissolve_SimonManus_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CDissolve_SimonManus_Dead::CDissolve_SimonManus_Dead(const CDissolve_SimonManus_Dead& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CDissolve_SimonManus_Dead::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDissolve_SimonManus_Dead::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Componet()))
        return E_FAIL;

    DISSOLVE_OBJECT_DESC* pDesc = static_cast<DISSOLVE_OBJECT_DESC*>(pArg);

    m_pModelCom = pDesc->pModelCom;
    Safe_AddRef(m_pModelCom);

    m_pManusTransformCom = pDesc->pManusTransformCom;
    Safe_AddRef(m_pManusTransformCom);

    m_pDissolveTextureCom = pDesc->pDissolveTextureCom;
    Safe_AddRef(m_pDissolveTextureCom);

    m_pThreshold = pDesc->pThreshold;
    m_vTextureSize = pDesc->vTextureSize;

    m_iShaderIndex = 16;

    return S_OK;
}

void CDissolve_SimonManus_Dead::Priority_Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;
}

void CDissolve_SimonManus_Dead::Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;

    CVIBuffer_Instancing::PARTICLE_MOVEMENT Movement = {};

    Movement.iState |= CVIBuffer_Instancing::STATE_RANDOM;
    Movement.vPivot = _Vec4(0.f, 0.f, 0.f, 1.f);
    Movement.fGravity = 0.f;
    Movement.vMoveDir = _Vec4(0.f, 1.f, 0.f, 0.f);
    Movement.fTimeDelta = fTimeDelta;
    Movement.vOrbitAxis = _Vec3(0.f, 1.f, 0.f);
    Movement.fOrbitAngle = 90.f;
    Movement.fTimeInterval = 1.f;
    Movement.fRandomRatio = 0.5f;
    Movement.fAccelLimit = 0.f;
    Movement.fAccelSpeed = 0.f;
    Movement.WorldMatrix = m_pManusTransformCom->Get_WorldMatrix();

    CVIBuffer_Dissolve_Instance::DISSOLVE_DATA Data = {};
    Data.fThreshold = 1.f - *m_pThreshold;
    Data.iModelType = CModel::TYPE_ANIM;
    Data.vTextureSize = m_vTextureSize;

    if (true == m_pVIBufferCom->DispatchCS(m_pActionCS, m_pDissolveTextureCom, m_pModelCom, Movement, Data))
    {
        m_bOn = false;
    }
}

void CDissolve_SimonManus_Dead::Late_Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CDissolve_SimonManus_Dead::Render()
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

    if (FAILED(m_pVIBufferCom->Bind_Buffer(m_pShaderCom, "Particle_SRV")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(m_iShaderIndex)))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CDissolve_SimonManus_Dead::Reset()
{
    CVIBuffer_Instancing::PARTICLE_MOVEMENT Movement = {};
    CVIBuffer_Dissolve_Instance::DISSOLVE_DATA Data = {};

    m_pVIBufferCom->DispatchCS(m_pResetCS, m_pDissolveTextureCom, m_pModelCom, Movement, Data);
    m_pVIBufferCom->Reset();
}

HRESULT CDissolve_SimonManus_Dead::Ready_Componet()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxDiffuseInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Container */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Dissolve_SimonManus_Dead"),
        TEXT("Com_Container"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Shere_01_C_LGS"),
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

    return S_OK;
}

CDissolve_SimonManus_Dead* CDissolve_SimonManus_Dead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDissolve_SimonManus_Dead* pInstance = new CDissolve_SimonManus_Dead(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CDissolve_SimonManus_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDissolve_SimonManus_Dead::Clone(void* pArg)
{
    CDissolve_SimonManus_Dead* pInstance = new CDissolve_SimonManus_Dead(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CDissolve_SimonManus_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDissolve_SimonManus_Dead::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pActionCS);
    Safe_Release(m_pResetCS);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pManusTransformCom);
    Safe_Release(m_pDissolveTextureCom);
}
