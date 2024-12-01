#include "stdafx.h"
#include "Controller_Cutscene.h"
#include "GameInstance.h"

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

	return S_OK;
}

void CController_Cutscene::Update(_float fTimeDelta)
{
	Show_CutScene_List();
	CutScene_Detail();
}

void CController_Cutscene::Show_CutScene_List()
{

    static int item_selected_idx = 0; // Here we store our selected data as an index.

    static bool item_highlight = false;
    int item_highlighted_idx = 0; // Here we store our highlighted data as an index.

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

    m_CurSequence = &m_SequenceList[item_selected_idx];
}

void CController_Cutscene::CutScene_Detail()
{
    //if (ImGui::CollapsingHeader("Sequencer"))
    //{
    //    // let's create the sequencer
    //    static int selectedEntry = -1;
    //    static int firstFrame = 0;
    //    static bool expanded = true;
    //    static int currentFrame = 100;

    //    ImGui::PushItemWidth(130);
    //    ImGui::InputInt("Frame Min", &m_CurSequence.mFrameMin);
    //    ImGui::SameLine();
    //    ImGui::InputInt("Frame ", &currentFrame);
    //    ImGui::SameLine();
    //    ImGui::InputInt("Frame Max", &m_CurSequence.mFrameMax);
    //    ImGui::PopItemWidth();
    //    Sequencer(&m_CurSequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_CHANGE_FRAME);
    //    // add a UI to edit that particular item
    //    if (selectedEntry != -1)
    //    {
    //        const MySequence::MySequenceItem& item = m_CurSequence.myItems[selectedEntry];
    //        ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
    //        // switch (type) ....
    //    }
    //}

    ImGui::PushItemWidth(180);   //사이즈 제한
    ImGui::InputInt("Frame Max", &m_CurSequence->mFrameMax);
    ImGui::PopItemWidth();  //사이즈 제한 풀기

    _float fCurrentFrame = 0.f;
    ImGui::DragFloat("TimeLine", &fCurrentFrame, 0.01f, 0.f, m_CurSequence->mFrameMax);
}

void CController_Cutscene::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
