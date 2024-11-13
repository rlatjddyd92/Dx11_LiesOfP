#pragma once
#include "Base.h"


/* 컴포넌트의 원형을 레벨별로 보관한다. */
/* 복제하여 리턴한다.*/

BEGIN(Engine)

class CInstance_Manager final : public CBase
{
	enum INSTANCEDRAW_TYPE { BASE, DRAW_END };
private:
	CInstance_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CInstance_Manager() = default;

public:
	HRESULT Initialize();
	class CModel* Add_NonAnimModel(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear();

	HRESULT Draw(_uint iPass);
	HRESULT Draw_Shadow();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance*						m_pGameInstance = { nullptr };
	map<const _wstring, class CModel*>			m_pNonAnimModels;

	class CShader*								m_pNonAnimInstanceShader = { nullptr };

private:
	class CModel* Find_NonAnimModel(const _wstring& strPrototypeTag);

public:
	static CInstance_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END