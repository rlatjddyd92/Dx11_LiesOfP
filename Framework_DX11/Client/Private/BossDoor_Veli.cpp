#include "stdafx.h"
#include "BossDoor_Veli.h"
#include "GameInstance.h"

CBossDoor_Veli::CBossDoor_Veli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBossDoor_Veli::CBossDoor_Veli(const CBossDoor_Veli& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CBossDoor_Veli::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossDoor_Veli::Initialize(void* pArg)
{
	BOSSDOOR_DESC* pDesc = static_cast<BOSSDOOR_DESC*>(pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

	m_vColor = { 0.f, 0.05f, 0.1f, 10.f };
	m_vTileRepeat = { 3.f, 1.f };
	m_eDoorState = DOOR_DEFAULT;

	m_fAlpha = 0.f;
	// 0에서 문 열리고 컷신 끝나면 클로즈
	// 죽고 상호작용하면 Open, 

	return S_OK;
}

void CBossDoor_Veli::Priority_Update(_float fTimeDelta)
{
}

void CBossDoor_Veli::Update(_float fTimeDelta)
{
	m_vTileMove += _Vec2(1.f, 1.f) * 0.25f * fTimeDelta;
	m_fAccumulateTime += fTimeDelta;

	switch (m_eDoorState)
	{
	case DOOR_OPEN:
		m_fAlpha -= fTimeDelta * 0.5f;
		break;

	case DOOR_CLOSE:
		m_fAlpha += fTimeDelta * 0.5f;
		break;

	default:
		break;
	}

	m_fAlpha = max(0.f, min(1.f, m_fAlpha));
}

void CBossDoor_Veli::Late_Update(_float fTimeDelta)
{
	if (m_vTileMove.x > m_vTileRepeat.x)
		m_vTileMove = { 0.f, 0.f };

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CBossDoor_Veli::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[DIFFUSE]->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[MASK_1]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_1", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[MASK_2]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_2", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[DISSOLVE]->Bind_ShadeResource(m_pShaderCom, "g_NormalTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof m_vColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTileRepeat", &m_vTileRepeat, sizeof m_vTileRepeat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTileMove", &m_vTileMove, sizeof m_vTileMove)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof m_fAlpha)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccumulateTime", &m_fAccumulateTime, sizeof m_fAccumulateTime)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(12)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossDoor_Veli::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Mesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_SM_plane_01_GDH"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Tile_Noise_37_C_RSW"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[DIFFUSE]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Wave_01_C_GDH"),
		TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[MASK_1]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Mask_07_C_LGS"),
		TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pTextureCom[MASK_2]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_Tile_Noise_68_C_HJS"),
		TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pTextureCom[DISSOLVE]))))
		return E_FAIL;

	return S_OK;
}

CBossDoor_Veli* CBossDoor_Veli::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossDoor_Veli* pInstance = new CBossDoor_Veli(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CBossDoor_Veli"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBossDoor_Veli* CBossDoor_Veli::Clone(void* pArg)
{
	CBossDoor_Veli* pInstance = new CBossDoor_Veli(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CBossDoor_Veli"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossDoor_Veli::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for (auto& Texture : m_pTextureCom)
		Safe_Release(Texture);

}
