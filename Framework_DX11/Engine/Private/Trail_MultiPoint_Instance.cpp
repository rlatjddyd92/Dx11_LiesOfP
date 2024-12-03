#include "Trail_MultiPoint_Instance.h"
#include "GameInstance.h"

CTrail_MultiPoint_Instance::CTrail_MultiPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing(pDevice, pContext)
{
}

CTrail_MultiPoint_Instance::CTrail_MultiPoint_Instance(const CTrail_MultiPoint_Instance& Prototype)
    : CVIBuffer_Instancing(Prototype)
    , m_iNumTailInstance(Prototype.m_iNumTailInstance)
    , m_vTailSpeed(Prototype.m_vTailSpeed)
    , m_vTail_LifeTime(Prototype.m_vTail_LifeTime)
    , m_HeadBuffer_Desc(Prototype.m_HeadBuffer_Desc)
    , m_TailBuffer_Desc(Prototype.m_TailBuffer_Desc)
    , m_MoveBuffer_Desc(Prototype.m_MoveBuffer_Desc)
    , m_HeadUAV_Desc(Prototype.m_HeadUAV_Desc)
    , m_HeadSRV_Desc(Prototype.m_HeadSRV_Desc)
    , m_TailUAV_Desc(Prototype.m_TailUAV_Desc)
    , m_TailSRV_Desc(Prototype.m_TailSRV_Desc)
    , m_InitHeadBuffer_Desc(Prototype.m_InitHeadBuffer_Desc)
    , m_InitHeadSRV_Desc(Prototype.m_InitHeadSRV_Desc)
    , m_pInstanceTailes(Prototype.m_pInstanceTailes)
    , m_TailInitialData(Prototype.m_TailInitialData)
    , m_InitTailBuffer_Desc(Prototype.m_InitTailBuffer_Desc)
    , m_InitTailSRV_Desc(Prototype.m_InitTailSRV_Desc)
{

}

