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

HRESULT CEffect_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strEffectPath, const _wstring strTexturePath, const _wstring strModelPath)
{
    m_pDevice = pDevice;
    Safe_AddRef(m_pDevice);

    m_pContext = pContext;
    Safe_AddRef(m_pContext);

    if(FAILED(Load_Textures(strTexturePath)))
        return E_FAIL;

    if(FAILED(Load_Models(strModelPath)))
        return E_FAIL;

    if(FAILED(Load_Shaders()))
        return E_FAIL;

    if(FAILED(Load_Objects()))
        return E_FAIL;

    if (FAILED(Load_Effects(strEffectPath)))
        return E_FAIL;

    if (FAILED(Load_EffectContainers(strEffectPath)))
        return E_FAIL;

    return S_OK;
}

CEffect_Container* CEffect_Manager::Clone_Effect(const _wstring& strECTag, const _Matrix* pParentMatrix, const _Matrix* pSocketMatrix, _Vec3 vPos, _Vec3 vDir, _Vec3 vScale)
{
    CEffect_Container::EFFECT_DESC desc = {};

    desc.fRotationPerSec = XMConvertToRadians(90.f);
    desc.fSpeedPerSec = 1.f;
    desc.iLevelIndex = LEVEL_GAMEPLAY;
    desc.pParentMatrix = pParentMatrix;
    desc.pSocketMatrix = pSocketMatrix;
    desc.vPos = vPos;
    desc.vDir = vDir;
    desc.vScale = vScale;

    CEffect_Container* pContainer = Find_PoolingEffect(strECTag, &desc);

    pContainer->Set_Loop(false);
    pContainer->Set_Dead(true);
    pContainer->Set_Cloned(true);

    return pContainer;
}


HRESULT CEffect_Manager::Add_Effect_ToLayer(_uint iLevelID, const _wstring& strECTag, _Vec3 vPos, _Vec3 vDir, _Vec3 vScale)
{
    CEffect_Container::EFFECT_DESC desc = {};

    desc.fRotationPerSec = XMConvertToRadians(90.f);
    desc.fSpeedPerSec = 1.f;
    desc.iLevelIndex = iLevelID;
    desc.pParentMatrix = nullptr;
    desc.pSocketMatrix = nullptr;
    desc.vPos = vPos;
    desc.vDir = vDir;
    desc.vScale = vScale;

    return m_pGameInstance->Add_Object_ToLayer(desc.iLevelIndex, TEXT("Layer_Effect"), Find_PoolingEffect(strECTag, &desc));
}

HRESULT CEffect_Manager::Add_Effect_ToLayer(_uint iLevelID, const _wstring& strECTag, const _Matrix* pParentMatrix, const _Matrix* pSocketMatrix, _Vec3 vPos, _Vec3 vDir, _Vec3 vScale)
{
    CEffect_Container::EFFECT_DESC desc = {};

    desc.fRotationPerSec = XMConvertToRadians(90.f);
    desc.fSpeedPerSec = 1.f;
    desc.iLevelIndex = iLevelID;
    desc.pParentMatrix = pParentMatrix;
    desc.pSocketMatrix = pSocketMatrix;
    desc.vPos = vPos;
    desc.vDir = vDir;
    desc.vScale = vScale;

    return m_pGameInstance->Add_Object_ToLayer(desc.iLevelIndex, TEXT("Layer_Effect"), Find_PoolingEffect(strECTag, &desc));
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
            else if (TEXT("TOP") == strFileExtention)
            {
                if (FAILED(Load_TrailOP_Effect(strResultPath)))
                    return E_FAIL;
            }
            else if (TEXT("TTP") == strFileExtention)
            {
                if (FAILED(Load_TrailTP_Effect(strResultPath)))
                    return E_FAIL;
            }
            else if (TEXT("TMP") == strFileExtention)
            {
                if (FAILED(Load_TrailMP_Effect(strResultPath)))
                    return E_FAIL;
            }
        }
    }
    // 핸들 닫기
    FindClose(hFind);

	return S_OK;
}

