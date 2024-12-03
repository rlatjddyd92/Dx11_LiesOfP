#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Controller_Cutscene.h"
#include "Controller_UITool.h"
#include "GameInstance.h"
#include "CutScene.h"
IMPLEMENT_SINGLETON(CController_Cutscene)


CController_Cutscene::CController_Cutscene()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_Cutscene::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

    if (m_CutSceneList[0] == nullptr)
    {
        CGameObject* pCutScene = nullptr;
        for (int i = 0; i < 8; ++i)
        {
            pCutScene = m_pGameInstance->Get_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene"));
            if (nullptr != pCutScene)
            {
                m_CutSceneList[i] = static_cast<CCutScene*>(pCutScene);
            }
        }
    }

    m_fTrackPosition = &m_fTrackPosition_Zero;
	return S_OK;
}

void CController_Cutscene::Update(_float fTimeDelta)
{
    Menu();

    if (m_iPreSelectedCutScene != -1)
    {
        m_pCurrentCutScene->Update(fTimeDelta);
    }
}

void CController_Cutscene::Menu()
{
    static int item_selected_idx = -1; // Here we store our selected data as an index.

    static bool item_highlight = false;
    int item_highlighted_idx = 0; // Here we store our highlighted data as an index.

    static float fMaxFrame = { 0.f };

#pragma region 컷신 리스트 출력
    if (ImGui::CollapsingHeader("Sequence_List"))
    {
        ImGui::PushItemWidth(180);  //사이즈 고정
        if (ImGui::BeginListBox("Name"))
        {
            for (int n = 0; n < 8; n++)
            {
                const bool is_selected = (item_selected_idx == n);
                if (ImGui::Selectable(m_CutSceneNameList[n], is_selected))
                    item_selected_idx = n;

                if (item_highlight && ImGui::IsItemHovered())
                    item_highlighted_idx = n;
                                           
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
    }

#pragma endregion
    //새로운 컷신 선텍
    if(m_iPreSelectedCutScene != item_selected_idx)
    {
        m_iPreSelectedCutScene = item_selected_idx;
        m_pCurrentCutScene = m_CutSceneList[item_selected_idx];
        fMaxFrame = m_pCurrentCutScene->Get_MaxFrame();
        m_fTrackPosition = m_pCurrentCutScene->Get_CurFrame_Ptr();
        m_bPlay = false;
    }

#pragma region 컷신 세부사항 조정
    if (ImGui::CollapsingHeader("CutScene Detail"))
    {
        //컷신 이름
        ImGui::Text("Name : "); 
        if (m_iPreSelectedCutScene != -1)
        {
            ImGui::SameLine();
            ImGui::Text(m_CutSceneNameList[item_selected_idx]);
        }
        //최대 프레임 설정
        ImGui::PushItemWidth(150);   //사이즈 제한
        ImGui::InputFloat("Frame Max", &fMaxFrame, 1.f);
        ImGui::PopItemWidth();  //사이즈 제한 풀기
        //프레임 바 관련
        ImGui::SeparatorText("Frame");
      
        if (ImGui::Button("Play"))
        {
            m_bPlay = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            m_bPlay = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("ReSet Frame"))
        {
            *m_fTrackPosition = 0.f;
            m_pCurrentCutScene->Keyframe_Actived_Reset();
        }

        ImGui::SliderFloat("TimeLine", m_fTrackPosition, 0.0f, fMaxFrame);

        //정보 담기
        if (m_iPreSelectedCutScene != -1)
        {
            m_pCurrentCutScene->Set_MaxFrame(fMaxFrame);
            m_pCurrentCutScene->Set_Play(m_bPlay);
        }

        //키프레임
        ImGui::SeparatorText("Key Frame Detail");
        if (ImGui::Button("Create KeyFrame") && m_pCurrentCutScene != nullptr)
        {
            m_pCurrentCutScene->Create_KeyFrame();
        }
        //키프레임 리스트
        ImGui::BeginGroup();
        ImGui::Text("[KeyFrame_List]");

        static _int iSelectedKeyframeNum = -1;

        ImGui::BeginChild("KeyFrame List", ImVec2(150, 150), true); //네모 칸 만들기

        static _int iKeyFrameCount = 0;
        if(m_pCurrentCutScene!=nullptr)
            iKeyFrameCount = m_pCurrentCutScene->Get_KeyFrameCount();


        for (_int i = 0; i < iKeyFrameCount; i++)
        {
            _char szText[MAX_PATH] = "KeyFrmae ";
            _char szNum[MAX_PATH];
            _char szTime[MAX_PATH];
            sprintf_s(szNum, "%d : ", i);
            sprintf_s(szTime, "%f", m_pCurrentCutScene->Get_Selected_KeyFrame(i)->fTrackPosition);
            strcat_s(szText, szNum);
            strcat_s(szText, szTime);

            if (ImGui::Selectable(szText, iSelectedKeyframeNum == i))
            {
                iSelectedKeyframeNum = i;
            }
        }
        
        if (m_iPreSelected_KeyFrame != iSelectedKeyframeNum) //새로 고른 경우
        {
            m_iPreSelected_KeyFrame = iSelectedKeyframeNum;
            if(iSelectedKeyframeNum != -1)
            {
                CCutScene::CUTSCENE_DESC* pDesc = m_pCurrentCutScene->Get_Selected_KeyFrame(iSelectedKeyframeNum);
                *m_fTrackPosition = pDesc->fTrackPosition;
            }
            else
            {
                *m_fTrackPosition = m_fTrackPosition_Zero;
            }
        }

        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();

        //키프레임 별 요소 목록
        ImGui::BeginGroup();
        ImGui::Text("[Actor_List]");
       
        ImGui::BeginChild("Actor List", ImVec2(110, 90), true);

        static int selected = -1;
        for (int n = 0; n < CCutScene::TYPE_END; n++)
        {
            char buf[32];
            sprintf_s(buf, m_ActorTypeNameList[n]);
            if (ImGui::Selectable(buf, selected == n))
                selected = n;
        }
        ImGui::EndChild(); 
        //삭제 버튼
        if (ImGui::Button("Delete Selected KeyFrame"))
        {
            m_pCurrentCutScene->Delete_Selected_Keyframe(iSelectedKeyframeNum);
            iSelectedKeyframeNum = -1;
        }
        //TrackPosition 수정 체크박스
        static bool bChangeTrackPos = false;
        ImGui::Checkbox("Change TrackPosition", &bChangeTrackPos);
        if (bChangeTrackPos)
        {
            if (iSelectedKeyframeNum == -1)
                bChangeTrackPos = false;
            else
            {
                m_pCurrentCutScene->Get_Selected_KeyFrame(iSelectedKeyframeNum)->fTrackPosition = *m_fTrackPosition;
                m_pCurrentCutScene->Sort_KeyFrame_TrackPosition();
            }
        }
        ImGui::EndGroup();

        //요소별 메뉴 창
        ImGui::BeginChild("Detail view", ImVec2(300, 100));

        switch (selected)
        {
        case CCutScene::CAMERA:
            Show_Camera_State();
            break;
        case CCutScene::UI:
            Show_UI_State();
            break;
        case CCutScene::SHADER:
            Show_Shader_State();
            break;
        case CCutScene::GAMEOBJECT:
            Show_GamgeObject_State();
            break;
        default:
            break;
        }
        ImGui::EndChild();
        
       
        
        
    }
#pragma endregion
}

void CController_Cutscene::Show_Camera_State()
{
}

void CController_Cutscene::Show_UI_State()
{
}

void CController_Cutscene::Show_Shader_State()
{

}

void CController_Cutscene::Show_GamgeObject_State()
{
}

void CController_Cutscene::Camera_Nemu()
{
}

void CController_Cutscene::UI_Nemu()
{
}

void CController_Cutscene::Shader_Nemu()
{
}

void CController_Cutscene::GamgeObject_Nemu()
{
}


void CController_Cutscene::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