HRESULT CTrail_MultiPoint_Instance::Initialize_Prototype(const TRAIL_MP_INSTANCE_DESC& Desc, _bool isClient)
{
    m_isClient = isClient;

    if (true == m_isClient)
    {
        if(FAILED(Ready_Buffers(Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CTrail_MultiPoint_Instance::Initialize(void* pArg)
{
    if (false == m_isClient)
    {
        TRAIL_MP_INSTANCE_DESC* pDesc = static_cast<TRAIL_MP_INSTANCE_DESC*>(pArg);
        
        if(FAILED(Ready_Buffers(*pDesc)))
            return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateBuffer(&m_HeadBuffer_Desc, &m_InstanceInitialData, &m_pHeadBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateBuffer(&m_TailBuffer_Desc, &m_TailInitialData, &m_pTailBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateBuffer(&m_MoveBuffer_Desc, nullptr, &m_pMoveBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pHeadBuffer, &m_HeadUAV_Desc, &m_pHeadUAV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pHeadBuffer, &m_HeadSRV_Desc, &m_pHeadSRV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTailBuffer, &m_TailUAV_Desc, &m_pTailUAV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTailBuffer, &m_TailSRV_Desc, &m_pTailSRV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateBuffer(&m_InitHeadBuffer_Desc, &m_InstanceInitialData, &m_pInitHeadBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pInitHeadBuffer, &m_InitHeadSRV_Desc, &m_pInitHeadSRV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateBuffer(&m_InitTailBuffer_Desc, &m_TailInitialData, &m_pInitTailBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pInitTailBuffer, &m_InitTailSRV_Desc, &m_pInitTailSRV)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrail_MultiPoint_Instance::Bind_HeadBuffer(CShader_NonVTX* pShader, const _char* pConstantName)
{
    //// SRV 바인딩하고
    if (FAILED(pShader->Bind_SRV(pConstantName, m_pHeadSRV)))
        return E_FAIL;

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    m_pContext->IASetInputLayout(nullptr);

    return S_OK;
}

HRESULT CTrail_MultiPoint_Instance::Bind_TailBuffer(CShader_NonVTX* pShader, const _char* pConstantName)
{
    //// SRV 바인딩하고
    if (FAILED(pShader->Bind_SRV(pConstantName, m_pTailSRV)))
        return E_FAIL;

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    m_pContext->IASetInputLayout(nullptr);

    return S_OK;
}

HRESULT CTrail_MultiPoint_Instance::Render_Head()
{
    m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

    // UAV 해제하고
    ID3D11ShaderResourceView* nullSRV = { nullptr };
    m_pContext->VSSetShaderResources(0, 1, &nullSRV);

    return S_OK;
}

HRESULT CTrail_MultiPoint_Instance::Render_Tail()
{
    m_pContext->DrawInstanced(1, m_iNumInstance * m_iNumTailInstance, 0, 0);

    // UAV 해제하고
    ID3D11ShaderResourceView* nullSRV = { nullptr };
    m_pContext->VSSetShaderResources(0, 1, &nullSRV);

    return S_OK;
}

_bool CTrail_MultiPoint_Instance::DispatchCS(CShader_Compute* pComputeShader, const TRAIL_MP_MOVEMENT& MovementData)
{	
    /* 순서 중요 !! */
    // 상수 버퍼 업데이트 하고
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if (FAILED(m_pContext->Map(m_pMoveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
        return true;

    TRAIL_MP_MOVEMENT* pMovement = static_cast<TRAIL_MP_MOVEMENT*>(mappedResource.pData);
    *pMovement = MovementData;
    (*pMovement).iNumInstance = m_iNumInstance;
    (*pMovement).iNumTailInstance = m_iNumTailInstance;
    if (false == m_bFirst)
    {
        pMovement->fPadding = { 0.f };
        m_bFirst = true;
    }
    else
    {
        pMovement->fPadding = { 1.f };
    }

    

    m_pContext->Unmap(m_pMoveBuffer, 0);

    m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pHeadUAV, nullptr);
    m_pContext->CSSetUnorderedAccessViews(1, 1, &m_pTailUAV, nullptr);
    m_pContext->CSSetShaderResources(0, 1, &m_pInitHeadSRV);
    m_pContext->CSSetShaderResources(1, 1, &m_pInitTailSRV);
    m_pContext->CSSetConstantBuffers(0, 1, &m_pMoveBuffer);

    pComputeShader->Execute_ComputeShader((m_iNumInstance * m_iNumTailInstance + 255) / 256, 1, 1);

    // UAV 해제
    ID3D11UnorderedAccessView* nullUAV[2] = { nullptr, nullptr };
    m_pContext->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);

    // SRV 해제
    ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
    m_pContext->CSSetShaderResources(0, 2, nullSRV);

    // Compute Shader 해제
    m_pContext->CSSetShader(nullptr, nullptr, 0);

    if (false == (STATE_LOOP & MovementData.iState))
    {
        m_fTime += MovementData.fTimeDelta;
        if (m_vTail_LifeTime.y + m_vLifeTime.y < m_fTime)
            return true;
    }

    return false;
}

void CTrail_MultiPoint_Instance::Reset()
{
    m_bFirst = false;
    m_fTime = 0.f;
}

HRESULT CTrail_MultiPoint_Instance::Ready_Buffers(const TRAIL_MP_INSTANCE_DESC& Desc)
{
    if (FAILED(__super::Initialize_Desc(Desc)))
        return E_FAIL;
    m_iNumTailInstance = Desc.iTail_NumInstance;
    m_vTailSpeed = Desc.vTail_Speed;
    m_vTail_Size = Desc.vTail_Size;
    m_vTail_LifeTime = Desc.vTail_LifeTime;
    m_vTail_MinColor = Desc.vTail_MinColor;
    m_vTail_MaxColor = Desc.vTail_MaxColor;

    m_pInstanceVertices = new HEAD_PARTICLE[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(HEAD_PARTICLE) * m_iNumInstance);

    HEAD_PARTICLE* pParticle = static_cast<HEAD_PARTICLE*>(m_pInstanceVertices);
    Init_HeadParticle(pParticle);

    ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
    m_InstanceInitialData.pSysMem = m_pInstanceVertices;

    m_pInstanceTailes = new TAIL_PARTICLE[m_iNumInstance * m_iNumTailInstance];
    TAIL_PARTICLE* pTailParticle = static_cast<TAIL_PARTICLE*>(m_pInstanceTailes);
    Init_TailParticle(pTailParticle);

    ZeroMemory(&m_TailInitialData, sizeof m_TailInitialData);
    m_TailInitialData.pSysMem = m_pInstanceTailes;

    m_HeadBuffer_Desc = {};
    m_HeadBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_HeadBuffer_Desc.ByteWidth = sizeof(HEAD_PARTICLE) * m_iNumInstance;
    m_HeadBuffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    m_HeadBuffer_Desc.StructureByteStride = sizeof(HEAD_PARTICLE);
    m_HeadBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_TailBuffer_Desc = {};
    m_TailBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_TailBuffer_Desc.ByteWidth = sizeof(TAIL_PARTICLE) * m_iNumInstance * m_iNumTailInstance;
    m_TailBuffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    m_TailBuffer_Desc.StructureByteStride = sizeof(TAIL_PARTICLE);
    m_TailBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_MoveBuffer_Desc = {};
    m_MoveBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
    m_MoveBuffer_Desc.ByteWidth = sizeof(TRAIL_MP_MOVEMENT);
    m_MoveBuffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_MoveBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    m_HeadUAV_Desc = {};
    m_HeadUAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_HeadUAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    m_HeadUAV_Desc.Buffer.NumElements = m_iNumInstance;

    m_HeadSRV_Desc = {};
    m_HeadSRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_HeadSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    m_HeadSRV_Desc.Buffer.NumElements = m_iNumInstance;

    m_TailUAV_Desc = {};
    m_TailUAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_TailUAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    m_TailUAV_Desc.Buffer.NumElements = m_iNumInstance * m_iNumTailInstance;;

    m_TailSRV_Desc = {};
    m_TailSRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_TailSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    m_TailSRV_Desc.Buffer.NumElements = m_iNumInstance * m_iNumTailInstance;;

    m_InitHeadBuffer_Desc = {};
    m_InitHeadBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_InitHeadBuffer_Desc.ByteWidth = sizeof(HEAD_PARTICLE) * m_iNumInstance;
    m_InitHeadBuffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    m_InitHeadBuffer_Desc.StructureByteStride = sizeof(HEAD_PARTICLE);
    m_InitHeadBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_InitHeadSRV_Desc = {};
    m_InitHeadSRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_InitHeadSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    m_InitHeadSRV_Desc.Buffer.NumElements = m_iNumInstance;

    m_InitTailBuffer_Desc = {};
    m_InitTailBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_InitTailBuffer_Desc.ByteWidth = sizeof(TAIL_PARTICLE) * m_iNumInstance * m_iNumTailInstance;
    m_InitTailBuffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    m_InitTailBuffer_Desc.StructureByteStride = sizeof(TAIL_PARTICLE);
    m_InitTailBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_InitTailSRV_Desc = {};
    m_InitTailSRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_InitTailSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    m_InitTailSRV_Desc.Buffer.NumElements = m_iNumInstance * m_iNumTailInstance;

    return S_OK;
}

HRESULT CTrail_MultiPoint_Instance::Init_HeadParticle(HEAD_PARTICLE* pParticles)
{
    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        pParticles[i].Particle.vPosition = _float3(0.f, 0.f, 0.f);

        _float fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);
        pParticles[i].Particle.vPSize = _float2(fScale, fScale);

        pParticles[i].Particle.vRight = _float4(1.f, 0.f, 0.f, 0.f);
        pParticles[i].Particle.vUp = _float4(0.f, 1.f, 0.f, 0.f);
        pParticles[i].Particle.vLook = _float4(0.f, 0.f, 1.f, 0.f);

        pParticles[i].Particle.vTranslation = Get_ExceptedTranslation();
        pParticles[i].Particle.vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
        pParticles[i].Particle.vColor = _float4(m_pGameInstance->Get_Random(m_vMinColor.x, m_vMaxColor.x),
            m_pGameInstance->Get_Random(m_vMinColor.y, m_vMaxColor.y),
            m_pGameInstance->Get_Random(m_vMinColor.z, m_vMaxColor.z),
            m_pGameInstance->Get_Random(m_vMinColor.w, m_vMaxColor.w));

        pParticles[i].Particle.fSpeed = m_pGameInstance->Get_Random(m_vSpeed.x, m_vSpeed.y);

        pParticles[i].Particle.vCurrenrRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
        pParticles[i].Particle.vNextRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);

        pParticles[i].vPreTranslation = {};
        pParticles[i].vCurrentRandomPos = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
        pParticles[i].vNextRandomPos = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
        pParticles[i].iTailInitIndex = i * m_iNumTailInstance;
    }

    return S_OK;
}

HRESULT CTrail_MultiPoint_Instance::Init_TailParticle(TAIL_PARTICLE* pParticles)
{
    for (_uint i = 0; i < m_iNumInstance * m_iNumTailInstance; ++i)
    {
        pParticles[i].Particle.vPosition = _float3(0.f, 0.f, 0.f);

        _float fScale = m_pGameInstance->Get_Random(m_vTail_Size.x, m_vTail_Size.y);
        pParticles[i].Particle.vPSize = _float2(fScale, fScale);

        pParticles[i].Particle.vRight = _float4(1.f, 0.f, 0.f, 0.f);
        pParticles[i].Particle.vUp = _float4(0.f, 1.f, 0.f, 0.f);
        pParticles[i].Particle.vLook = _float4(0.f, 0.f, 1.f, 0.f);
        pParticles[i].Particle.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
        pParticles[i].Particle.vLifeTime = _float2(m_pGameInstance->Get_Random(m_vTail_LifeTime.x, m_vTail_LifeTime.y), 0.0f);
        pParticles[i].Particle.vLifeTime.y = pParticles[i].Particle.vLifeTime.x;
        pParticles[i].Particle.vColor = _float4(m_pGameInstance->Get_Random(m_vTail_MinColor.x, m_vTail_MaxColor.x),
            m_pGameInstance->Get_Random(m_vTail_MinColor.y, m_vTail_MaxColor.y),
            m_pGameInstance->Get_Random(m_vTail_MinColor.z, m_vTail_MaxColor.z),
            m_pGameInstance->Get_Random(m_vTail_MinColor.w, m_vTail_MaxColor.w));
        pParticles[i].Particle.fSpeed = m_pGameInstance->Get_Random(m_vTailSpeed.x, m_vTailSpeed.y);
        pParticles[i].Particle.vCurrenrRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
        pParticles[i].Particle.vNextRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
        pParticles[i].vMoveDir = _float4(0.f, 0.f, 0.f, 0.f);

    }
    return S_OK;
}

CTrail_MultiPoint_Instance* CTrail_MultiPoint_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_MP_INSTANCE_DESC& Desc, _bool isClient)
{
    CTrail_MultiPoint_Instance* pInstance = new CTrail_MultiPoint_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(Desc, isClient)))
    {
        MSG_BOX(TEXT("Create Failed : CTrail_MultiPoint_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTrail_MultiPoint_Instance::Clone(void* pArg)
{
    CTrail_MultiPoint_Instance* pInstance = new CTrail_MultiPoint_Instance(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CTrail_MultiPoint_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrail_MultiPoint_Instance::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        Safe_Release(m_pHeadBuffer);
        Safe_Release(m_pHeadUAV);
        Safe_Release(m_pHeadSRV);

        Safe_Release(m_pTailBuffer);
        Safe_Release(m_pTailUAV);
        Safe_Release(m_pTailSRV);

        Safe_Release(m_pMoveBuffer);

        Safe_Release(m_pInitHeadBuffer);
        Safe_Release(m_pInitHeadSRV);

        Safe_Release(m_pInitTailBuffer);
        Safe_Release(m_pInitTailSRV);

#pragma region TOOL
        if (false == m_isClient)
        {
            Safe_Delete_Array(m_pInstanceTailes);
        }
#pragma endregion
    }
#pragma region CLIENT
    if (true == m_isClient && false == m_isCloned)
    {
        Safe_Delete_Array(m_pInstanceTailes);
    }
#pragma endregion 


}
