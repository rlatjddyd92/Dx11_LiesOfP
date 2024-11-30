#include "stdafx.h"
#include "SteppingStone.h"
#include "GameInstance.h"

_int CSteppingStone::iStaticIndex = 0;

CSteppingStone::CSteppingStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CSteppingStone::CSteppingStone(const CSteppingStone& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CSteppingStone::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSteppingStone::Initialize(void* pArg)
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

    m_iIndex = iStaticIndex;
    iStaticIndex++;

    m_strObjectTag = TEXT("SteppingStone");

    return S_OK;
}

void CSteppingStone::Priority_Update(_float fTimeDelta)
{
}

void CSteppingStone::Update(_float fTimeDelta)
{
    if (m_bCollision)
        m_fCollisonTimer += fTimeDelta;

    //if (m_fCollisonTimer >= 2.f)
    //{
    //    Change_Player_Pos();
    //}

    if (m_pColliderCom != nullptr)
    {
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }
}

void CSteppingStone::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        if (m_pColliderCom != nullptr)
            m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CSteppingStone::Render()
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

void CSteppingStone::OnCollisionEnter(CGameObject* pOther)
{
}

void CSteppingStone::OnCollisionStay(CGameObject* pOther)
{
    if (pOther->Get_Tag() == TEXT("Player"))
    {
        m_bCollision = true;
    }
}

void CSteppingStone::OnCollisionExit(CGameObject* pOther)
{
    if (pOther->Get_Tag() == TEXT("Player"))
    {
        m_bCollision = false;
        m_fCollisonTimer = 0.f;
    }
}

HRESULT CSteppingStone::Ready_Components(OBJECT_DEFAULT_DESC* pDesc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SM_Monastery_SteppingStone_01"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.2f, 1.f, 1.2f);
    ColliderDesc.vAngles = _float3(0.f,0.f,0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

void CSteppingStone::Change_Player_Pos()
{
    //플레이어 이동
    CGameObject* pPlayer = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY);
    if (pPlayer == nullptr)
        return;

    CComponent* pNavComponenet = pPlayer->Find_Component(NAVIGATION);
    CRigidBody* pRigidComponenet = static_cast<CRigidBody*>(pPlayer->Find_Component(RIGIDBODY));

    switch (m_iIndex)
    {
    case 0:
        static_cast<CNavigation*>(pNavComponenet)->Move_to_Cell(pRigidComponenet, 792);
        break;
    case 1:
        static_cast<CNavigation*>(pNavComponenet)->Move_to_Cell(pRigidComponenet, 1068);
        break;
    default:
        break;
    }        
}

CSteppingStone* CSteppingStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSteppingStone* pInstance = new CSteppingStone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSteppingStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSteppingStone::Clone(void* pArg)
{
    CSteppingStone* pInstance = new CSteppingStone(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSteppingStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSteppingStone::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
