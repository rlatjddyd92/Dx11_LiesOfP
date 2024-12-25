#include "stdafx.h"
#include "Trail_Effect_MP.h"
#include "GameInstance.h"

CTrail_Effect_MP::CTrail_Effect_MP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CTrail_Effect_MP::CTrail_Effect_MP(const CTrail_Effect_MP& Prototype)
	: CEffect_Base(Prototype)
{
}

HRESULT CTrail_Effect_MP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Effect_MP::Initialize(void* pArg)
{
	TRAIL_MP_DESC* pDesc = static_cast<TRAIL_MP_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_DefaultDesc = pDesc->DefaultDesc;
	m_InitDesc = m_DefaultDesc;

	m_eEffectType = TYPE_TRAIL_MP;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
	m_pTransformCom->Set_Scaled(m_DefaultDesc.vScale.x, m_DefaultDesc.vScale.y, m_DefaultDesc.vScale.z);
	m_pTransformCom->Rotation(m_DefaultDesc.vRotation.x, m_DefaultDesc.vRotation.y, m_DefaultDesc.vRotation.z);

	m_iNumTailInstance = m_pVIBufferCom->Get_TailInstance();

	return S_OK;
}

void CTrail_Effect_MP::Priority_Update(_float fTimeDelta)
{
	if (false == m_isActive)
		m_isActive = true;
}

void CTrail_Effect_MP::Update(_float fTimeDelta)
{
	if (false == m_isActive)
		return;

	__super::Set_WorldMatrix();

	// 값 제대로 안채우면 컴퓨트셰이더에서 무한루프 돌아감.
	CTrail_MultiPoint_Instance::TRAIL_MP_MOVEMENT Movement = {};
	Movement.iState = m_DefaultDesc.iComputeState;
	Movement.vPivot = m_DefaultDesc.vPivot;
	Movement.fGravity = m_DefaultDesc.fGravity;
	Movement.vMoveDir = m_DefaultDesc.vMoveDir;
	Movement.vOrbitAxis = m_DefaultDesc.vOrbitAxis;
	Movement.fOrbitAngle = m_DefaultDesc.fOrbitAngle;
	Movement.fTimeInterval = m_DefaultDesc.fRandomTimeInterval;
	Movement.fRandomRatio = m_DefaultDesc.fRandomMoveRatio;
	Movement.fAccelLimit = m_DefaultDesc.fAccelLimit;
	Movement.fAccelSpeed = m_DefaultDesc.fAccelSpeed;
	Movement.fTailInterval = m_DefaultDesc.fTailInterval;
	Movement.fTailDistance = m_DefaultDesc.fTailDistance;
	Movement.vTailMoveDir = m_DefaultDesc.vTailMoveDir;
	Movement.fTimeDelta = fTimeDelta;
	Movement.WorldMatrix = m_WorldMatrix;

	if (7 == m_DefaultDesc.iShaderIndex && MT_FOLLOW == m_DefaultDesc.eType)
		Movement.iState |= CTrail_MultiPoint_Instance::STATE_TRAIL;

	if (true == m_pVIBufferCom->DispatchCS(m_pActionCS, Movement))
	{
		m_isDead = true;
		m_isActive = false;
	}

	if (0.f < m_DefaultDesc.fDuration && m_DefaultDesc.fDuration < m_fAccumulateTime)
		m_DefaultDesc.iComputeState &= ~CVIBuffer_Instancing::STATE_LOOP;

}

void CTrail_Effect_MP::Late_Update(_float fTimeDelta)
{
	if (false == m_isActive)
		return;

	m_fAccumulateTime += fTimeDelta;

	m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_RenderDesc.iRenderGroup, this);
}

