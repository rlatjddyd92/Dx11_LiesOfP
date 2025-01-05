#include "stdafx.h"
#include "Decal_Blood.h"

#include "GameInstance.h"

#include "ObjectPool.h"

CDecal_Blood::CDecal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal_Blood::CDecal_Blood(const CDecal_Blood& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CDecal_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal_Blood::Initialize(void* pArg)
{
	BLOOD_DECAL_DESC* pDesc = static_cast<BLOOD_DECAL_DESC*>(pArg);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_isNormal = true;
	m_bUseWorldColor = false;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CDecal_Blood::Priority_Update(_float fTimeDelta)
{
}

void CDecal_Blood::Update(_float fTimeDelta)
{
	m_fRenderTimer += fTimeDelta;
	if (m_fRenderTimer >= m_fRenderTime)
	{
		m_fCurrentSize -= (m_fShrinkSpeed * fTimeDelta);

		if (m_fCurrentSize <= 0.f)
		{
			CObjectPool<CDecal_Blood>::Return_GameObject(this);
			m_isActive = false;
		}
		else
			m_pTransformCom->Set_Scaled(m_fCurrentSize, m_fCurrentSize, m_fCurrentSize);
	}
}

void CDecal_Blood::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_DECAL, this);
}

HRESULT CDecal_Blood::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_vDecalWorldInverse", &m_pTransformCom->Get_WorldMatrix_Inverse())))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Diffuse[m_eType]->Bind_ShadeResource(m_pShaderCom, "g_DeacalDiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_Normal[m_eType]->Bind_ShadeResource(m_pShaderCom, "g_DeacalNormalTexture", 0)))
		return E_FAIL;

	_bool bFalse = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("bARM", &bFalse, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("vColor", &m_vColor, sizeof(_Vec3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_ARM"), "g_ARMTexture")))
		return E_FAIL;

	if(!m_isTrailDecal)
		m_pShaderCom->Begin(2);
	else
		m_pShaderCom->Begin(3);


	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CDecal_Blood::Active_Random(_Vec3 vPos)
{
	m_fCurrentSize = m_pGameInstance->Get_Random(0.3f, 1.f);
	m_fShrinkSpeed = m_pGameInstance->Get_Random(0.2f, 0.3f);
	m_fRenderTime = m_pGameInstance->Get_Random(1.f, 2.f);
	m_pTransformCom->Set_Scaled(m_fCurrentSize, m_fCurrentSize, m_fCurrentSize);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fRenderTimer = 0.f;
	m_eType = (BLOOD_TYPE)(rand() % 4);

	m_isTrailDecal = false;
	m_isActive = true;
}

void CDecal_Blood::Active_Trail(_Vec3 vPos)
{
	m_fCurrentSize = 1.f;
	m_fShrinkSpeed = 0.25f;
	m_fRenderTime = 1.5f;
	m_pTransformCom->Set_Scaled(m_fCurrentSize, m_fCurrentSize, m_fCurrentSize);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fRenderTimer = 0.f;
	m_eType = TYPE_TRAIL;

	m_isTrailDecal = true;
	m_isActive = true;
}

HRESULT CDecal_Blood::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_SSD"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_01_C_LGS"),
		TEXT("Com_Texture0"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse[0]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_01_N_LGS"),
		TEXT("Com_NormalTexture0"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal[0]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_02_C_LGS"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse[1]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_02_N_LGS"),
		TEXT("Com_NormalTexture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal[1]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_03_C_LGS"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse[2]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_03_N_LGS"),
		TEXT("Com_NormalTexture2"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal[2]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_04_C_LGS"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse[3]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_04_N_LGS"),
		TEXT("Com_NormalTexture3"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal[3]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_TrailFlow_01_C_RSW"),
		TEXT("Com_Texture4"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse[3]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_TrailFlow_01_C_RSW"),		//마스크 텍스쳐
		TEXT("Com_NormalTexture4"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal[3]))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CDecal_Blood* CDecal_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal_Blood* pInstance = new CDecal_Blood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDecal_Blood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal_Blood::Clone(void* pArg)
{
	CDecal_Blood* pInstance = new CDecal_Blood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDecal_Blood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal_Blood::Free()
{
	__super::Free();

	for (_uint i = 0; i < 4; ++i)
	{
		Safe_Release(m_pTextureCom_Diffuse[i]);
		Safe_Release(m_pTextureCom_Normal[i]);
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
