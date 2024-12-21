#include "stdafx.h"
#include "Sophia.h"
#include "GameInstance.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

CSophia::CSophia(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}


CSophia::CSophia(const CSophia& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CSophia::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSophia::Initialize(void* pArg)
{
    OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_bShadow = pDesc->bShadow;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Effect()))
        return E_FAIL;

    return S_OK;
}

void CSophia::Priority_Update(_float fTimeDelta)
{
    m_pHearEffeft->Priority_Update(fTimeDelta);
}

void CSophia::Update(_float fTimeDelta)
{
    m_pHearEffeft->Update(fTimeDelta);
}

void CSophia::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
    {
        __super::Late_Update(fTimeDelta);

        if(m_isRender)
            m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    }

    m_pHearEffeft->Late_Update(fTimeDelta);
}

HRESULT CSophia::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {

        if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
            return E_FAIL;

        if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(1)))
                return E_FAIL;
        }
        else
        {
            if (i == 0)
            {
                if (FAILED(m_pShaderCom->Begin(4)))
                    return E_FAIL;
            }
            else
            {
                if (FAILED(m_pShaderCom->Begin(0)))
                    return E_FAIL;
            }
        }

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;

    }
    return S_OK;
}

void CSophia::Active_HeartEffect()
{
    m_pHearEffeft->Set_Loop(true);

    _Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    vLook.y = 0.f;
    vLook.Normalize();

    vPos += vLook * 0.19f;
    vPos.y += 1.7f;

    m_pHearEffeft->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);

}

void CSophia::DeActive_HeartEffect()
{
    m_pHearEffeft->Set_Loop(false);
}

void CSophia::Active_DisapperEffect()
{
    _Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    vLook.y = 0.f;
    vLook.Normalize();

    vPos += vLook * 0.19f;
    vPos.y += 1.2f;

    CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Sophia_CutScene_Death"), vPos);
}

HRESULT CSophia::Ready_Components(OBJECT_DEFAULT_DESC* pDesc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Sophia_Stoned"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSophia::Ready_Effect()
{
    m_pHearEffeft = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Sophia_CutScene_Heart"), nullptr, nullptr);

    return S_OK;
}

CSophia* CSophia::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSophia* pInstance = new CSophia(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSophia"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSophia::Clone(void* pArg)
{
    CSophia* pInstance = new CSophia(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSophia"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSophia::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        m_pHearEffeft->Set_Cloned(false);
        Safe_Release(m_pHearEffeft);
    }

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
