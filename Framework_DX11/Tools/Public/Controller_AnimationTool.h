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

public:
	vector<_wstring>& Get_PrototypeTags() { return m_PrototypeTags; }
	_wstring& Get_PrtotypeTagIndex(_uint iIndex) { return m_PrototypeTags[iIndex]; }

	void					Add_ProtytypeTag(_wstring strTag) { m_PrototypeTags.emplace_back(strTag); }

private:
	CController_AnimationTool();
	virtual ~CController_AnimationTool() = default;

public:
	HRESULT Initialize();

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_PrototypeTags;

public:
	virtual void Free() override;
};

END

