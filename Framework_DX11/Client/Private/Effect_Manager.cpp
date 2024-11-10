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

    Load_Effects(strEffectPath);
    Load_Textures();
    return S_OK;
}

CEffect_Container* CEffect_Manager::Clone_Effect(EFFECT eEffect, void* pArg)
{
    switch (eEffect)
    {
    case EFFECT_POWER_HIT:
    {
        CEffect_Container* pEffectContainer = CREATE_CONTAINER(pArg);

        // 여기서부터 추가
        CParticle_Effect* pParticleEffect = Clone_ParticleEffect(TEXT("Particle_Test"));
        pEffectContainer->Add_Effect(pParticleEffect);

        // 여기까지

        return pEffectContainer;
    }
        break;

    default:
        break;
    }

    return nullptr;
}

HRESULT CEffect_Manager::Add_Effect_ToLayer(_uint iLevelID, EFFECT eEffect, void* pArg)
{
    switch (eEffect)
    {
    case EFFECT_POWER_HIT:
    {
        CEffect_Container* pEffectContainer = CREATE_CONTAINER(pArg);

        // 여기서부터 추가
        CParticle_Effect* pParticleEffect = Clone_ParticleEffect(TEXT("Particle_Test"));
        pEffectContainer->Add_Effect(pParticleEffect);


        // 여기까지

        if(FAILED(m_pGameInstance->Add_Object_ToLayer(iLevelID, TEXT("Layer_Effect"), pEffectContainer)))
            return E_FAIL;
    }
    break;

    default:
        break;
    }


    return S_OK;
}

HRESULT CEffect_Manager::Load_Effects(const _wstring& strEffectPath)
{
    std::wstring searchPath = strEffectPath + L"\\*.*";
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

            }

        }
    }
    // 핸들 닫기
    FindClose(hFind);

	return S_OK;
}

HRESULT CEffect_Manager::Load_Textures()
{
#pragma region EFFECT
#pragma region DIFFUSE
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

    /* For. Prototype_Component_Texture_Stone_8x8 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Stone_8x8"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SubUV_DebStone_03_8x8_SC_KMH.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Thunder_4x4 */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Thunder_4x4"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SubUV_Thunder_01_4x4_SC_HJS.dds"), 1))))
        return E_FAIL;

#pragma endregion

#pragma region
    /* For. Prototype_Component_Texture_DefaultNormal */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DefaultNormal"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/default_normal.dds"), 1))))
        return E_FAIL;

    /* For. Prototype_Component_Texture_Stone_8x8_Normal */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Stone_8x8_Normal"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SubUV_DebStone_03_8x8_SN_KMH.dds"), 1))))
        return E_FAIL;

#pragma endregion
#pragma endregion

    return S_OK;
}


_wstring CEffect_Manager::Get_FileExtentin(const _wstring& strFileName)
{
    size_t dotPos = strFileName.find_last_of(L'.');
    if (dotPos == std::wstring::npos) {
        return L""; // 확장자가 없으면 빈 문자열 반환
    }

    _wstring strExtention = strFileName.substr(dotPos + 1);

    size_t nullPos = strExtention.find(L'\0');
    if (nullPos != std::wstring::npos) {
        strExtention.erase(nullPos);
    }

    return strExtention;
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
    infile.read(reinterpret_cast<_char*>(&InstanceDesc), sizeof(InstanceDesc));		// 이거 버퍼 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.InitDesc), sizeof(TestDesc.InitDesc));			// 이게 실제로 적용되는 거.

    infile.close();

    _wstring strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Point_Instance_");
    strVIBufferTag += strEffectName;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strVIBufferTag,CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, InstanceDesc, true))))
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
    infile.read(reinterpret_cast<_char*>(&TestDesc.TextureDesc), sizeof(TestDesc.TextureDesc));		// 이거 텍스처 초기화에 쓰고
    infile.read(reinterpret_cast<_char*>(&TestDesc.ActionDesc), sizeof(TestDesc.ActionDesc));		// 이거는 그대로 가져다 쓰고

    infile.close();

    m_TEDescs.emplace(strEffectName, TestDesc);

    return S_OK;
}

HRESULT CEffect_Manager::Load_Mesh_Effect(const _wstring& strResultPath)
{
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

void CEffect_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
