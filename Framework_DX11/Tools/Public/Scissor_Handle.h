#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Tools)

class CScissor_Handle final : public CGameObject
{
public:
	enum SCISSOR_TYPE { SCISSOR_LEFT, SCISSOR_RIGHT, SCISSOR_END };

	typedef struct 
	{
		SCISSOR_TYPE eScissorType;
	}SCISSOR_DESC;

private:
	CScissor_Handle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScissor_Handle(const CScissor_Handle& Prototype);
	virtual ~CScissor_Handle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_vector		m_vRootMoveStack{};

	_float		m_fEmissive = {};
	_float		m_fRimLight = {};

	_bool		m_bRender = { false };

	_bool		m_bRemoteTuning = true;

	_bool		m_bSubRootMove = false;
	_int		m_iSubRootBone{};
	_vector		m_vSubRootMoveStack{};

	class CScissor_Blade* m_pBlade = { nullptr, };

	SCISSOR_TYPE		m_eType = { SCISSOR_END };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Blade();

	HRESULT Change_ScissorType();

public:
	static CScissor_Handle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END