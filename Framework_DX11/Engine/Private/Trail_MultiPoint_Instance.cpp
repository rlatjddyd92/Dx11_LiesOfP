#include "Trail_MultiPoint_Instance.h"

CTrail_MultiPoint_Instance::CTrail_MultiPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing(pDevice, pContext)
{
}

CTrail_MultiPoint_Instance::CTrail_MultiPoint_Instance(const CTrail_MultiPoint_Instance& Prototype)
    : CVIBuffer_Instancing(Prototype)
    , m_iNumTailInstance( Prototype.m_iNumTailInstance )
    , m_HeadBuffer_Desc(Prototype.m_HeadBuffer_Desc)
    , m_TailBuffer_Desc(Prototype.m_TailBuffer_Desc)
    , m_MoveBuffer_Desc(Prototype.m_MoveBuffer_Desc)
    , m_HeadUAV_Desc(Prototype.m_HeadUAV_Desc)
    , m_TailUAV_Desc(Prototype.m_TailUAV_Desc)
    , m_TailSRV_Desc(Prototype.m_TailSRV_Desc)
    , m_InitHeadBuffer_Desc(Prototype.m_InitHeadBuffer_Desc)
    , m_InitHeadSRV_Desc(Prototype.m_InitHeadSRV_Desc)
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

    if (FAILED(m_pDevice->CreateBuffer(&m_TailBuffer_Desc, nullptr, &m_pTailBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateBuffer(&m_MoveBuffer_Desc, nullptr, &m_pHeadMoveBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateBuffer(&m_MoveBuffer_Desc, nullptr, &m_pTailMoveBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pHeadBuffer, &m_HeadUAV_Desc, &m_pHeadUAV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTailBuffer, &m_TailUAV_Desc, &m_pTailUAV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTailBuffer, &m_TailSRV_Desc, &m_pTailSRV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateBuffer(&m_InitHeadBuffer_Desc, &m_InstanceInitialData, &m_pInitHeadBuffer)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pInitHeadBuffer, &m_InitHeadSRV_Desc, &m_pInitHeadSRV)))
        return E_FAIL;

    return S_OK;
}

void CTrail_MultiPoint_Instance::Reset()
{
}

HRESULT CTrail_MultiPoint_Instance::Ready_Buffers(const TRAIL_MP_INSTANCE_DESC& Desc)
{
    if (FAILED(__super::Initialize_Desc(Desc)))
        return E_FAIL;
    m_iNumTailInstance = Desc.iNumTailInstance;

    m_pInstanceVertices = new PARTICLE[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(PARTICLE) * m_iNumInstance);

    PARTICLE* pParticle = static_cast<PARTICLE*>(m_pInstanceVertices);

    Init_Particle(pParticle);

    m_HeadBuffer_Desc = {};
    m_HeadBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_HeadBuffer_Desc.ByteWidth = sizeof(PARTICLE) * m_iNumInstance;
    m_HeadBuffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    m_HeadBuffer_Desc.StructureByteStride = sizeof(PARTICLE);
    m_HeadBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_TailBuffer_Desc = {};
    m_HeadBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_HeadBuffer_Desc.ByteWidth = sizeof(PARTICLE) * m_iNumTailInstance;
    m_HeadBuffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    m_HeadBuffer_Desc.StructureByteStride = sizeof(PARTICLE);
    m_HeadBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_MoveBuffer_Desc = {};
    m_MoveBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
    m_MoveBuffer_Desc.ByteWidth = sizeof(PARTICLE_MOVEMENT);
    m_MoveBuffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_MoveBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    m_HeadUAV_Desc = {};
    m_HeadUAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_HeadUAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    m_HeadUAV_Desc.Buffer.NumElements = m_iNumInstance;

    m_TailUAV_Desc = {};
    m_TailUAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_TailUAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    m_TailUAV_Desc.Buffer.NumElements = m_iNumTailInstance;

    m_TailSRV_Desc = {};
    m_TailSRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_TailSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    m_TailSRV_Desc.Buffer.NumElements = m_iNumTailInstance;

    m_InitHeadBuffer_Desc = {};
    m_InitHeadBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_InitHeadBuffer_Desc.ByteWidth = sizeof(PARTICLE) * m_iNumInstance;
    m_InitHeadBuffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    m_InitHeadBuffer_Desc.StructureByteStride = sizeof(PARTICLE);
    m_InitHeadBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_InitHeadSRV_Desc = {};
    m_InitHeadSRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
    m_InitHeadSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    m_InitHeadSRV_Desc.Buffer.NumElements = m_iNumTailInstance;

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
}