HRESULT CEffect_Manager::Load_Textures(const _wstring strTexturePath)
{
    _wstring searchPath = strTexturePath + L"\\*.*";
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
            _wstring strFileBaseName = Get_FileName(strFileName);

            _wstring strResultPath = strTexturePath + TEXT('/') + strFileName;

            _wstring strPrototypeTag = TEXT("Prototype_Component_Texture_") + strFileBaseName;

            if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeTag,
                CTexture::Create(m_pDevice, m_pContext, strResultPath.c_str(), 1))))
                return E_FAIL;
        }
    }
    // 핸들 닫기
    FindClose(hFind);

    return S_OK;
}

HRESULT CEffect_Manager::Load_Models(const _wstring& strEffectPath)
{
    _matrix		PreTransformMatrix = XMMatrixIdentity();

    /* For. Prototype_Component_Model_Effect_Lily */
    PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_Lily"),
        CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/Lily.dat", PreTransformMatrix))))
        return E_FAIL;


    _wstring searchPath = strEffectPath + L"\\*.dat";
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
            _wstring strFileBaseName = Remove_FileExtentin(strFileName);

            _wstring strResultPath = strEffectPath + TEXT('/') + strFileName;

            _wstring strPrototypeTag = TEXT("Prototype_Component_Model_Effect_") + strFileBaseName;

            // 변환 후 필요한 버퍼 크기 계산
            int bufferSize = WideCharToMultiByte(CP_UTF8, 0, strResultPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (bufferSize == 0) {
                throw std::runtime_error("WideCharToMultiByte failed.");
            }

            // 변환 결과를 저장할 std::string 생성
            std::string str(bufferSize, '\0');
            WideCharToMultiByte(CP_UTF8, 0, strResultPath.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);

            PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
            if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeTag,
                CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, str.c_str(), PreTransformMatrix))))
                return E_FAIL;
        }
    }
    // 핸들 닫기
    FindClose(hFind);

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
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect_Mesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
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

    /* For. Prototype_Component_Shader_Compute_Particle_LocalSpread */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_LocalSpread"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleCompute.hlsl"), "CS_SPREAD_LOCAL_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Particle_LocalMove */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_LocalMove"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleCompute.hlsl"), "CS_MOVE_LOCAL_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Particle_LocalConverge */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Particle_LocalConverge"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleCompute.hlsl"), "CS_CONVERGE_LOCAL_MAIN"))))
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

    /* For. Prototype_Component_Shader_Compute_Trail_Follow */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Trail_Follow"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail_MultiPoint_Compute.hlsl"), "CS_FOLLOW_MAIN"))))
        return E_FAIL;

    /* For. Prototype_Component_Shader_Compute_Trail_LocalSpread */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Trail_LocalSpread"),
        CShader_Compute::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail_MultiPoint_Compute.hlsl"), "CS_SPREAD_LOCAL_MAIN"))))
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

    /* For. Prototype_GameObject_Effect_Trail_OP */
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Trail_OP"),
        CTrail_Effect_OP::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Effect_Trail_TP */
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Trail_TP"),
        CTrail_Effect_TP::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For. Prototype_GameObject_Effect_Trail_MP */
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Trail_MP"),
        CTrail_Effect_MP::Create(m_pDevice, m_pContext))))
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

                CEffect_Container::EFFECT_DESC desc = {};
                desc.fRotationPerSec = XMConvertToRadians(90.f);
                desc.fSpeedPerSec = 1.f;
                desc.iLevelIndex = LEVEL_GAMEPLAY;
                desc.pParentMatrix = nullptr;
                desc.pSocketMatrix = nullptr;
                desc.vPos = _Vec3(0.f, 0.f, 0.f);
                desc.vDir = _Vec3(0.f, 0.f, 0.f);
                desc.vScale = _Vec3(1.f, 1.f, 1.f);

                Effect_Pooling(strFileName, &desc, 1);

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

