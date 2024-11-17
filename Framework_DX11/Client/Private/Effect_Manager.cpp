#include "stdafx.h"
#include "Effect_Manager.h"
#include "GameInstance.h"

#include <fstream>

#include "Effect_Container.h"

#define CREATE_CONTAINER(pArg) static_cast<CEffect_Container*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Container"), pArg));


IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strEffectPath)
{
    m_pDevice = pDevice;
    Safe_AddRef(m_pDevice);

    m_pContext = pContext;
    Safe_AddRef(m_pContext);

    if(FAILED(Load_Effects(strEffectPath)))
        return E_FAIL;

    if(FAILED(Load_Textures()))
        return E_FAIL;

    if(FAILED(Load_EffectContainers(strEffectPath)))
        return E_FAIL;

    if(FAILED(Load_Models()))
        return E_FAIL;

    if(FAILED(Load_Shaders()))
        return E_FAIL;

    if(FAILED(Load_Objects()))
        return E_FAIL;

    return S_OK;
}

CEffect_Container* CEffect_Manager::Clone_Effect(const _wstring& strECTag, void* pArg)
{
    CEffect_Container* pEffectContainer = CREATE_CONTAINER(pArg);

    auto& iter = m_EffectContainers.find(strECTag);

    for (auto& strEffectTag : iter->second)
    {
        _wstring strFileExtention = Get_FileExtentin(strEffectTag);   // 확장자
        _wstring strEffectName = Get_FileName(strEffectTag);

        if (TEXT("PE") == strFileExtention)
        {
            pEffectContainer->Add_Effect(Clone_ParticleEffect(strEffectName));
        }
        else if (TEXT("TE") == strFileExtention)
        {
            pEffectContainer->Add_Effect(Clone_TextureEffect(strEffectName));
        }
        else if (TEXT("ME") == strFileExtention)
        {
            pEffectContainer->Add_Effect(Clone_MeshEffect(strEffectName));
        }
    }

    return pEffectContainer;
}

