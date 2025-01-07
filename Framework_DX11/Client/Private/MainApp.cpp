#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "Level_Loading.h"
#include "GameInstance.h"
#include "GameInterface_Controller.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);	
}

HRESULT CMainApp::Initialize()
{	
	/*_float		fData = 0.0561 + 0.1353 + 0.278 + 0.4868 + 0.7261 + 0.9231 + 1.f + 0.9231 + 0.7261 + 0.4868 + 0.278 + 0.1353 + 0.0561;*/


	ENGINE_DESC			EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowsed = true;

	/* 엔진을 사용하기위한 여러 초기화 과정을 수행한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Gara()))
		return E_FAIL;	

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	// 24-11-09 김성용
	// GameInterface 세팅 
	if (FAILED(GET_GAMEINTERFACE->Initialize_GameInterface(&m_pDevice, &m_pContext, m_pGameInstance)))
		return E_FAIL;

	CCamera_Manager::Get_Instance()->Initialize(m_pDevice, m_pContext);

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	_float fObject_Time = fTimeDelta;

	if (GET_GAMEINTERFACE->IsGamePause())
		fObject_Time = 0.f;


	m_pGameInstance->Update_Engine(fObject_Time);

	// 24-12-01 김성용 
	// UI 연출 관련 테스트 코드 
	if (KEY_HOLD(KEY::CTRL))
	{
		if (KEY_TAP(KEY::NUM1))
			GET_GAMEINTERFACE->Show_Script(TEXT("Testtest"), TEXT("Testtest"), 10.f);
		if (KEY_TAP(KEY::NUM2))
			GET_GAMEINTERFACE->Show_PlayerDead_UI();
		if (KEY_TAP(KEY::NUM3))
			GET_GAMEINTERFACE->Show_Heart(TEXT("심장이 고동친다"));
		if (KEY_TAP(KEY::NUM4))
			GET_GAMEINTERFACE->Fade_Out(TEXT(""), TEXT(""));
		if (KEY_TAP(KEY::NUM5))
			GET_GAMEINTERFACE->Fade_In();
		if (KEY_TAP(KEY::NUM6))
			GET_GAMEINTERFACE->Show_Script_Npc_Talking(NPC_SCRIPT::SCR_MANUS);
		if (KEY_TAP(KEY::NUM7))
			GET_GAMEINTERFACE->Show_Script_Npc_Talking(NPC_SCRIPT::SCR_SOPIA_DIE);
		if (KEY_TAP(KEY::NUM8))
			GET_GAMEINTERFACE->Show_Script_Npc_Talking(NPC_SCRIPT::SCR_ZEMINI_SECOND);
		if (KEY_TAP(KEY::NUM9))
			GET_GAMEINTERFACE->UIPart_On();

	}

	if (GET_GAMEINTERFACE->IsTalking_WithNPC() == true)
		_int i = 0;

	if (GET_GAMEINTERFACE->IsTalking_SelectPage() == true)
		_int i = 0;

	if (GET_GAMEINTERFACE->IsTalking_WithNPC(NPC_SCRIPT::SCR_MANUS) == true)
		_int i = 0;

	if (GET_GAMEINTERFACE->IsEndTalk_WithNPC(NPC_SCRIPT::SCR_MANUS) == true)
		_int i = 0;
	
	if (KEY_TAP(KEY::M))
	{
		g_isHideCursor = !g_isHideCursor;

		if (g_isHideCursor)
		{
			//ShowCursor(FALSE);
			RECT rect;
			GetClientRect(g_hWnd, &rect); // 게임 창의 클라이언트 영역을 얻어옴
			ClientToScreen(g_hWnd, (POINT*)&rect.left);  // 좌표를 화면 좌표로 변환
			ClientToScreen(g_hWnd, (POINT*)&rect.right);
			ClipCursor(&rect); // 마우스를 게임 창의 영역에 고정
		}
		else
		{
			//ShowCursor(TRUE);
			ClipCursor(NULL);
		}
	}

	GET_GAMEINTERFACE->Update_GameInterface(fTimeDelta);

	Update_Radial(fTimeDelta);
	
//#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
//#endif
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin();

	m_pGameInstance->Draw_Engine();

	++m_iNumDraw;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);
		m_fTimeAcc = 0.f;
		m_iNumDraw = 0;
	}

	m_pGameInstance->Render_Text(TEXT("Font_145"), m_szFPS, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));


	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Gara()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Gulim"), TEXT("../Bin/Resources/Fonts/Gulim.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_145"), TEXT("../Bin/Resources/Fonts/143ex.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_Static()
{
#pragma region Shader
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;	

	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModelInstnace */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModelInstnace"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Instance.hlsl"), VTXMODELINSTANCE::Elements, VTXMODELINSTANCE::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_SSD"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SSD.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma region VIBuffer
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CVIBuffer_2DPolygon::UIPOLIGON_DESC tDesc{};
	tDesc.iPoint = 8;
	tDesc.fAngle = 0.f;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_2DPolygon"),
		CVIBuffer_2DPolygon::Create(m_pDevice, m_pContext, tDesc))))
		return E_FAIL;
#pragma endregion

#pragma region Collider
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
#pragma endregion

#pragma region Component
	/* For. Prototype_Component_RigidBody */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		CFsm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Sound */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		CSound::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{	
	/* 어떤 레벨을 선택하던 로딩 레벨로 선 진입한다.  */
	/* 로딩레벨에서 내가 선택한 레벨에 필요한 자원을 준비한다. */
	/* 로딩 레벨은 다음레벨(내가 선택한 레벨)이 누구인지를 인지해야한다. */
	if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update_Radial(_float fTimeDelta)
{
	RADIAL_DESC* pDesc = m_pGameInstance->Get_RadialDesc();

	if (GET_GAMEINTERFACE->IsTalking_SelectPage())
	{
		if (!m_isRadialBlur)
		{
			m_fRadialStartDelay += fTimeDelta;

			if (m_fRadialStartDelay > 0.8f)
			{
				m_fRadialDurationDelay = 0.5f;
				m_fRadialStartDelay = 0.f;

				pDesc->vRadialCenterPos.x = m_pGameInstance->Get_Random(0.3f, 0.7f);
				pDesc->vRadialCenterPos.y = m_pGameInstance->Get_Random(0.5f, 0.8f);
				pDesc->isOnRadial = true;

				m_isRadialBlur = true;
			}
		}
		else
		{
			m_fRadialDurationDelay = max(0.f, m_fRadialDurationDelay - fTimeDelta);
			if (m_fRadialDurationDelay <= 0.f)
			{
				pDesc->isOnRadial = false;
				m_isRadialBlur = false;
			}

			pDesc->fRadialPower = m_fRadialDurationDelay * 0.05f;
		}
	}
	else
	{
		if (m_isRadialBlur)
		{
			m_fRadialDurationDelay = max(0.f, m_fRadialDurationDelay - fTimeDelta);
			if (m_fRadialDurationDelay <= 0.f)
			{
				pDesc->isOnRadial = false;
				m_isRadialBlur = false;
			}

			pDesc->fRadialPower = m_fRadialDurationDelay * 0.05f;
		}
	}
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	CEffect_Manager::Destroy_Instance();
	CCamera_Manager::Destroy_Instance();
	GET_GAMEINTERFACE->Release_GameInterface();

	ClipCursor(NULL); // 마우스를 제한 해제
	ShowCursor(TRUE);  // 마우스 커서를 다시 보이게 함

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}

