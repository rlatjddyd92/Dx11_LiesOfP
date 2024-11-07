#include "stdafx.h"
#include "Controller_AnimationTool.h"
#include "GameInstance.h"

//++
#include "AnimModel.h"
#include "Animation.h"
#include "TargetBall.h"
#include "Mesh.h"

IMPLEMENT_SINGLETON(CController_AnimationTool)

CController_AnimationTool::CController_AnimationTool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_AnimationTool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//애니메이션 툴 피규어(위치나 이런부분 물어보기)
	
	CAnimModel::ANIMMODEL_DESC Desc{};
	Desc.vPosition = { 0.f,0.f,0.f };
	Desc.vScale = { 2.f,2.f,2.f };
	Desc.vRotation = { 0.f,0.f,0.f };
	Desc.pUpdateCtr = &m_bObjRenderCtr;
	
	strcpy_s(m_szCurrentModelText, "Prototype_AnimModel_Test");
	strcpy_s(Desc.szModelTag, m_szCurrentModelText);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_AnimationTool_Test"), TEXT("Prototype_GameObject_Anim"), &Desc)))
		return E_FAIL;

	m_ModelNames.reserve(m_pGameInstance->Get_ModelPrototypes(LEVEL_TOOL).size());
	for (auto& Pair : m_pGameInstance->Get_ModelPrototypes(LEVEL_TOOL))
	{
		m_ModelNames.push_back(Pair.first);
	}

	//Prototype_GameObject_TargetBall
	CTargetBall::TARGETBALL_DESC TB_Desc{};
	TB_Desc.pPos = &m_vPos;
	TB_Desc.pRenderCtr = &m_bTargetBallRender;
	TB_Desc.pUpdateCtr = &m_bObjRenderCtr;
	TB_Desc.vCenter = _float3{0, 0, 0};
	TB_Desc.fRadius = 0.2f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_AnimationTool_Test"), TEXT("Prototype_GameObject_TargetBall"), &TB_Desc)))
		return E_FAIL;


	//지정된 정점 위치 보여주는 방법

	return S_OK;
}

