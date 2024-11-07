#pragma once
#include "Base.h"


/* 컴포넌트의 원형을 레벨별로 보관한다. */
/* 복제하여 리턴한다.*/

BEGIN(Engine)

class Instance_Manager final : public CBase
{
private:
	Instance_Manager();
	virtual ~Instance_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_NonAnimModel(const _wstring& strPrototypeTag, class CModel* pModel);
	void Clear();

private:
	map<const _wstring, class CModel*>			m_pNonAnimModles;
private:
	class CModel* Find_NonAnimModel(const _wstring& strPrototypeTag);

public:
	static Instance_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END