HRESULT CEffect_Manager::Add_Effect_ToLayer(_uint iLevelID, const _wstring& strECTag, void* pArg)
{
    CEffect_Container* pEffectContainer = CREATE_CONTAINER(pArg);

    auto& iter = m_EffectContainers.find(strECTag);

    for (auto& strEffectTag : iter->second)
    {
        _wstring strFileExtention = Get_FileExtentin(strEffectTag);   // 확장자
        _wstring strEffectName = Get_FileName(strEffectTag);

        if (TEXT("PE") == strFileExtention)
        {
            pEffectContainer->Add_Effect(Clone_ParticleEffect(strEffectName));
        }
        else if (TEXT("TE") == strFileExtention)
        {
            pEffectContainer->Add_Effect(Clone_TextureEffect(strEffectName));
        }
        else if (TEXT("ME") == strFileExtention)
        {
            pEffectContainer->Add_Effect(Clone_MeshEffect(strEffectName));
        }
    }

    if(FAILED(m_pGameInstance->Add_Object_ToLayer(iLevelID, TEXT("Layer_Effect"), pEffectContainer)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Manager::Load_Effects(const _wstring& strEffectPath)
{
    _wstring searchPath = strEffectPath + L"\\*.*";
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        MSG_BOX(TEXT("Folder Open Failed"));
        return E_FAIL;
    }

    while (FindNextFileW(hFind, &findFileData))
    {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
        {
            _wstring strFileName = findFileData.cFileName;   // 파일 이름 + 확장자
            _wstring strFileExtention = Get_FileExtentin(strFileName);   // 확장자

            _wstring strResultPath = strEffectPath + TEXT('/') + strFileName;

            if (TEXT("PE") == strFileExtention)
            {
                if (FAILED(Load_Particle_Effect(strResultPath)))
                    return E_FAIL;
            }
            else if (TEXT("TE") == strFileExtention)
            {
                if (FAILED(Load_Texture_Effect(strResultPath)))
                    return E_FAIL;
            }
            else if (TEXT("ME") == strFileExtention)
            {
                if (FAILED(Load_Mesh_Effect(strResultPath)))
                    return E_FAIL;
            }

        }
    }
    // 핸들 닫기
    FindClose(hFind);

	return S_OK;
}

HRESULT CEffect_Manager::Load_Textures()
{
    /* For. Prototype_Component_Texture_DefaultBlack */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DefaultBlack"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Sample_Black.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Particle_Spark */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Spark"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle_Spark.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Glow */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Glow"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Glow_0.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Thunder */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Thunder"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Thunder_02_C_HJS.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_LensFlare */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LensFlare"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_LensFlare_01_C_KMH.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Ring */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ring_4"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Ring_04_C_KMH.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Ring_2 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ring_8"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Ring_08_C_HJS.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Tile_Spark */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile_Spark"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Tile_Spark_01_C_HJS.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Stone_8x8 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Stone_8x8"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SubUV_DebStone_03_8x8_SC_KMH.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Thunder_4x4 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Thunder_4x4"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SubUV_Thunder_01_4x4_SC_HJS.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Thunder_4x1 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Thunder_4x1"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SubUV_Thunder_01_4x1_SC_GDH.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Test_Trail */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Test_Trail"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_TrailGrad_01_C_RSW.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_DefaultNormal */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DefaultNormal"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/default_normal.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Stone_8x8_Normal */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Stone_8x8_Normal"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SubUV_DebStone_03_8x8_SN_KMH.dds"), 1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Manager::Load_Models()
{
    _matrix		PreTransformMatrix = XMMatrixIdentity();

    /* For. Prototype_Component_Model_HalfSphere_1 */
    PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationX(XMConvertToRadians(90.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HalfSphere_1"),
        CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/SM_HalfSphere_01_GDH.dat", PreTransformMatrix))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Manager::Load_Shaders()
{
    /* For. Prototype_Component_Shader_Effect_Texture */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Texture"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect_Texture.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Effect_Mesh */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Mesh"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect_Mesh.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_VtxPointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        CShader_NonVTX::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl")))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Trail_OnePoint_Instance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail_OnePoint_Instance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrail_OnePoint_Instance.hlsl"), VTXTRAIL_ONEPOINT_INSTANCE::Elements, VTXTRAIL_ONEPOINT_INSTANCE::iNumElements))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Trail_TwoPoint_Instance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail_TwoPoint_Instance"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrail_TwoPoint_Instance.hlsl"), VTXTRAIL_TWOPOINT_INSTANCE::Elements, VTXTRAIL_TWOPOINT_INSTANCE::iNumElements))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_TrailTail_PointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_TrailTail_PointInstance"),
        CShader_NonVTX::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_TrailTail_PointInstance.hlsl")))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_TrailHead_PointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_TrailHead_PointInstance"),
        CShader_NonVTX::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_TrailHead_PointInstance.hlsl")))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Particle_Spread */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_Spread"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleCompute.hlsl"), "CS_SPREAD_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Particle_Move */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_Move"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleCompute.hlsl"), "CS_MOVE_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Particle_Converge */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_Converge"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleCompute.hlsl"), "CS_CONVERGE_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Particle_Reset */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_Reset"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleCompute.hlsl"), "CS_RESET_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Trail_Spread */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Trail_Spread"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail_MultiPoint_Compute.hlsl"), "CS_SPREAD_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Trail_Move */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Trail_Move"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail_MultiPoint_Compute.hlsl"), "CS_MOVE_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Trail_Converge */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Trail_Converge"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail_MultiPoint_Compute.hlsl"), "CS_CONVERGE_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Trail_Reset */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Trail_Reset"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail_MultiPoint_Compute.hlsl"), "CS_RESET_MAIN"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Manager::Load_Objects()
{
    /* For. Prototype_GameObject_Effect_Container */
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Container"),
        CEffect_Container::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Effect_Particle */
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle"),
        CParticle_Effect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Effect_Texture */
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Texture"),
        CTexture_Effect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Effect_Mesh */
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Mesh"),
        CMesh_Effect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Manager::Load_EffectContainers(const _wstring& strEffectPath)
{
    _wstring ContainerPath = strEffectPath + TEXT("\\EffectContainer");
    _wstring searchPath = strEffectPath + TEXT("\\EffectContainer\\*.*");
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        MSG_BOX(TEXT("Folder Open Failed"));
        return E_FAIL;
    }

    while (FindNextFileW(hFind, &findFileData))
    {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            _wstring strFileName = findFileData.cFileName;   // 파일 이름 + 확장자
            _wstring strFileExtention = Get_FileExtentin(strFileName);   // 확장자

            _wstring strResultPath = ContainerPath + TEXT("\\") + strFileName;

            if (TEXT("EC") == strFileExtention)
            {
                std::ifstream infile(strResultPath, ios::binary);


                if (!infile.is_open())
                    return E_FAIL;

                vector<_wstring> EffectNames;

                while (true)
                {
                    _tchar szEffectName[MAX_PATH] = TEXT("");
                    infile.read(reinterpret_cast<_char*>(szEffectName), MAX_PATH * sizeof(_tchar));

                    if (true == infile.eof())
                        break;

                    EffectNames.emplace_back(szEffectName);
                }

                infile.close();

                size_t dotPos = strFileName.rfind(L'.');
                strFileName = strFileName.substr(0, dotPos);

                m_EffectContainers.emplace(strFileName, EffectNames);
                EffectNames.clear();
            }
        }
    }
    // 핸들 닫기
    FindClose(hFind);

    return S_OK;

}