HRESULT CTrail_Effect_MP::Render()
{
	_Matrix WorldMatrix = XMMatrixIdentity();
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource_Struct(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextureCom[TEXTURE_MASK_1])
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK_1]->Bind_ShaderResource_Struct(m_pShaderCom, "g_MaskTexture_1", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextureCom[TEXTURE_MASK_2])
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK_2]->Bind_ShaderResource_Struct(m_pShaderCom, "g_MaskTexture_2", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextureCom[TEXTURE_NORMAL])
	{
		if (FAILED(m_pTextureCom[TEXTURE_NORMAL]->Bind_ShaderResource_Struct(m_pShaderCom, "g_NormalTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_DefaultDesc.vTexDevide, sizeof(_Vec2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartScaling", &m_DefaultDesc.vStartScaling, sizeof(_Vec2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vScalingRatio", &m_DefaultDesc.vScalingRatio, sizeof(_Vec2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iState", &m_DefaultDesc.iGeomState, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_DefaultDesc.fRotationPerSecond, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStartRotation", &m_DefaultDesc.fStartRotation, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSpeed", &m_DefaultDesc.fSpriteSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fInterval", &m_DefaultDesc.fTailInterval, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumTailInstance", &m_iNumTailInstance, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_ShaderResource_NonVTX(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_TailBuffer(m_pShaderCom, "Particle_SRV")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Begin(m_DefaultDesc.iShaderIndex)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render_Tail()))
		return E_FAIL;

	return S_OK;
}

void CTrail_Effect_MP::Reset()
{
	m_DefaultDesc = m_InitDesc;
	m_isDead = false;
	m_isActive = false;
	m_fAccumulateTime = 0.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
	m_pTransformCom->Rotation(m_DefaultDesc.vRotation.x, m_DefaultDesc.vRotation.y, m_DefaultDesc.vRotation.z);
	m_pTransformCom->Set_Scaled(m_DefaultDesc.vScale.x, m_DefaultDesc.vScale.y, m_DefaultDesc.vScale.z);

	CTrail_MultiPoint_Instance::TRAIL_MP_MOVEMENT Movement = {};

	m_pVIBufferCom->DispatchCS(m_pResetCS, Movement);
	m_pVIBufferCom->Reset();
}

void CTrail_Effect_MP::Set_Loop(_bool bLoop)
{
	if (true == bLoop)
	{
		m_DefaultDesc.iComputeState |= CVIBuffer_Instancing::STATE_LOOP;
		m_InitDesc.iComputeState |= CVIBuffer_Instancing::STATE_LOOP;
		Reset();
	}
	else
	{
		m_DefaultDesc.iComputeState &= ~CVIBuffer_Instancing::STATE_LOOP;
		m_InitDesc.iComputeState &= ~CVIBuffer_Instancing::STATE_LOOP;
	}
}

HRESULT CTrail_Effect_MP::Ready_Components(const TRAIL_MP_DESC& Desc)
{
	/* FOR.Com_TailShader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_TrailTail_PointInstance"),
		TEXT("Com_TailShader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, Desc.strVIBufferTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	_wstring strComputeShaderTag = TEXT("");
	switch (Desc.DefaultDesc.eType)
	{
	case MT_SPREAD:
		strComputeShaderTag = TEXT("Prototype_Component_Shader_Compute_Trail_Spread");
		break;

	case MT_MOVE:
		strComputeShaderTag = TEXT("Prototype_Component_Shader_Compute_Trail_Move");
		break;

	case MT_CONVERGE:
		strComputeShaderTag = TEXT("Prototype_Component_Shader_Compute_Trail_Converge");
		break;

	case MT_FOLLOW:
		strComputeShaderTag = TEXT("Prototype_Component_Shader_Compute_Trail_Follow");
		break;

	case MT_LOCALSPREAD:
		strComputeShaderTag = TEXT("Prototype_Component_Shader_Compute_Trail_LocalSpread");
		break;

	default:
		return E_FAIL;
	}
	/* FOR.Com_ActionCS */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComputeShaderTag,
		TEXT("Com_ActionCS"), reinterpret_cast<CComponent**>(&m_pActionCS))))
		return E_FAIL;

	/* FOR.Com_ResetCS */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Trail_Reset"),
		TEXT("Com_ResetCS"), reinterpret_cast<CComponent**>(&m_pResetCS))))
		return E_FAIL;


	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szDiffuseTexturTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szDiffuseTexturTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szNormalTextureTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szNormalTextureTag, TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORMAL]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_1))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szMaskTextureTag_1, TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_1]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_2))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szMaskTextureTag_2, TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_2]))))
			return E_FAIL;
	}


	return S_OK;
}

CTrail_Effect_MP* CTrail_Effect_MP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Effect_MP* pInstance = new CTrail_Effect_MP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CTrail_Effect_MP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Effect_MP::Clone(void* pArg)
{
	CTrail_Effect_MP* pInstance = new CTrail_Effect_MP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CTrail_Effect_MP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Effect_MP::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pActionCS);
	Safe_Release(m_pResetCS);

	for (size_t i = 0; i < TEXTURE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

}
