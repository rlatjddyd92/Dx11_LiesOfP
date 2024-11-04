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
	HRESULT Initialize();

public:
	void	SetUp_AnimTool();

	void	ListUp_Anim();
	void	SetUp_Controller_Anim();

	void	ListUp_Bone();
	void	SetUp_Controller_Bone();

	void	Ready_PickModel();
	void	SetUp_Controller_Vertex();

private:
	vector<CAnimation*>* m_pCopyAnimVec = {nullptr};
	vector<CBone*>* m_pCopyBoneVec = { nullptr };
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CModel* m_pCopyModelCom = {nullptr};

	_uint m_iSelected_Index_Anim{};
	_uint m_iCurSelected_Index_Anim{};		//이전 선택지와 비교하는 용도

	_uint m_iSelected_Index_Bone{};
	_uint m_iCurSelected_Index_Bone{};		//이전 선택지와 비교하는 용도

	_uint m_iBoneTypeIndex{};

	_double m_AnimSpeedPS{};


public:
	virtual void Free() override;
};

END

