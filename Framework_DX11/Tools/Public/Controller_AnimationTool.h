#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_AnimationTool : public CBase
{
	DECLARE_SINGLETON(CController_AnimationTool)
private:
	CController_AnimationTool();
	virtual ~CController_AnimationTool() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void				SetUp_Lump(_int iNumAnimModel) { pLumpModelpaths.pLump.resize(iNumAnimModel); };
	FilePathStructStack* Get_StructStack(_int iModelCnt) { return &pLumpModelpaths.pLump[iModelCnt]; }

	void	SetUp_AnimTool();

	void	ListUp_Anim();
	void	SetUp_Controller_Anim();

	void	ListUp_Bone();
	void	SetUp_Controller_Bone();

	void	ListUp_Virtex();
	void	SetUp_Controller_Vertex();

	void	EndFrame_AnimTool();
	void	BlockObjCtr() { m_bObjRenderCtr = false; };

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	class CModel*			m_pCopyModelCom = { nullptr };
	vector<CAnimation*>*	m_pCopyAnimVec = { nullptr };
	vector<CBone*>*			m_pCopyBoneVec = { nullptr };
	vector<class CMesh*>*	m_pCopyMeshVec = { nullptr };
	VTXANIMMESH*			m_pCopyVtxAnimMeshes = { nullptr };

	vector<const _char*>			m_ModelNames;
	map<string, CModel*>	m_Models;
	 
private:
	StructStacklump			pLumpModelpaths;

private:

	//리스트 순회, 및 번호 확인용
	_uint m_iSelected_Index_Model{};
	_uint m_iCurSelected_Index_Model{};
	_uint m_iSelected_Index_Anim{};
	_uint m_iCurSelected_Index_Anim{};		//이전 선택지와 비교하는 용도
	_uint m_iSelected_Index_Anim_Boundary{};
	_uint m_iCurSelected_Index_Anim_Boundary{};

	_uint m_iSelected_Index_Bone{};
	_uint m_iCurSelected_Index_Bone{};		//이전 선택지와 비교하는 용도

	_uint m_iSelected_Index_KeyFrame{};
	_uint m_iCurSelected_Index_KeyFrame{};

	_uint m_iSelected_Index_Mesh{};
	_uint m_iCurSelected_Index_Mesh{};

	_uint m_iSelected_Index_Vtx{};
	_uint m_iCurSelected_Index_Vtx{};

	_uint m_iSelected_Index_UFVtx{};
	_uint m_iCurSelected_Index_UFVtx{};

	_uint m_iBoneTypeIndex{};

	//입력값 혹은 출력값
	_double m_AnimSpeedPS{};
	_double m_AnimDuration{};
	_float	m_fAnimTrackPosition{};

	_char	m_szEvKeyFrameText[MAX_PATH] = {""};

	_char	m_szCurrentModelText[MAX_PATH] = { "" };

	//조건 적용용 불변수
	_bool	m_bDivide_Boundary { false };
	_bool	m_bObjRenderCtr {true};
	_bool	m_bIsAnimStopped { false };
	_bool	m_bTargetBallRender{ false };

	_vector	m_vPos{};

public:
	virtual void Free() override;
};

END

