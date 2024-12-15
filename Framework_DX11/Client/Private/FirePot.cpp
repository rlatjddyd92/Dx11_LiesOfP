#include "stdafx.h"
#include "FirePot.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect_Container.h"

CFirePot::CFirePot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}


CFirePot::CFirePot(const CFirePot& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CFirePot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFirePot::Initialize(void* pArg)
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

    m_Effect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Map_FirePot"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr);
    m_Effect->Set_Loop(true);

    return S_OK;
}

void CFirePot::Priority_Update(_float fTimeDelta)
{
    m_Effect->Priority_Update(fTimeDelta);
}

void CFirePot::Update(_float fTimeDelta)
{
    m_Effect->Update(fTimeDelta);
}


void CFirePot::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_Effect->Late_Update(fTimeDelta);
    }
}

HRESULT CFirePot::Render()
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
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;

    }
    return S_OK;
}

HRESULT CFirePot::Ready_Components(OBJECT_DEFAULT_DESC* pDesc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SM_Cathedral_FirePot_02"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

CFirePot* CFirePot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFirePot* pInstance = new CFirePot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFirePot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFirePot::Clone(void* pArg)
{
    CFirePot* pInstance = new CFirePot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFirePot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFirePot::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        m_Effect->Set_Cloned(false);
        Safe_Release(m_Effect);
    }
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