HRESULT CEffect_Manager::Load_TrailOP_Effect(const _wstring strResultPath)
{
    CTrail_Effect_OP::TRAIL_OP_DESC TestDesc = {};

    std::ifstream infile(strResultPath, ios::binary);

    if (!infile.is_open())
        return E_FAIL;

    TestDesc.pParentMatrix = { nullptr };
    TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
    TestDesc.fSpeedPerSec = 1.f;
    TestDesc.iLevelIndex = LEVEL_GAMEPLAY;
    
    _tchar strEffectName[MAX_PATH] = TEXT("");
    CTrail_Effect_OP::BUFFER_DESC BufferDesc = {};

    infile.read(reinterpret_cast<_char*>(strEffectName), sizeof(strEffectName));	// 이거 키로 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
    infile.read(reinterpret_cast<_char*>(&BufferDesc), sizeof(BufferDesc));		// 이거 버퍼 초기화에 쓰고

    infile.close();

    _wstring strVIBufferTag = TEXT("Prototype_Component_Trail_OnePoint_Instance_");
    strVIBufferTag += strEffectName;

    CVIBuffer_Instancing::INSTANCE_DESC InstanceDesc = {};

    InstanceDesc.iNumInstance = BufferDesc.iNumInstance;
    InstanceDesc.vLifeTime = BufferDesc.vLifeTime;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, strVIBufferTag, CTrail_OnePoint_Instance::Create(m_pDevice, m_pContext, InstanceDesc, true))))
        return E_FAIL;

    TestDesc.strVIBufferTag = strVIBufferTag;

    m_TOPDescs.emplace(strEffectName, TestDesc);

    return S_OK;
}

HRESULT CEffect_Manager::Load_TrailTP_Effect(const _wstring strResultPath)
{
    CTrail_Effect_TP::TRAIL_TP_DESC TestDesc = {};

    std::ifstream infile(strResultPath, ios::binary);

    if (!infile.is_open())
        return E_FAIL;

    TestDesc.pParentMatrix = { nullptr };
    TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
    TestDesc.fSpeedPerSec = 1.f;
    TestDesc.iLevelIndex = LEVEL_GAMEPLAY;

    _tchar strEffectName[MAX_PATH] = TEXT("");
    CTrail_Effect_TP::BUFFER_DESC BufferDesc = {};

    infile.read(reinterpret_cast<_char*>(strEffectName), sizeof(strEffectName));	// 이거 키로 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
    infile.read(reinterpret_cast<_char*>(&BufferDesc), sizeof(BufferDesc));		// 이거 버퍼 초기화에 쓰고

    infile.close();

    _wstring strVIBufferTag = TEXT("Prototype_Component_Trail_TwoPoint_Instance_");
    strVIBufferTag += strEffectName;

    CVIBuffer_Instancing::INSTANCE_DESC InstanceDesc = {};

    InstanceDesc.iNumInstance = BufferDesc.iNumInstance;
    InstanceDesc.vLifeTime = BufferDesc.vLifeTime;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, strVIBufferTag, CTrail_TwoPoint_Instance::Create(m_pDevice, m_pContext, InstanceDesc, true))))
        return E_FAIL;

    TestDesc.strVIBufferTag = strVIBufferTag;

    m_TTPDescs.emplace(strEffectName, TestDesc);

    return S_OK;
}

HRESULT CEffect_Manager::Load_TrailMP_Effect(const _wstring strResultPath)
{
    CTrail_Effect_MP::TRAIL_MP_DESC TestDesc = {};

    std::ifstream infile(strResultPath, ios::binary);

    if (!infile.is_open())
        return E_FAIL;

    TestDesc.pParentMatrix = { nullptr };
    TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
    TestDesc.fSpeedPerSec = 1.f;
    TestDesc.iLevelIndex = LEVEL_GAMEPLAY;

    _tchar strEffectName[MAX_PATH] = TEXT("");
    CTrail_Effect_MP::BUFFER_DESC BufferDesc = {};

    infile.read(reinterpret_cast<_char*>(strEffectName), sizeof(strEffectName));	// 이거 키로 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
    infile.read(reinterpret_cast<_char*>(&BufferDesc), sizeof(BufferDesc));		// 이거 버퍼 초기화에 쓰고

    infile.close();

    _wstring strVIBufferTag = TEXT("Prototype_Component_Trail_MultiPoint_Instance_");
    strVIBufferTag += strEffectName;

    CTrail_MultiPoint_Instance::TRAIL_MP_INSTANCE_DESC InstanceDesc = {};

    InstanceDesc.iNumInstance = BufferDesc.iNumInstance;
    InstanceDesc.vCenter = BufferDesc.vCenter;
    InstanceDesc.vRange = BufferDesc.vRange;
    InstanceDesc.vExceptRange = BufferDesc.vExceptRange;
    InstanceDesc.vSize = BufferDesc.vSize;
    InstanceDesc.vSpeed = BufferDesc.vSpeed;
    InstanceDesc.vLifeTime = BufferDesc.vLifeTime;
    InstanceDesc.vMinColor = BufferDesc.vMinColor;
    InstanceDesc.vMaxColor = BufferDesc.vMaxColor;
    InstanceDesc.iTail_NumInstance = BufferDesc.iTail_NumInstance;
    InstanceDesc.vTail_Speed = BufferDesc.vTail_Speed;
    InstanceDesc.vTail_Size = BufferDesc.vTail_Size;
    InstanceDesc.vTail_LifeTime = BufferDesc.vTail_LifeTime;
    InstanceDesc.vTail_MinColor = BufferDesc.vTail_MinColor;
    InstanceDesc.vTail_MaxColor = BufferDesc.vTail_MaxColor;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, strVIBufferTag, CTrail_MultiPoint_Instance::Create(m_pDevice, m_pContext, InstanceDesc, true))))
        return E_FAIL;

    TestDesc.strVIBufferTag = strVIBufferTag;

    m_TMPDescs.emplace(strEffectName, TestDesc);

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

