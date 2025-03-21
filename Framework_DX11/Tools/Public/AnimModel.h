#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Tools)

class CAnimModel final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_char		szLayerTag[MAX_PATH];
		_char		szPrototypeTag[MAX_PATH];
		_char		szModelTag[MAX_PATH];
		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;

		//이봉준 추가
		_bool*		pUpdateCtr;
		_bool* pRenderCtr_TB;
		_vector* pPos_TB;
	}ANIMMODEL_DESC;

private:
	CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimModel(const CAnimModel& Prototype);
	virtual ~CAnimModel() = default;

public:
	ANIMMODEL_DESC Get_NonAniModelDesc() { return m_tDesc; }
	void Set_NonAniModelDesc(ANIMMODEL_DESC* Desc) { memcpy(&m_tDesc, Desc, sizeof(ANIMMODEL_DESC)); }
	void Set_SubRootBone(_int iBoneIndex) { m_iSubRootBone = iBoneIndex; }
	void ChangeAnim(_int iAnimIndex);

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

	ANIMMODEL_DESC	m_tDesc;
	_vector		m_vRootMoveStack{};

	_float		m_fEmissive = {};
	_float		m_fRimLight = {};

	_bool		m_bRender = { true };

	_bool		m_bRemoteTuning = true;

	_bool		m_bSubRootMove = false;
	_int		m_iSubRootBone{};
	_vector		m_vSubRootMoveStack{};

private:
	HRESULT Ready_Components(ANIMMODEL_DESC* pNonAnimDesc);
	virtual class CComponent* Change_Component(const _wstring& strComponentTag, CComponent* pComponent, _uint iPartObjIndex = 0);//받아온 컴포넌트로 교체하고 이전의 컴포넌트를 반환

public:
	static CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END