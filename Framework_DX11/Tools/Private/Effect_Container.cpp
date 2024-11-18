#include "stdafx.h"
#include "Effect_Container.h"
#include "GameInstance.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"
#include "Mesh_Effect.h"
#include "Trail_Effect_OP.h"
#include "Trail_Effect_TP.h"
#include "Trail_Effect_MP.h"

CEffect_Container::CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Container::CEffect_Container(const CEffect_Container& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CEffect_Container::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Container::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	for(auto& elem : pDesc->pParticleEffect_Descs)
	{
		if (nullptr == elem)
			continue;

		CParticle_Effect::PARTICLE_EFFECT_DESC* pParticleDesc = static_cast<CParticle_Effect::PARTICLE_EFFECT_DESC*>(elem);
		pParticleDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Effect"), pParticleDesc)));

		_tchar szEffectName[MAX_PATH] = TEXT("");
		wcscpy_s(szEffectName, pParticleDesc->szEffectName);
		wcscat_s(szEffectName, TEXT(".PE"));
		m_EffectNames.emplace_back(szEffectName);
	}

	for (auto& elem : pDesc->pTextureEffect_Descs)
	{
		if (nullptr == elem)
			continue;

		CTexture_Effect::TEXTURE_EFFECT_DESC* pTextureDesc = static_cast<CTexture_Effect::TEXTURE_EFFECT_DESC*>(elem);
		pTextureDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Texture_Effect"), pTextureDesc)));

		_tchar szEffectName[MAX_PATH] = TEXT("");
		wcscpy_s(szEffectName, pTextureDesc->szEffectName);
		wcscat_s(szEffectName, TEXT(".TE"));
		m_EffectNames.emplace_back(szEffectName);
	}

	for (auto& elem : pDesc->pMeshEffect_Descs)
	{
		if (nullptr == elem)
			continue;

		CMesh_Effect::MESH_EFFECT_DESC* pMeshDesc = static_cast<CMesh_Effect::MESH_EFFECT_DESC*>(elem);
		pMeshDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Mesh_Effect"), pMeshDesc)));

		_tchar szEffectName[MAX_PATH] = TEXT("");
		wcscpy_s(szEffectName, pMeshDesc->szEffectName);
		wcscat_s(szEffectName, TEXT(".ME"));
		m_EffectNames.emplace_back(szEffectName);
	}

	for (auto& elem : pDesc->pTrail_OPDesc)
	{
		if (nullptr == elem)
			continue;

		CTrail_Effect_OP::TRAIL_OP_DESC* pTrailOP = static_cast<CTrail_Effect_OP::TRAIL_OP_DESC*>(elem);
		pTrailOP->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Effect_OP"), pTrailOP)));

		_tchar szEffectName[MAX_PATH] = TEXT("");
		wcscpy_s(szEffectName, pTrailOP->szEffectName);
		wcscat_s(szEffectName, TEXT(".TOP"));
		m_EffectNames.emplace_back(szEffectName);
	}

	for (auto& elem : pDesc->pTrail_TPDesc)
	{
		if (nullptr == elem)
			continue;

		CTrail_Effect_TP::TRAIL_TP_DESC* pTrailTP = static_cast<CTrail_Effect_TP::TRAIL_TP_DESC*>(elem);
		pTrailTP->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Effect_TP"), pTrailTP)));

		_tchar szEffectName[MAX_PATH] = TEXT("");
		wcscpy_s(szEffectName, pTrailTP->szEffectName);
		wcscat_s(szEffectName, TEXT(".TTP"));
		m_EffectNames.emplace_back(szEffectName);
	}

	for (auto& elem : pDesc->pTrail_MPDesc)
	{
		if (nullptr == elem)
			continue;

		CTrail_Effect_MP::TRAIL_MP_DESC* pTrailMP = static_cast<CTrail_Effect_MP::TRAIL_MP_DESC*>(elem);
		pTrailMP->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Effect_MP"), pTrailMP)));

		_tchar szEffectName[MAX_PATH] = TEXT("");
		wcscpy_s(szEffectName, pTrailMP->szEffectName);
		wcscat_s(szEffectName, TEXT(".TMP"));
		m_EffectNames.emplace_back(szEffectName);
	}

	m_strContainerName = pDesc->szEffectContainerName;

	return S_OK;
}

