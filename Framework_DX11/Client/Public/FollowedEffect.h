#pragma once

#include "Client_Defines.h"
#include "EffectObject.h"


BEGIN(Client)

class CFollowedEffect final : public CEffectObject
{
public:
	typedef struct : EFFECTOBJ_DESC
	{
		const _Matrix* pParentWorldMat = { nullptr };
		const _Matrix* pBoneCombinedMat = { nullptr };

	}FOLLOWEFFOBJ_DESC;

private:
	CFollowedEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFollowedEffect(const CGameObject& Prototype);
	virtual ~CFollowedEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

private:
	const _Matrix* m_pParentWorldMat = { nullptr };
	const _Matrix* m_pBoneCombinedMat = { nullptr };

private:
	HRESULT Ready_Components(_wstring strEffectTag);

public:
	static CFollowedEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END