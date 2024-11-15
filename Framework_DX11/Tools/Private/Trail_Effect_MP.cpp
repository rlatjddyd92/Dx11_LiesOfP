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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(10.f, 0.f, 10.f));

	return S_OK;
}

void CTrail_Effect_MP::Priority_Update(_float fTimeDelta)
{
}

void CTrail_Effect_MP::Update(_float fTimeDelta)
{
	// 값 제대로 안채우면 컴퓨트셰이더에서 무한루프 돌아감.
	CTrail_MultiPoint_Instance::TRAIL_MP_MOVEMENT Movement = {};
	Movement.iState = 0;// CVIBuffer_Instancing::STATE_LOOP;
	Movement.vPivot = { 0.f, 0.f, 0.f, 1.f };
	Movement.fGravity = 4.f;
	Movement.vMoveDir = { 0.f, 1.f, 0.f, 0.f };
	Movement.fTimeDelta = fTimeDelta;
	Movement.vOrbitAxis = {0.f, 0.f, 0.f};
	Movement.fOrbitAngle = 0.f;
	Movement.fTimeInterval = { 0.25f };
	Movement.fRandomRatio = 0.f;
	Movement.fAccelLimit = 0.f;
	Movement.fAccelSpeed = 0.f;
	Movement.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	Movement.fTailInterval = 0.1f;
	Movement.fTailDistance = 0.25f;
	Movement.vTailMoveDir = _float4(0.f, 1.f, 0.f, 0.f);

	m_pVIBufferCom->DispatchCS(m_pSpreadCS, Movement);
}

void CTrail_Effect_MP::Late_Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	
	if (5.f < m_fTime)
		m_isDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CTrail_Effect_MP::Render()
{
	_Matrix WorldMatrix = XMMatrixIdentity();
	_uint iParticleState = 0;

	//// 대가리
	//if (FAILED(m_pHeadShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
	//	return E_FAIL;

	//if (FAILED(m_pHeadShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pHeadShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;

	//if (FAILED(m_pHeadShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
	//	return E_FAIL;
	//if (FAILED(m_pHeadShaderCom->Bind_RawValue("g_vScaling", &m_vScaling, sizeof(_Vec2))))
	//	return E_FAIL;

	//if (FAILED(m_pHeadShaderCom->Bind_RawValue("g_iState", &iParticleState, sizeof(_uint))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBufferCom->Bind_HeadBuffer(m_pHeadShaderCom, "Particle_SRV")))
	//	return E_FAIL;
	//if (FAILED(m_pHeadShaderCom->Begin(6)))
	//	return E_FAIL;
	//if (FAILED(m_pVIBufferCom->Render_Head()))
	//	return E_FAIL;



	// 꼬리

	WorldMatrix = XMMatrixIdentity();
	if (FAILED(m_pTailShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pTailShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pTailShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTailShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
		return E_FAIL;
	if (FAILED(m_pTailShaderCom->Bind_RawValue("g_vScaling", &m_vScaling, sizeof(_Vec2))))
		return E_FAIL;

	if (FAILED(m_pTailShaderCom->Bind_RawValue("g_iState", &iParticleState, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_TailBuffer(m_pTailShaderCom, "Particle_SRV")))
		return E_FAIL;
	if (FAILED(m_pTailShaderCom->Begin(6)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render_Tail()))
		return E_FAIL;

	return S_OK;
}

void CTrail_Effect_MP::Reset()
{
}

HRESULT CTrail_Effect_MP::Save(_wstring strFilePath)
{
	return S_OK;
}

HRESULT CTrail_Effect_MP::Ready_Components()
{
	/* FOR.Com_HeadShader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_TrailHead_PointInstance"),
		TEXT("Com_HeadShader"), reinterpret_cast<CComponent**>(&m_pHeadShaderCom))))
		return E_FAIL;


	/* FOR.Com_TailShader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_TrailTail_PointInstance"),
		TEXT("Com_TailShader"), reinterpret_cast<CComponent**>(&m_pTailShaderCom))))
		return E_FAIL;

	/* FOR.Com_SpreadCS */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Compute_Trail_Converge"),
		TEXT("Com_SpreadCS"), reinterpret_cast<CComponent**>(&m_pSpreadCS))))
		return E_FAIL;

	CTrail_MultiPoint_Instance::TRAIL_MP_INSTANCE_DESC desc = {};

	desc.iNumInstance		= 100;
	desc.vCenter			= {0.f, 5.f, 0.f};
	desc.vRange				= { 5.f, 2.f, 5.f };
	desc.vExceptRange		= {0.f, 0.f, 0.f};
	desc.vLifeTime			= {0.5f, 1.f};
	desc.vMaxColor			= { 0.f, 0.f, 0.f, 1.f };
	desc.vMinColor			= { 1.f, 1.f, 1.f, 1.f};
	desc.vSize				= {0.2f, 0.4f};
	desc.vSpeed				= {2.f, 4.f};
	desc.iTail_NumInstance	= 100;
	desc.vTail_Speed		= {0.25f, 0.5f};
	desc.vTail_Size			= {0.02f, 0.04f};
	desc.vTail_LifeTime		= {0.5f, 1.f};
	desc.vTail_MinColor		= {0.f, 0.f, 0.f, 1.f};
	desc.vTail_MaxColor		= {1.f, 1.f, 1.f, 1.f};

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Trail_MultiPoint_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &desc)))
		return E_FAIL;


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

	Safe_Release(m_pHeadShaderCom);
	Safe_Release(m_pTailShaderCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pNormalTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pSpreadCS);
	Safe_Release(m_pResetCS);
}