void CEffect_Container::Priority_Update(_float fTimeDelta)
{
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		Effect->Priority_Update(fTimeDelta);
	}
}

void CEffect_Container::Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::G))
	{
		if (false == m_bOrbit)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(5.f, 0.f, 0.f));
			m_bOrbit = true;
			m_bTurn = false;
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(0.f, 0.f, 0.f));
			m_bOrbit = false;
			m_bTurn = false;
		}
	}

	if (KEY_TAP(KEY::H))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(0.f, 0.f, 0.f));
		m_bTurn = !m_bTurn;
		m_bOrbit = false;
	}

	if (true == m_bOrbit)
	{
		_Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_Vec4		vRotation = XMQuaternionRotationRollPitchYaw(0.f, fTimeDelta * XMConvertToRadians(180.f), 0.f);
		_Matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
		
		vPos = XMVector3TransformCoord(vPos, RotationMatrix);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if(true == m_bTurn)
	{ 
		m_pTransformCom->Turn(_Vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta, 10.f);
	}

	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		Effect->Update(fTimeDelta);
	}
}

void CEffect_Container::Late_Update(_float fTimeDelta)
{
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		Effect->Late_Update(fTimeDelta);
	}
}

HRESULT CEffect_Container::Render()
{

	return S_OK;
}

HRESULT CEffect_Container::Save_Effects(_wstring strFilePath)
{
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		if(FAILED(Effect->Save(strFilePath)))
			return E_FAIL;

		if (strFilePath.back() == L'\0')
			strFilePath.resize(strFilePath.size() - 1);
	}

	_wstring strFoldertPath = strFilePath + TEXT("\\") + TEXT("EffectContainer");
	_tmkdir(strFoldertPath.c_str());
	Save_EffectContainer(strFoldertPath);

	return S_OK;
}

HRESULT CEffect_Container::Add_Effects_To_Layer()
{
	for (auto& Effect : m_Effects)
	{
		CEffect_Base::EFFECT_TYPE EffectType = Effect->Get_EffectType();
		Effect->Set_ParentMartix(nullptr);

		switch (EffectType)
		{
		case CEffect_Base::TYPE_PARTICLE:
			// 여기서 고대로 레이어에 추가하고
			if(FAILED(m_pGameInstance->Add_Object_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), Effect)))
				return E_FAIL;
			break;
		case CEffect_Base::TYPE_TEXTURE:
			if(FAILED(m_pGameInstance->Add_Object_ToLayer(LEVEL_TOOL, TEXT("Layer_TextureEffect"), Effect)))
				return E_FAIL;
			break;
		case CEffect_Base::TYPE_MESH:
			if(FAILED(m_pGameInstance->Add_Object_ToLayer(LEVEL_TOOL, TEXT("Layer_MeshEffect"), Effect)))
				return E_FAIL;
			break;
		case CEffect_Base::TYPE_TRAIL_OP:
			if (FAILED(m_pGameInstance->Add_Object_ToLayer(LEVEL_TOOL, TEXT("Layer_TrailOP"), Effect)))
				return E_FAIL;
			break;

		case CEffect_Base::TYPE_TRAIL_TP:
			if (FAILED(m_pGameInstance->Add_Object_ToLayer(LEVEL_TOOL, TEXT("Layer_TrailTP"), Effect)))
				return E_FAIL;
			break;

		case CEffect_Base::TYPE_TRAIL_MP:
			if (FAILED(m_pGameInstance->Add_Object_ToLayer(LEVEL_TOOL, TEXT("Layer_TrailMP"), Effect)))
				return E_FAIL;
			break;
		}
	}
	// 다했으면 싹 지우고

	m_Effects.clear();
	return S_OK;
}

