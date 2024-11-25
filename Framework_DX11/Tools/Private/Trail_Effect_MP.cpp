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

	m_eEffectType = TYPE_TRAIL_MP;

	m_InitDesc = *pDesc;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
	m_pTransformCom->Set_Scaled(m_DefaultDesc.vScale.x, m_DefaultDesc.vScale.y, m_DefaultDesc.vScale.z);
	m_pTransformCom->Rotation(m_DefaultDesc.vRotation.x, m_DefaultDesc.vRotation.y, m_DefaultDesc.vRotation.z);

	return S_OK;
}

void CTrail_Effect_MP::Priority_Update(_float fTimeDelta)
{
}

void CTrail_Effect_MP::Update(_float fTimeDelta)
{
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


	_bool bOver = { false };

	switch (m_DefaultDesc.eType)
	{
	case MT_SPREAD:
		bOver = m_pVIBufferCom->DispatchCS(m_pSpreadCS, Movement);
		break;

	case MT_MOVE:
		bOver = m_pVIBufferCom->DispatchCS(m_pMoveCS, Movement);
		break;

	case MT_CONVERGE:
		bOver = m_pVIBufferCom->DispatchCS(m_pConvergeCS, Movement);
		break;

	case MT_FOLLOW:
		bOver = m_pVIBufferCom->DispatchCS(m_pFollowCS, Movement);
		break;
	}

	if (true == bOver)
	{
		m_isActive = false;
	}
}

void CTrail_Effect_MP::Late_Update(_float fTimeDelta)
{
	if (CRenderer::RG_END <= m_RenderDesc.iRenderGroup)
		return;
	m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_RenderDesc.iRenderGroup, this);
}

HRESULT CTrail_Effect_MP::Render()
{
	_Matrix WorldMatrix = XMMatrixIdentity();

	// 꼬리
	WorldMatrix = XMMatrixIdentity();

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
	m_DefaultDesc = m_InitDesc.DefaultDesc;
	m_isActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
	m_pTransformCom->Rotation(m_DefaultDesc.vRotation.x, m_DefaultDesc.vRotation.y, m_DefaultDesc.vRotation.z);
	m_pTransformCom->Set_Scaled(m_DefaultDesc.vScale.x, m_DefaultDesc.vScale.y, m_DefaultDesc.vScale.z);

	CTrail_MultiPoint_Instance::TRAIL_MP_MOVEMENT Movement = {};

	m_pVIBufferCom->DispatchCS(m_pResetCS, Movement);
	m_pVIBufferCom->Reset();

}

HRESULT CTrail_Effect_MP::Save(_wstring strFilePath)
{
	if (strFilePath.back() == L'\0')
		strFilePath.resize(strFilePath.size() - 1);

	_wstring strResultPath = strFilePath + TEXT("\\") + m_strEffectName + TEXT(".TMP");

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
	outfile.write(reinterpret_cast<const _char*>(&m_InitDesc.BufferDesc), sizeof(m_InitDesc.BufferDesc));

	outfile.close();

	return S_OK;

	return S_OK;
}

void CTrail_Effect_MP::Set_Loop(_bool bLoop)
{
	if (true == bLoop)
	{
		m_DefaultDesc.iComputeState |= CVIBuffer_Instancing::STATE_LOOP;
		Reset();
	}
	else
		m_DefaultDesc.iComputeState &= ~CVIBuffer_Instancing::STATE_LOOP;
}

void CTrail_Effect_MP::Set_Desc(TRAIL_MP_DESC Desc)
{
	m_DefaultDesc = Desc.DefaultDesc;
	m_RenderDesc = Desc.RenderDesc;

	m_InitDesc.DefaultDesc = Desc.DefaultDesc;
	m_InitDesc.RenderDesc = Desc.RenderDesc;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
	m_pTransformCom->Set_Scaled(m_DefaultDesc.vScale.x, m_DefaultDesc.vScale.y, m_DefaultDesc.vScale.z);
	m_pTransformCom->Rotation(m_DefaultDesc.vRotation.x, m_DefaultDesc.vRotation.y, m_DefaultDesc.vRotation.z);
	Reset();
}

HRESULT CTrail_Effect_MP::Ready_Components(const TRAIL_MP_DESC& Desc)
{
	/* FOR.Com_TailShader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_TrailTail_PointInstance"),
		TEXT("Com_TailShader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CTrail_MultiPoint_Instance::TRAIL_MP_INSTANCE_DESC desc = {};

	desc.iNumInstance = Desc.BufferDesc.iNumInstance;
	desc.vCenter = Desc.BufferDesc.vCenter;
	desc.vRange = Desc.BufferDesc.vRange;
	desc.vExceptRange = Desc.BufferDesc.vExceptRange;
	desc.vLifeTime = Desc.BufferDesc.vLifeTime;
	desc.vMaxColor = Desc.BufferDesc.vMaxColor;
	desc.vMinColor = Desc.BufferDesc.vMinColor;
	desc.vSize = Desc.BufferDesc.vSize;
	desc.vSpeed = Desc.BufferDesc.vSpeed;
	desc.iTail_NumInstance = Desc.BufferDesc.iTail_NumInstance;
	desc.vTail_Speed = Desc.BufferDesc.vTail_Speed;
	desc.vTail_Size = Desc.BufferDesc.vTail_Size;
	desc.vTail_LifeTime = Desc.BufferDesc.vTail_LifeTime;
	desc.vTail_MinColor = Desc.BufferDesc.vTail_MinColor;
	desc.vTail_MaxColor = Desc.BufferDesc.vTail_MaxColor;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Trail_MultiPoint_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &desc)))
		return E_FAIL;

	/* FOR.Com_SpreadCS */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Trail_Spread"),
		TEXT("Com_SpreadCS"), reinterpret_cast<CComponent**>(&m_pSpreadCS))))
		return E_FAIL;

	/* FOR.Com_MoveCS */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Trail_Move"),
		TEXT("Com_MoveCS"), reinterpret_cast<CComponent**>(&m_pMoveCS))))
		return E_FAIL;

	/* FOR.Com_ConvergeCS */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Trail_Converge"),
		TEXT("Com_ConvergeCS"), reinterpret_cast<CComponent**>(&m_pConvergeCS))))
		return E_FAIL;


	/* FOR.Com_ConvergeCS */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Trail_Follow"),
		TEXT("Com_FollowCS"), reinterpret_cast<CComponent**>(&m_pFollowCS))))
		return E_FAIL;
	
	/* FOR.Com_ResetCS */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Trail_Reset"),
		TEXT("Com_ResetCS"), reinterpret_cast<CComponent**>(&m_pResetCS))))
		return E_FAIL;


	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szDiffuseTexturTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.TextDesc.szDiffuseTexturTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szNormalTextureTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.TextDesc.szNormalTextureTag, TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORMAL]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_1))
	{
		if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.TextDesc.szMaskTextureTag_1, TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_1]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_2))
	{
		if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.TextDesc.szMaskTextureTag_2, TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_2]))))
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

	Safe_Release(m_pSpreadCS);
	Safe_Release(m_pMoveCS);
	Safe_Release(m_pConvergeCS);
	Safe_Release(m_pFollowCS);
	Safe_Release(m_pResetCS);

	for (size_t i = 0; i < TEXTURE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

}
