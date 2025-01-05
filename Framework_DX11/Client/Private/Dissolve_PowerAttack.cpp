#include "stdafx.h"
#include "Dissolve_PowerAttack.h"
#include "GameInstance.h"

CDissolve_PowerAttack::CDissolve_PowerAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDissolve_Effect(pDevice, pContext)
{
}

CDissolve_PowerAttack::CDissolve_PowerAttack(const CDissolve_PowerAttack& Prototype)
    : CDissolve_Effect(Prototype)
{
}

HRESULT CDissolve_PowerAttack::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDissolve_PowerAttack::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iShaderIndex = 16;
    m_iState = CVIBuffer_Instancing::STATE_RANDOM | CVIBuffer_Instancing::STATE_LOOP;
    return S_OK;
}

void CDissolve_PowerAttack::Priority_Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;

}

void CDissolve_PowerAttack::Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;

    CVIBuffer_Instancing::PARTICLE_MOVEMENT Movement = {};

    Movement.iState = m_iState;
    Movement.vPivot = _Vec4(0.f, 0.f, 0.f, 1.f);
    Movement.fGravity = 0.f;
    Movement.vMoveDir = _Vec4(0.f, 1.f, 0.f, 0.f);
    Movement.fTimeDelta = fTimeDelta;
    Movement.vOrbitAxis = _Vec3(0.f, 1.f, 0.f);
    Movement.fOrbitAngle = 90.f;
    Movement.fTimeInterval = 1.f;
    Movement.fRandomRatio = 1.f;
    Movement.fAccelLimit = 0.f;
    Movement.fAccelSpeed = 0.f;
    Movement.WorldMatrix = m_pTarget_TransformCom->Get_WorldMatrix();

    CVIBuffer_Dissolve_Instance::DISSOLVE_DATA Data = {};
    Data.fThreshold = 0.f;
    Data.iModelType = CModel::TYPE_ANIM;
    Data.vTextureSize = {};

    if (true == m_pVIBufferCom->DispatchCS(m_pActionCS, nullptr, m_pTarget_ModelCom, Movement, Data))
    {
        m_bOn = false;
    }
}

void CDissolve_PowerAttack::Late_Update(_float fTimeDelta)
{
    if (false == m_bOn)
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CDissolve_PowerAttack::Render()
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
    return S_OK;
}

void CDissolve_PowerAttack::Set_On(_bool bOn)
{
    if (true == bOn)
    {
        __super::Set_On(bOn);
        m_iState |= CVIBuffer_Instancing::STATE_LOOP;
    }
    else
        m_iState &= ~CVIBuffer_Instancing::STATE_LOOP;
}

HRESULT CDissolve_PowerAttack::Ready_Components()
{
    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Shere_01_C_LGS"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_Compute_Move */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Dissolve_Move_NonTexture"),
        TEXT("Com_Compute_Move"), reinterpret_cast<CComponent**>(&m_pActionCS))))
        return E_FAIL;

    return S_OK;
}

CDissolve_PowerAttack* CDissolve_PowerAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDissolve_PowerAttack* pInstance = new CDissolve_PowerAttack(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CDissolve_PowerAttack"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CDissolve_PowerAttack* CDissolve_PowerAttack::Clone(void* pArg)
{
    CDissolve_PowerAttack* pInstance = new CDissolve_PowerAttack(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CDissolve_PowerAttack"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CDissolve_PowerAttack::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pActionCS);
}
