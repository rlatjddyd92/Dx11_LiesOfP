#include "EasyDrawSphere.h"

#include "GameInstance.h"



CEasyDrawSphere::CEasyDrawSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEasyDrawSphere::CEasyDrawSphere(const CEasyDrawSphere& Prototype)
	: CGameObject{ Prototype }
	,m_pBatch{Prototype.m_pBatch}
	, m_pEffect{Prototype.m_pEffect}
	, m_pInputLayout{Prototype.m_pInputLayout}
	, m_pOriginalBoundingDesc{Prototype.m_pOriginalBoundingDesc }
	, m_pBoundingDesc{Prototype.m_pBoundingDesc}
{
	m_isCloned = true;
}

HRESULT CEasyDrawSphere::Initialize_Prototype()
{
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	
	m_pEffect = new DirectX::BasicEffect(m_pDevice);
	
	m_pEffect->SetVertexColorEnabled(true);
	
	const void* pShaderByteCode = { nullptr };
	size_t			iShaderByteCodeLength = {};
	
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);
	
	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CEasyDrawSphere::Initialize(void* pArg)
{
	EASYDRAWSPHERE_DESC* pDesc = static_cast<EASYDRAWSPHERE_DESC*>(pArg);
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	m_pBoundingDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pOriginalBoundingDesc = new BoundingSphere(*m_pBoundingDesc);
	m_tDesc.pPos = pDesc->pPos;
	m_tDesc.vCenter = pDesc->vCenter;
	m_tDesc.fRadius = pDesc->fRadius;

	if (m_tDesc.pPos == nullptr)
		return E_FAIL;

	return S_OK;
}

void CEasyDrawSphere::Priority_Update(_float fTimeDelta)
{
}

void CEasyDrawSphere::Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_tDesc.pPos != nullptr)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *m_tDesc.pPos);
	}

	m_pOriginalBoundingDesc->Transform(*m_pBoundingDesc, m_pTransformCom->Get_WorldMatrix());
}

void CEasyDrawSphere::Late_Update(_float fTimeDelta)
{
}

HRESULT CEasyDrawSphere::Render()
{

	m_pContext->GSSetShader(nullptr, nullptr, 0);
	
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ));
	
	m_pContext->IASetInputLayout(m_pInputLayout);
	
	m_pEffect->Apply(m_pContext);
	
	m_pBatch->Begin();
	
	DX::Draw(m_pBatch, *m_pBoundingDesc, XMVectorSet(1.f, 1.f, 1.f, 0.1f));
	
	m_pBatch->End();

	return S_OK;
}




void CEasyDrawSphere::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
		Safe_Release(m_pInputLayout);
	}

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pOriginalBoundingDesc);

}