HRESULT CEffect_Manager::Load_Particle_Effect(const _wstring& strResultPath)
{
    CParticle_Effect::PARTICLE_EFFECT_DESC TestDesc = {};

    std::ifstream infile(strResultPath, ios::binary);

    if (!infile.is_open())
        return E_FAIL;

    TestDesc.pParentMatrix = { nullptr };
    TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
    TestDesc.fSpeedPerSec = 1.f;
    TestDesc.iLevelIndex = LEVEL_GAMEPLAY;

    _tchar strEffectName[MAX_PATH] = TEXT("");
    CVIBuffer_Point_Instance::INSTANCE_DESC InstanceDesc = {};

    infile.read(reinterpret_cast<_char*>(strEffectName), sizeof(strEffectName));	// 이거 키로 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
    infile.read(reinterpret_cast<_char*>(&InstanceDesc), sizeof(InstanceDesc));		// 이거 버퍼 초기화에 쓰고

    infile.close();

    _wstring strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Point_Instance_");
    strVIBufferTag += strEffectName;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, strVIBufferTag,CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, InstanceDesc, true))))
        return E_FAIL;

    TestDesc.strVIBufferTag = strVIBufferTag;

    m_PEDescs.emplace(strEffectName, TestDesc);

    return S_OK;
}

HRESULT CEffect_Manager::Load_Texture_Effect(const _wstring& strResultPath)
{
    CTexture_Effect::TEXTURE_EFFECT_DESC TestDesc = {};

    ifstream infile(strResultPath, ios::binary);

    if (!infile.is_open())
        return E_FAIL;

    TestDesc.pParentMatrix = { nullptr };
    TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
    TestDesc.fSpeedPerSec = 1.f;
    TestDesc.iLevelIndex = LEVEL_GAMEPLAY;

    _tchar strEffectName[MAX_PATH] = TEXT("");

    infile.read(reinterpret_cast<_char*>(strEffectName), sizeof(strEffectName));	// 이거 키로 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 텍스처 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));		// 이거는 그대로 가져다 쓰고

    infile.close();

    m_TEDescs.emplace(strEffectName, TestDesc);

    return S_OK;
}

HRESULT CEffect_Manager::Load_Mesh_Effect(const _wstring& strResultPath)
{
    CMesh_Effect::MESH_EFFECT_DESC TestDesc = {};

    ifstream infile(strResultPath, ios::binary);

    if (!infile.is_open())
        return E_FAIL;

    TestDesc.pParentMatrix = { nullptr };
    TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
    TestDesc.fSpeedPerSec = 1.f;
    TestDesc.iLevelIndex = LEVEL_GAMEPLAY;

    _tchar strEffectName[MAX_PATH] = TEXT("");

    infile.read(reinterpret_cast<_char*>(strEffectName), sizeof(strEffectName));	// 이거 키로 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 텍스처 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));		// 이거는 그대로 가져다 쓰고

    infile.close();

    m_MEDescs.emplace(strEffectName, TestDesc);

    return S_OK;
}

CParticle_Effect* CEffect_Manager::Clone_ParticleEffect(const _wstring& strEffectTag)
{
    CParticle_Effect::PARTICLE_EFFECT_DESC ParticleDesc = {};

    auto& iter = m_PEDescs.find(strEffectTag);

    if (m_PEDescs.end() == iter)
        return nullptr;

    ParticleDesc = iter->second;
    CParticle_Effect* pParticleEffect = static_cast<CParticle_Effect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Particle"), &ParticleDesc));

    return pParticleEffect;
}

CTexture_Effect* CEffect_Manager::Clone_TextureEffect(const _wstring& strEffectTag)
{
    CTexture_Effect::TEXTURE_EFFECT_DESC TextureDesc = {};

    auto& iter = m_TEDescs.find(strEffectTag);

    if (m_TEDescs.end() == iter)
        return nullptr;

    TextureDesc = iter->second;
    CTexture_Effect* pTextureEffect = static_cast<CTexture_Effect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Texture"), &TextureDesc));

    return pTextureEffect;
}

CMesh_Effect* CEffect_Manager::Clone_MeshEffect(const _wstring& strEffectTag)
{
    CMesh_Effect::MESH_EFFECT_DESC MeshDesc = {};

    auto& iter = m_MEDescs.find(strEffectTag);

    if (m_MEDescs.end() == iter)
        return nullptr;

    MeshDesc = iter->second;
    CMesh_Effect* pMeshEffect = static_cast<CMesh_Effect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Mesh"), &MeshDesc));

    return pMeshEffect;
}

_wstring CEffect_Manager::Get_FileName(const _wstring& strFileTag)
{
    size_t dotPos = strFileTag.rfind(L'.');
    if (dotPos == _wstring::npos) {
        // 확장자가 없으면 원본 반환
        return strFileTag;
    }
    return strFileTag.substr(0, dotPos);
}

_wstring CEffect_Manager::Get_FileExtentin(const _wstring& strFileTag)
{
    size_t dotPos = strFileTag.find_last_of(L'.');
    if (dotPos == _wstring::npos) {
        return L""; // 확장자가 없으면 빈 문자열 반환
    }

    _wstring strExtention = strFileTag.substr(dotPos + 1);

    size_t nullPos = strExtention.find(L'\0');
    if (nullPos != _wstring::npos) {
        strExtention.erase(nullPos);
    }

    return strExtention;
}

void CEffect_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