HRESULT CEffect_Container::Reset_Effects()
{
	for (auto& Effect : m_Effects)
	{
		Effect->Reset();
	}

	return S_OK;
}


HRESULT CEffect_Container::Save_EffectContainer(_wstring strFolderPath)
{
	_wstring strResultPath = strFolderPath + TEXT("\\") + m_strContainerName + TEXT(".EC");

	_char FilePath[MAX_PATH] = {};
	int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (sizeNeeded > 0 && sizeNeeded <= MAX_PATH)
	{
		WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, FilePath, MAX_PATH, nullptr, nullptr);
	}

	ofstream outfile(FilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;

	for(auto& elem : m_EffectNames)
	{
		_tchar szName[MAX_PATH] = TEXT("");
		wcscpy_s(szName, elem.c_str());
		outfile.write(reinterpret_cast<const _char*>(szName), MAX_PATH * sizeof(_tchar));
	}

	outfile.close();


	return S_OK;
}

HRESULT CEffect_Container::Load_EffectContainer(_wstring strFilePath)
{
	ifstream infile(strFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;

	while (true)
	{
		_tchar strEffectName[MAX_PATH] = TEXT("");
		infile.read(reinterpret_cast<_char*>(strEffectName), MAX_PATH * sizeof(_tchar));

		if (true == infile.eof())
			break;

		_wstring strCurrentPath = getPreviousFolderPath(strFilePath);
		strCurrentPath = strCurrentPath + TEXT("\\") + strEffectName;

		if(FAILED(Load_Effect_By_Path(strCurrentPath)))
			return E_FAIL;
	}

	infile.close();

	return S_OK;
}

HRESULT CEffect_Container::Load_Effect_By_Path(const _wstring& strFilePath)
{
	size_t dotPosition = strFilePath.find_last_of(L'.');

	// 마지막 점 이후부터 끝까지의 문자열을 반환합니다.
	_wstring strExtention = strFilePath.substr(dotPosition + 1);
	strExtention;
	size_t nullPos = strExtention.find(TEXT('\0'));
	if (nullPos != _wstring::npos)
		strExtention.erase(nullPos);

	if (TEXT("PE") == strExtention)
	{
		CParticle_Effect::PARTICLE_EFFECT_DESC TestDesc = {};

		ifstream infile(strFilePath, ios::binary);

		if (!infile.is_open())
			return E_FAIL;

		TestDesc.pParentMatrix = { nullptr };
		TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
		TestDesc.fSpeedPerSec = 1.f;
		TestDesc.iLevelIndex = LEVEL_TOOL;

		infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
		infile.read(reinterpret_cast<_char*>(&TestDesc.BufferDesc), sizeof(TestDesc.BufferDesc));			// 이게 실제로 적용되는 거.

		infile.close();

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Effect"), &TestDesc)))
			return E_FAIL;

	}
	else if (TEXT("TE") == strExtention)
	{
		CTexture_Effect::TEXTURE_EFFECT_DESC TestDesc = {};

		ifstream infile(strFilePath, ios::binary);

		if (!infile.is_open())
			return E_FAIL;

		TestDesc.pParentMatrix = { nullptr };
		TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
		TestDesc.fSpeedPerSec = 1.f;
		TestDesc.iLevelIndex = LEVEL_TOOL;

		infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 텍스처 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));		// 이거는 그대로 가져다 쓰고

		infile.close();

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TextureEffect"), TEXT("Prototype_GameObject_Texture_Effect"), &TestDesc)))
			return E_FAIL;
	}
	else if (TEXT("ME") == strExtention)
	{
		CMesh_Effect::MESH_EFFECT_DESC TestDesc = {};

		ifstream infile(strFilePath, ios::binary);

		if (!infile.is_open())
			return E_FAIL;

		TestDesc.pParentMatrix = { nullptr };
		TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
		TestDesc.fSpeedPerSec = 1.f;
		TestDesc.iLevelIndex = LEVEL_TOOL;

		infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 텍스처 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));		// 이거는 그대로 가져다 쓰고

		infile.close();

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_MeshEffect"), TEXT("Prototype_GameObject_Mesh_Effect"), &TestDesc)))
			return E_FAIL;
	}
	else if (TEXT("TOP") == strExtention)
	{
		CTrail_Effect_OP::TRAIL_OP_DESC TestDesc = {};

		ifstream infile(strFilePath, ios::binary);

		if (!infile.is_open())
			return E_FAIL;

		TestDesc.pParentMatrix = { nullptr };
		TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
		TestDesc.fSpeedPerSec = 1.f;
		TestDesc.iLevelIndex = LEVEL_TOOL;

		infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
		infile.read(reinterpret_cast<_char*>(&TestDesc.BufferDesc), sizeof(TestDesc.BufferDesc));			// 이게 실제로 적용되는 거.

		infile.close();

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TrailOP"), TEXT("Prototype_GameObject_Trail_Effect_OP"), &TestDesc)))
			return E_FAIL;
	}
	else if (TEXT("TTP") == strExtention)
	{
		CTrail_Effect_TP::TRAIL_TP_DESC TestDesc = {};

		ifstream infile(strFilePath, ios::binary);

		if (!infile.is_open())
			return E_FAIL;

		TestDesc.pParentMatrix = { nullptr };
		TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
		TestDesc.fSpeedPerSec = 1.f;
		TestDesc.iLevelIndex = LEVEL_TOOL;

		infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
		infile.read(reinterpret_cast<_char*>(&TestDesc.BufferDesc), sizeof(TestDesc.BufferDesc));			// 이게 실제로 적용되는 거.

		infile.close();

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TrailTP"), TEXT("Prototype_GameObject_Trail_Effect_TP"), &TestDesc)))
			return E_FAIL;

	}
	else if (TEXT("TMP") == strExtention)
	{
		CTrail_Effect_MP::TRAIL_MP_DESC TestDesc = {};

		ifstream infile(strFilePath, ios::binary);

		if (!infile.is_open())
			return E_FAIL;

		TestDesc.pParentMatrix = { nullptr };
		TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
		TestDesc.fSpeedPerSec = 1.f;
		TestDesc.iLevelIndex = LEVEL_TOOL;

		infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
		infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
		infile.read(reinterpret_cast<_char*>(&TestDesc.BufferDesc), sizeof(TestDesc.BufferDesc));			// 이게 실제로 적용되는 거.

		infile.close();

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TrailMP"), TEXT("Prototype_GameObject_Trail_Effect_MP"), &TestDesc)))
			return E_FAIL;
	}
	return S_OK;
}

_wstring CEffect_Container::getPreviousFolderPath(const _wstring& path)
{
	size_t lastSlashPos = path.find_last_of(L"\\/");
	if (lastSlashPos == std::wstring::npos) {
		return L""; // 구분자가 없으면 빈 문자열 반환
	}

	// 마지막 경로 구분자를 기준으로 이전 폴더 경로 추출
	size_t prevSlashPos = path.find_last_of(L"\\/", lastSlashPos - 1);
	if (prevSlashPos == std::wstring::npos) {
		return L""; // 이전 폴더가 없으면 빈 문자열 반환
	}

	// 이전 폴더까지의 경로 반환
	return path.substr(0, prevSlashPos);
}

CEffect_Container* CEffect_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Container* pInstance = new CEffect_Container(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CEffect_Container"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Container::Clone(void* pArg)
{
	CEffect_Container* pInstance = new CEffect_Container(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CEffect_Container"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Container::Free()
{
	__super::Free();

	for (auto& elem : m_Effects)
		Safe_Release(elem);
	
	m_Effects.clear();
}