void CController_AnimationTool::SetUp_AnimTool()
{
	m_bObjRenderCtr = true;
	CModel* pModel = dynamic_cast<CModel*>(m_pGameInstance->Find_Component(LEVEL_TOOL, TEXT("Layer_AnimationTool_Test"), TEXT("Com_Model")));
	
	if (m_pCopyModelCom != pModel)
	{
		m_pCopyModelCom = pModel;
		Safe_AddRef(pModel);

		m_pCopyAnimVec = nullptr;
		m_pCopyBoneVec = nullptr;
	}
	else
	{
		Safe_AddRef(pModel);
	}

	if (m_pCopyModelCom != nullptr)
	{
		if (m_pCopyAnimVec == nullptr)
		{
			m_pCopyAnimVec = &m_pCopyModelCom->Get_Animations();

			m_AnimSpeedPS = (*m_pCopyAnimVec)[m_iSelected_Index_Anim]->Get_SpeedPerSec();

			m_AnimDuration = (*m_pCopyAnimVec)[m_iSelected_Index_Anim]->Get_Duration();
		}

		if (m_pCopyBoneVec == nullptr)
		{
			m_pCopyBoneVec = &m_pCopyModelCom->Get_Bones();
		}
	}
	


	if (ImGui::BeginTabBar("AnimTool"))
	{
		if (ImGui::BeginTabItem("Anim"))
		{
			ListUp_Anim();
			SetUp_Controller_Anim();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Bone"))
		{
			ListUp_Bone();
			SetUp_Controller_Bone();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Vertex"))
		{
			m_bTargetBallRender = true;
			ListUp_Virtex();
			SetUp_Controller_Vertex();
			ImGui::EndTabItem();
		}
		else
		{
			m_bTargetBallRender = false;
		}

		ImGui::EndTabItem();
	}
}

void CController_AnimationTool::ListUp_Anim()
{

	m_iCurSelected_Index_Model = m_iSelected_Index_Model;
	ImGui::PushItemWidth(200); // 크기조정
	if (ImGui::BeginListBox("Model List"))
	{
		bool is_selected{ false };
		for (int i = 0; i < m_ModelNames.size(); i++)
		{
			if (m_iSelected_Index_Model == i)
			{
				is_selected = true;
			}

			if (ImGui::Selectable(m_ModelNames[i], is_selected))
			{
				m_iSelected_Index_Model = i;
			}


			if (!is_selected)
				ImGui::SetItemDefaultFocus();
			is_selected = false;
			// 반복문으로 리스트박스의 선택된 객체 찾기
		}
		ImGui::EndListBox();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("ChangeModel"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			CComponent* pOut = nullptr;

			auto	iter = m_Models.find(string(m_ModelNames[m_iSelected_Index_Model]));
			
			if (iter == m_Models.end())
			{
				string strName(m_ModelNames[m_iSelected_Index_Model]);
				_wstring szName;
				szName.assign(strName.begin(), strName.end());

				CComponent* pComponent = m_pGameInstance->Clone_Component(LEVEL_TOOL, szName.c_str());
				pOut = m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_AnimationTool_Test"), 0)->Change_Component(TEXT("Com_Model"), pComponent);
			}
			else
			{
				pOut = m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_AnimationTool_Test"), 0)->Change_Component(TEXT("Com_Model"), iter->second);
				
			}
			string strCurModelText(m_szCurrentModelText);
			iter = m_Models.find(strCurModelText);
			if (iter == m_Models.end())
			{
				m_Models.emplace(strCurModelText, dynamic_cast<CModel*>(pOut));
			}

			strcpy_s(m_szCurrentModelText, m_ModelNames[m_iSelected_Index_Model]);


			m_iCurSelected_Index_Anim = 0;
			m_iCurSelected_Index_Anim_Boundary = 0;
			m_iCurSelected_Index_Bone = 0;
			m_iCurSelected_Index_KeyFrame = 0;

			m_iSelected_Index_Anim = 0;
			m_iSelected_Index_Anim_Boundary = 0;
			m_iSelected_Index_Bone = 0;
			m_iSelected_Index_KeyFrame = 0;

			m_fAnimTrackPosition = 0.f;

		}
	}

	ImGui::Text("\n");
	ImGui::Text("\n");

	//이전 선택 저장
	m_iCurSelected_Index_Anim = m_iSelected_Index_Anim;
	_bool BoundaryCheck = m_pCopyModelCom->Get_IsUseBoundary();
	if (m_pCopyModelCom != nullptr && BoundaryCheck)
	{
		ImGui::Checkbox("Divide_Boundary", &m_bDivide_Boundary);
		ImGui::Text("Anim Lower Body");
		ImGui::SameLine();
		ImGui::Text("\t\t");
		ImGui::SameLine();
		ImGui::Text("Anim Upper Body");
	}
	else
		ImGui::Text("Animation");

	ImGui::PushItemWidth(200); // 크기조정
	if (ImGui::BeginListBox("##Anim_Lower"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			bool is_selected{ false };
			for (int i = 0; i < m_pCopyAnimVec->size(); i++)
			{
				if (m_iSelected_Index_Anim == i)
				{
					is_selected = true;
				}

				if (ImGui::Selectable((*m_pCopyAnimVec)[i]->Get_Name(), is_selected))
				{
					m_iSelected_Index_Anim = i;
				}


				if (!is_selected)
					ImGui::SetItemDefaultFocus();
				is_selected = false;
				// 반복문으로 리스트박스의 선택된 객체 찾기
			}
		}
		
		ImGui::EndListBox();
	}
	if (m_pCopyModelCom != nullptr)
	{
		if (m_iSelected_Index_Anim != m_iCurSelected_Index_Anim)
		{
			m_pCopyModelCom->SetUp_NextAnimation(m_iSelected_Index_Anim, true);
			m_AnimSpeedPS = (*m_pCopyAnimVec)[m_iSelected_Index_Anim]->Get_SpeedPerSec();
			m_AnimDuration = (*m_pCopyAnimVec)[m_iSelected_Index_Anim]->Get_Duration();
		}
	}
	
	if (m_pCopyModelCom != nullptr && BoundaryCheck)
	{
		m_iCurSelected_Index_Anim_Boundary = m_iSelected_Index_Anim_Boundary;

		if (!m_bDivide_Boundary)
		{
			m_iSelected_Index_Anim_Boundary = m_iSelected_Index_Anim;
		}

		//
		//m_pCopyModelCom.
		_uint iIndexBoundaryAnim = m_pCopyModelCom->Get_CurrentAnimationIndex_Boundary();
		if (iIndexBoundaryAnim != m_iSelected_Index_Anim_Boundary && iIndexBoundaryAnim == m_iCurSelected_Index_Anim)
		{
			m_iSelected_Index_Anim_Boundary = iIndexBoundaryAnim;
		}

		ImGui::SameLine();
		if (ImGui::BeginListBox("##Anim_Upper"))
		{
			bool is_selected_Boundary{ false };

			for (int i = 0; i < m_pCopyAnimVec->size(); i++)
			{
				if (m_iSelected_Index_Anim_Boundary == i)
				{
					is_selected_Boundary = true;
				}

				if (ImGui::Selectable((*m_pCopyAnimVec)[i]->Get_Name(), is_selected_Boundary))
				{
					m_iSelected_Index_Anim_Boundary = i;
				}

				if (!is_selected_Boundary)
					ImGui::SetItemDefaultFocus();
				is_selected_Boundary = false;
				// 반복문으로 리스트박스의 선택된 객체 찾기
			}
			

			ImGui::EndListBox();
		}
	}

	ImGui::PopItemWidth();

	//이전 선택지 비교 후 애니메이션 전환
	if (m_pCopyModelCom != nullptr)
	{
		if (m_iSelected_Index_Anim_Boundary != m_iCurSelected_Index_Anim_Boundary)
		{
			m_pCopyModelCom->SetUp_NextAnimation_Boundary(m_iSelected_Index_Anim_Boundary, true);
		}
	}


}

void CController_AnimationTool::SetUp_Controller_Anim()
{
	ImGui::Text("\n");
	//속도 관련
	ImGui::InputDouble("SpeedPerSec", &m_AnimSpeedPS);

	if (ImGui::Button("Change_SpeedPerSec"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			(*m_pCopyAnimVec)[m_iSelected_Index_Anim]->Set_SpeedPerSec(m_AnimSpeedPS);
		}
	}
	EVENT_KEYFRAME EventKeyDesc{};

	ImGui::Text("\n");
	ImGui::InputText("EventKey Name", m_szEvKeyFrameText, MAX_PATH);

	//이펙트 관련
	if (ImGui::Button("Add_EventKeyFrame"))
	{
		if (m_pCopyModelCom != nullptr && strcmp(m_szEvKeyFrameText, ""))
		{
			strcpy_s(EventKeyDesc.szEventName, MAX_PATH, m_szEvKeyFrameText);
			EventKeyDesc.TrackPosition = m_fAnimTrackPosition;
			(*m_pCopyAnimVec)[m_iSelected_Index_Anim]->Add_EventKeyFrame(EventKeyDesc);
		}
	}

	ImGui::Text("\n");
	//정지 재생 관련
	m_fAnimTrackPosition = (_float)m_pCopyModelCom->Get_CurrentTrackPosition();
	if (ImGui::SliderFloat("Anim Track", &m_fAnimTrackPosition, 0.f, (_float)m_AnimDuration))
	{
		if (!m_bIsAnimStopped)
		{
			m_bIsAnimStopped = true;
		}
	}

	if (m_fAnimTrackPosition >= m_AnimDuration)
	{
		m_fAnimTrackPosition = (_float)m_AnimDuration - 0.1f;
	}

	if (m_bIsAnimStopped)
	{
		m_pCopyModelCom->Set_CurrentTrackPosition(m_fAnimTrackPosition);
		m_pCopyModelCom->Set_CurrentTrackPosition_Boundary(m_fAnimTrackPosition);
	}

	ImGui::SameLine();
	if (m_bIsAnimStopped)
	{
		if (ImGui::Button("Play Anim"))
		{
			m_bIsAnimStopped = false;
		}
		m_pCopyModelCom->Set_AnimPlay(false);
	}
	else
	{
		if (ImGui::Button("Pause Anim"))
		{
			m_bIsAnimStopped = true;
		}
		m_pCopyModelCom->Set_AnimPlay(true);
	}


	//저장한 키프레임들 확인
	vector<EVENT_KEYFRAME>*  EvKeyFramesvec = (*m_pCopyAnimVec)[m_iSelected_Index_Anim]->Get_EventKeyFrames_Ptr();

	m_iCurSelected_Index_KeyFrame = m_iSelected_Index_KeyFrame;

	ImGui::Text("\n");
	ImGui::PushItemWidth(300); // 크기조정
	if (ImGui::BeginListBox("KeyFrame List"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			bool is_selected{ false };
			for (int i = 0; i < (*EvKeyFramesvec).size(); i++)
			{
				if (m_iSelected_Index_KeyFrame == i)
				{
					is_selected = true;
				}

				if (ImGui::Selectable((*EvKeyFramesvec)[i].szEventName, is_selected))
				{
					m_iSelected_Index_KeyFrame = i;
				}


				if (!is_selected)
					ImGui::SetItemDefaultFocus();
				is_selected = false;
				// 반복문으로 리스트박스의 선택된 객체 찾기
			}
		}

		ImGui::EndListBox();
	}
	if (m_pCopyModelCom != nullptr)
	{
		if (m_iSelected_Index_KeyFrame != m_iCurSelected_Index_KeyFrame)
		{
			//선택한 이벤트 키프레임의 위치로 이동
			m_pCopyModelCom->Set_CurrentTrackPosition((*EvKeyFramesvec)[m_iSelected_Index_KeyFrame].TrackPosition);
			m_pCopyModelCom->Set_CurrentTrackPosition_Boundary((*EvKeyFramesvec)[m_iSelected_Index_KeyFrame].TrackPosition);
		}
	}

		//잘못된 키프레임 삭제
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		(*EvKeyFramesvec).erase((*EvKeyFramesvec).begin() + m_iSelected_Index_KeyFrame);

	}
		//키프레임 정보 확인
	ImGui::Text("\n");
	if ((*EvKeyFramesvec).size() > m_iSelected_Index_KeyFrame)
	{
		ImGui::Text("%f", (*EvKeyFramesvec)[m_iSelected_Index_KeyFrame].TrackPosition);
	}
	else
	{
		--m_iSelected_Index_KeyFrame;
	}
	ImGui::PopItemWidth();

	//콜라이더 관련


}

void CController_AnimationTool::ListUp_Bone()
{
	//이전 선택 저장
	m_iCurSelected_Index_Bone = m_iSelected_Index_Bone;

	ImGui::PushItemWidth(300); // 크기조정
	if (ImGui::BeginListBox("Bone List"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			bool is_selected{ false };
			for (int i = 0; i < m_pCopyBoneVec->size(); i++)
			{
				if (m_iSelected_Index_Bone == i)
				{
					is_selected = true;
				}

				if (ImGui::Selectable((*m_pCopyBoneVec)[i]->Get_Name(), is_selected))
				{
					m_iSelected_Index_Bone = i;
				}

				if (!is_selected)
					ImGui::SetItemDefaultFocus();
				is_selected = false;
				// 반복문으로 리스트박스의 선택된 객체 찾기
			}
		}

		ImGui::EndListBox();
	}
	ImGui::PopItemWidth();

	//뼈 이넘
	ImGui::PushItemWidth(100); // 크기조정
	if (ImGui::BeginListBox("Bone Type"))
	{
		if (ImGui::Selectable("ROOT", (m_iBoneTypeIndex == 0)))
		{
			m_iBoneTypeIndex = 0;
		}
		else
			ImGui::SetItemDefaultFocus();
		if (ImGui::Selectable("HAND_LEFT", (m_iBoneTypeIndex == 1)))
		{
			m_iBoneTypeIndex = 1;
		}
		else
			ImGui::SetItemDefaultFocus();
		if (ImGui::Selectable("HAND_RIGHT", (m_iBoneTypeIndex == 2)))
		{
			m_iBoneTypeIndex = 2;
		}
		else
			ImGui::SetItemDefaultFocus();
		if (ImGui::Selectable("FOOT_LEFT", (m_iBoneTypeIndex == 3)))
		{
			m_iBoneTypeIndex = 3;
		}
		else
			ImGui::SetItemDefaultFocus();
		if (ImGui::Selectable("FOOT_RIGHT", (m_iBoneTypeIndex == 4)))
		{
			m_iBoneTypeIndex = 4;
		}
		else
			ImGui::SetItemDefaultFocus();
		if (ImGui::Selectable("HEAD", (m_iBoneTypeIndex == 5)))
		{
			m_iBoneTypeIndex = 5;
		}
		else
			ImGui::SetItemDefaultFocus();
		if (ImGui::Selectable("WEAPON", (m_iBoneTypeIndex == 6)))
		{
			m_iBoneTypeIndex = 6;
		}
		else
			ImGui::SetItemDefaultFocus();
		if (ImGui::Selectable("BOUNDARY_UPPER", (m_iBoneTypeIndex == 7)))
		{
			m_iBoneTypeIndex = 7;
		}
		else
			ImGui::SetItemDefaultFocus();

		ImGui::EndListBox();
	}


	//이전 선택지 비교 후 애니메이션 전환
	if (m_pCopyModelCom != nullptr)
	{
		if (m_iSelected_Index_Bone != m_iCurSelected_Index_Bone)
		{
			//뼈의 인덱스, 이름 표기, 현재 뼈의 정보
			
		}
	}

	ImGui::PopItemWidth();
	
}

void CController_AnimationTool::SetUp_Controller_Bone()
{
	_int i  = m_pCopyModelCom->Get_BoneIndex(((*m_pCopyBoneVec)[m_iSelected_Index_Bone])->Get_Name()); // << 구해온 뼈 이름으로 구한 인덱스;


	if (ImGui::Button("Save_BoneIndex"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			//모델의 특정 컨테이너에 저장.
			m_pCopyModelCom->Set_UFBIndices(m_iBoneTypeIndex, m_iSelected_Index_Bone);

		}
	}
	ImGui::Text("\n");
	_vector vPos{};
	_float3 vTemp{};
	if (m_pCopyModelCom != nullptr)
	{
		//vPos = ((*m_pCopyBoneVec)[m_iSelected_Index_Bone])->Get_TransformationMatrix().r[CTransform::STATE_POSITION];
		vPos = ((*m_pCopyBoneVec)[m_iSelected_Index_Bone])->Get_CombinedTransformationMatrix().r[CTransform::STATE_POSITION];
	}

	ImGui::Text("Pos_X   %f", XMVectorGetX(vPos));
	ImGui::Text("Pos_Y   %f", XMVectorGetY(vPos));
	ImGui::Text("Pos_Z   %f", XMVectorGetZ(vPos));


	ImGui::PushItemWidth(300); // 크기조정
	if (ImGui::BeginListBox("USEFULLBONE Vec"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			bool is_selected{ false };
			ImGui::Text("ROOT :: %d", m_pCopyModelCom->Get_UFBIndices(0));
			ImGui::Text("HAND_LEFT :: %d", m_pCopyModelCom->Get_UFBIndices(1));
			ImGui::Text("HAND_RIGHT :: %d", m_pCopyModelCom->Get_UFBIndices(2));
			ImGui::Text("FOOT_LEFT :: %d", m_pCopyModelCom->Get_UFBIndices(3));
			ImGui::Text("FOOT_RIGHT :: %d", m_pCopyModelCom->Get_UFBIndices(4));
			ImGui::Text("HEAD :: %d", m_pCopyModelCom->Get_UFBIndices(5));
			ImGui::Text("WEAPON :: %d", m_pCopyModelCom->Get_UFBIndices(6));
			ImGui::Text("BOUNDARY_UPPER :: %d", m_pCopyModelCom->Get_UFBIndices(7));
		}

		ImGui::EndListBox();
	}
	ImGui::PopItemWidth();

	if (ImGui::Button("Update_Boundary"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			//모델의 특정 컨테이너에 저장.
			m_pCopyModelCom->Update_Boundary();

		}
	}

	ImGui::Text("\n");
	if ((*m_pCopyBoneVec)[m_iSelected_Index_Bone]->Get_ParentBoneIndex() != -1)
	{
		ImGui::Text((*m_pCopyBoneVec)[(*m_pCopyBoneVec)[m_iSelected_Index_Bone]->Get_ParentBoneIndex()]->Get_Name());
	}
	else
		ImGui::Text("First_Bone");

	if ((*m_pCopyBoneVec)[m_iSelected_Index_Bone]->Get_IsChildOf_Boundary())
	{
		ImGui::Text("Boundary : true");
	}
	else
	{
		ImGui::Text("Boundary : false");
	}

}

void CController_AnimationTool::ListUp_Virtex()
{
	if (m_pCopyModelCom != nullptr)
	{
		m_pCopyMeshVec = &(m_pCopyModelCom->Get_Meshes());
		//m_pCopyVtxAnimMeshes;
	}


	m_iCurSelected_Index_Mesh = m_iSelected_Index_Mesh;
	ImGui::PushItemWidth(300); // 크기조정
	if (ImGui::BeginListBox("Mesh List"))
	{
		if (m_pCopyMeshVec != nullptr || m_pCopyModelCom != nullptr)
		{
			bool is_selected{ false };
			for (_uint i = 0; i < m_pCopyModelCom->Get_NumMeshes(); i++)
			{
				if (m_iSelected_Index_Mesh == i)
				{
					is_selected = true;
				}
				string strnm;
				
				//예외 처리
				strnm.assign((*m_pCopyMeshVec)[i]->Get_Name());
				
				strnm = strnm + to_string(i);
				
				if (ImGui::Selectable(strnm.c_str(), is_selected))
				{
					m_iSelected_Index_Mesh = i;
				}


				if (!is_selected)
					ImGui::SetItemDefaultFocus();
				is_selected = false;
				// 반복문으로 리스트박스의 선택된 객체 찾기
			}
		}

		ImGui::EndListBox();
	}
	ImGui::PopItemWidth();

	if (m_iCurSelected_Index_Mesh != m_iSelected_Index_Mesh)
	{
		//메쉬 인덱스 변경시의 행동
		m_iCurSelected_Index_Vtx = m_iSelected_Index_Vtx = 0;
	}

	ImGui::Text("Now Selected Mesh : \t");
	ImGui::SameLine();
	ImGui::Text((*m_pCopyMeshVec)[m_iSelected_Index_Mesh]->Get_Name());


	if (m_pCopyMeshVec != nullptr)
	{
		m_pCopyVtxAnimMeshes = (*m_pCopyMeshVec)[m_iSelected_Index_Mesh]->Get_AnimVertices();
	}

	m_iCurSelected_Index_Vtx = m_iSelected_Index_Vtx;
	ImGui::PushItemWidth(300); // 크기조정
	if (ImGui::BeginListBox("Vtx List"))
	{
		if (m_pCopyModelCom != nullptr)
		{
			bool is_selected{ false };	//인디시즈 불러와서 인디시즈에 사용되는 버티시즈 확인하기
			for (_uint i = 0; i < (*m_pCopyMeshVec)[m_iSelected_Index_Mesh]->Get_NumIndices(); i++)
			{
				if (m_iSelected_Index_Vtx == i)
				{
					is_selected = true;
				}

				string strSelecIndex = to_string(i);
				

				if (ImGui::Selectable(strSelecIndex.c_str(), is_selected))
				{
					m_iSelected_Index_Vtx = i;
				}


				if (!is_selected)
					ImGui::SetItemDefaultFocus();
				is_selected = false;
				// 반복문으로 리스트박스의 선택된 객체 찾기
			}
		}

		ImGui::EndListBox();
	}
	ImGui::PopItemWidth();

	if (m_iCurSelected_Index_Vtx != m_iSelected_Index_Vtx)
	{
		//정점 인덱스 변경시의 행동
	}

	_int iTemp = (*m_pCopyMeshVec)[m_iSelected_Index_Mesh]->Get_Indices()[m_iSelected_Index_Vtx];

	string strSelecIndex = to_string(iTemp);

	ImGui::Text("Now Selected Vtx Index : \t");
	ImGui::SameLine();
	ImGui::Text(strSelecIndex.c_str());
	ImGui::Text("\n");


	ImGui::Text("Pos  X : \t %f", m_pCopyVtxAnimMeshes[iTemp].vPosition.x);
	ImGui::Text("Pos  Y : \t %f", m_pCopyVtxAnimMeshes[iTemp].vPosition.y);
	ImGui::Text("Pos  Z : \t %f", m_pCopyVtxAnimMeshes[iTemp].vPosition.z);
	
	_matrix matWorld = XMMatrixIdentity();
	
	matWorld *= m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_AnimationTool_Test"), 0)->Get_Transform()->Get_WorldMatrix();
	
	_vector vPos = XMVector3TransformCoord(XMLoadFloat3(&m_pCopyVtxAnimMeshes[iTemp].vPosition), m_pCopyModelCom->CalcMatrix_forVtxAnim(m_iSelected_Index_Mesh, m_pCopyVtxAnimMeshes[iTemp]));

	m_vPos = XMVector3TransformCoord(vPos, matWorld);
}

void CController_AnimationTool::SetUp_Controller_Vertex()
{
	if (ImGui::Button("Add_UFVtx"))
	{
		UFVTX UFVtx{};
		UFVtx.iMeshNum = m_iSelected_Index_Mesh;
		UFVtx.VtxNum = m_iSelected_Index_Vtx;

		m_pCopyModelCom->Add_UFVtxIndices(UFVtx);
	}

	ImGui::PushItemWidth(300); // 크기조정
	if (ImGui::BeginListBox("UFVtx List"))
	{
		if (m_pCopyMeshVec != nullptr || m_pCopyModelCom != nullptr)
		{
			vector<UFVTX>* vtxs = 	m_pCopyModelCom->Get_UFVtxIndices();

			bool is_selected{ false };
			for (_uint i = 0; i < vtxs->size(); i++)
			{
				if (m_iSelected_Index_UFVtx == i)
				{
					is_selected = true;
				}

				string strnm;
				string strCollon = " : ";
				string strDash = " Mesh  - ";
				string strTale = " Vtx";

				//예외 처리
				
				strnm = to_string(i) + strCollon + to_string((*vtxs)[i].iMeshNum) + strDash + to_string((*vtxs)[i].VtxNum) + strTale;
				

				if (ImGui::Selectable(strnm.c_str(), is_selected))
				{
					m_iSelected_Index_UFVtx = i;
				}


				if (!is_selected)
					ImGui::SetItemDefaultFocus();
				is_selected = false;
				// 반복문으로 리스트박스의 선택된 객체 찾기
			}
		}

		ImGui::EndListBox();
	}
	ImGui::PopItemWidth();

}

void CController_AnimationTool::EndFrame_AnimTool()
{
	Safe_Release(m_pCopyModelCom);
	m_pCopyAnimVec = nullptr;
	m_pCopyBoneVec = nullptr;
}

void CController_AnimationTool::Free()
{
	for (auto& iter : m_Models)
	{
		Safe_Release(iter.second);
	}
	m_Models.clear();


	Safe_Release(m_pGameInstance);
	__super::Free();
}
