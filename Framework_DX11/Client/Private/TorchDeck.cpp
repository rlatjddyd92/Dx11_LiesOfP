#include "stdafx.h"
#include "TorchDeck.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect_Container.h"

CTorchDeck::CTorchDeck(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTorchDeck::CTorchDeck(const CTorchDeck& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTorchDeck::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTorchDeck::Initialize(void* pArg)
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

    m_Effect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Map_Torch"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr, _Vec3(0.f, 0.f, 0.15f));
    m_Effect->Set_Loop(true);

    return S_OK;
}

void CTorchDeck::Priority_Update(_float fTimeDelta)
{
    m_Effect->Priority_Update(fTimeDelta);
}

void CTorchDeck::Update(_float fTimeDelta)
{
    m_Effect->Update(fTimeDelta);
}

void CTorchDeck::Late_Update(_float fTimeDelta)
{

   if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
   {
       __super::Late_Update(fTimeDelta);
       m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
       m_Effect->Late_Update(fTimeDelta);
   }

}

HRESULT CTorchDeck::Render()
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

HRESULT CTorchDeck::Ready_Components(OBJECT_DEFAULT_DESC* pDesc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SM_Monastery_WallDeco_01_TorchDeck01"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}
CTorchDeck* CTorchDeck::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTorchDeck* pInstance = new CTorchDeck(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTorchDeck"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CTorchDeck::Clone(void* pArg)
{
    CTorchDeck* pInstance = new CTorchDeck(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTorchDeck"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTorchDeck::Free()
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