CTrail_Effect_OP* CEffect_Manager::Clone_TrailOP_Effect(const _wstring strEffectTag)
{
    CTrail_Effect_OP::TRAIL_OP_DESC TrailOP_Desc = {};

    auto& iter = m_TOPDescs.find(strEffectTag);

    if (m_TOPDescs.end() == iter)
        return nullptr;

    TrailOP_Desc = iter->second;
    CTrail_Effect_OP* pTrailOP_Effect = static_cast<CTrail_Effect_OP*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Trail_OP"), &TrailOP_Desc));

    return pTrailOP_Effect;
}

CTrail_Effect_TP* CEffect_Manager::Clone_TrailTP_Effect(const _wstring strEffectTag)
{
    CTrail_Effect_TP::TRAIL_TP_DESC TrailTP_Desc = {};

    auto& iter = m_TTPDescs.find(strEffectTag);

    if (m_TTPDescs.end() == iter)
        return nullptr;

    TrailTP_Desc = iter->second;
    CTrail_Effect_TP* pTrailTP_Effect = static_cast<CTrail_Effect_TP*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Trail_TP"), &TrailTP_Desc));

    return pTrailTP_Effect;
}

CTrail_Effect_MP* CEffect_Manager::Clone_TrailMP_Effect(const _wstring strEffectTag)
{
    CTrail_Effect_MP::TRAIL_MP_DESC TrailMP_Desc = {};

    auto& iter = m_TMPDescs.find(strEffectTag);

    if (m_TMPDescs.end() == iter)
        return nullptr;

    TrailMP_Desc = iter->second;
    CTrail_Effect_MP* pTrailMP_Effect = static_cast<CTrail_Effect_MP*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Trail_MP"), &TrailMP_Desc));

    return pTrailMP_Effect;
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

_wstring CEffect_Manager::Remove_FileExtentin(const _wstring& strFileTag)
{
    size_t dotPosition = strFileTag.find_last_of('.');
    if (dotPosition != std::string::npos) {
        return strFileTag.substr(0, dotPosition); // 확장자 제거
    }
    return strFileTag; // 확장자가 없으면 그대로 반환
}

CEffect_Container* CEffect_Manager::Find_PoolingEffect(const _wstring& strECTag, void* pArg)
{
    auto& iter = m_EffectPooling.find(strECTag);

    // 아예 처음이면 만들어서 반환.
    if (m_EffectPooling.end() == iter)
    {
        CEffect_Container* pContainer = Clone_Effect_From_Prototype(strECTag, pArg);

        if (nullptr == pContainer)
            return nullptr;

        vector<CEffect_Container*> Containers;
        Containers.emplace_back(pContainer);

        m_EffectPooling.emplace(strECTag, Containers);

        Safe_AddRef(pContainer);
        return pContainer;
    }

    for (auto& pContainer : iter->second)
    {
        // 찾아서 안돌아가는 거 있으면 그거 반환.
        if (true == pContainer->Get_Dead() && false == pContainer->Get_Cloned())
        {
            CEffect_Container::EFFECT_DESC* pDesc = static_cast<CEffect_Container::EFFECT_DESC*>(pArg);
            pContainer->Set_EffectDesc(*pDesc);
            pContainer->Set_Dead(false);

            Safe_AddRef(pContainer);
            return pContainer;
        }
    }

    // 찾아봤는데 안돌아가는 게 없으면 새로 만들어서 넣고 반환.
    CEffect_Container* pContainer = Clone_Effect_From_Prototype(strECTag, pArg);
    iter->second.emplace_back(pContainer);

    Safe_AddRef(pContainer);
    return pContainer;
}

CEffect_Container* CEffect_Manager::Clone_Effect_From_Prototype(const _wstring& strECTag, void* pArg)
{
    auto& iter = m_EffectContainers.find(strECTag);

    if (m_EffectContainers.end() == iter)
        return nullptr;

    CEffect_Container* pEffectContainer = CREATE_CONTAINER(pArg);


    for (auto& strEffectTag : iter->second)
    {
        _wstring strFileExtention = Get_FileExtentin(strEffectTag);   // 확장자
        _wstring strEffectName = Get_FileName(strEffectTag);

        if (TEXT("PE") == strFileExtention)
        {
            CParticle_Effect* pEffect = Clone_ParticleEffect(strEffectName);

            if (nullptr == pEffect)
            {
                Safe_Release(pEffectContainer);
                return nullptr;
            }

            pEffectContainer->Add_Effect(pEffect);
        }
        else if (TEXT("TE") == strFileExtention)
        {
            CTexture_Effect* pEffect = Clone_TextureEffect(strEffectName);

            if (nullptr == pEffect)
            {
                Safe_Release(pEffectContainer);
                return nullptr;
            }
            pEffectContainer->Add_Effect(pEffect);
        }
        else if (TEXT("ME") == strFileExtention)
        {
            CMesh_Effect* pEffect = Clone_MeshEffect(strEffectName);
            if (nullptr == pEffect)
            {
                Safe_Release(pEffectContainer);
                return nullptr;
            }
            pEffectContainer->Add_Effect(pEffect);
        }
        else if (TEXT("TOP") == strFileExtention)
        {
            CTrail_Effect_OP* pEffect = Clone_TrailOP_Effect(strEffectName);

            if (nullptr == pEffect)
            {
                Safe_Release(pEffectContainer);
                return nullptr;
            }
            pEffectContainer->Add_Effect(pEffect);
        }
        else if (TEXT("TTP") == strFileExtention)
        {
            CTrail_Effect_TP* pEffect = Clone_TrailTP_Effect(strEffectName);

            if (nullptr == pEffect)
            {
                Safe_Release(pEffectContainer);
                return nullptr;
            }
            pEffectContainer->Add_Effect(pEffect);
        }
        else if (TEXT("TMP") == strFileExtention)
        {
            CTrail_Effect_MP* pEffect = Clone_TrailMP_Effect(strEffectName);

            if (nullptr == pEffect)
            {
                Safe_Release(pEffectContainer);
                return nullptr;
            }
            pEffectContainer->Add_Effect(pEffect);
        }
    }

    return pEffectContainer;
}

HRESULT CEffect_Manager::Effect_Pooling(const _wstring& strECTag, void* pArg, size_t iSize)
{
    auto& iter = m_EffectPooling.find(strECTag);

    if (m_EffectPooling.end() == iter)
    {
        vector<CEffect_Container*> Containers;

        for(size_t i = 0; i < iSize; ++i)
        {
            CEffect_Container* pContainer = Clone_Effect_From_Prototype(strECTag, pArg);

            if (nullptr == pContainer)
                return E_FAIL;

            pContainer->Set_Dead(true);

            Containers.emplace_back(pContainer);
        }

        m_EffectPooling.emplace(strECTag, Containers);
    }
    else
    {
        for (size_t i = 0; i < iSize; ++i)
        {
            CEffect_Container* pContainer = Clone_Effect_From_Prototype(strECTag, pArg);

            if (nullptr == pContainer)
                return E_FAIL;

            pContainer->Set_Dead(true);

            iter->second.emplace_back(pContainer);
        }
    }

    return S_OK;
}

void CEffect_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    for (auto& Pair : m_EffectPooling)
    {
        for (auto& Effect : Pair.second)
        {
            Safe_Release(Effect);
        }
        Pair.second.clear();
    }
    m_EffectPooling.clear();
}
