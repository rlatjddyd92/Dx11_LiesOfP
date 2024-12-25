#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CRaxasia_Sword_CutScene final : public CGameObject
{
public:
	typedef struct
	{
		const _Matrix* pParentWorldMatrix = { nullptr };
		const _Matrix* pSocketBoneMatrix = { nullptr };
	}WEAPON_DESC;

	enum EFFECT_TYPE 
	{ 
		EFFECT_P1_DRAG, EFFECT_P1_WEAPON_ELECTRIC,

		EFFECT_P2_TURN, EFFECT_P2_LIGHTNING_SMALL, EFFECT_P2_LIGHTNING, EFFECT_P2_SHINE, EFFECT_P2_EXPLOSION, 
		
		EFFECT_END };

public:
	class CModel* Get_Model() { return m_pModelCom; }

	void	Set_Offset(_Vec3 vOffset) { m_vOffset = vOffset; }

	void	Set_Pos(_Vec3 vPos) {
		m_WorldMatrix._41 = vPos.x;
		m_WorldMatrix._42 = vPos.y;
		m_WorldMatrix._43 = vPos.z;
	}

private:
	CRaxasia_Sword_CutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaxasia_Sword_CutScene(const CRaxasia_Sword_CutScene& Prototype);
	virtual ~CRaxasia_Sword_CutScene() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;


public:
	void		Play_Animation(_char* szAnimationName, _float fSpeedRation = 1.f);
	void		Stop_Animation();

	void		Change_SocketMatrix(const _Matrix* pSocketMatrix);

	void		Start_UpdatePos();
	void		Stop_UpdatePos();

	void		DeActive_AllEffect();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	class CEffect_Manager*				m_pEffect_Manager = { nullptr };

	vector<class CEffect_Container*>	m_Effects;
	
	_uint			m_iAnimation_Phase1Index = {};
	_uint			m_iAnimation_Phase2Index = {};

	_int			m_iFrame = {};
	_int			m_iTurnEffectFrame[11] = {};
	_int			m_iInsertEffectFrame[11] = {};

	_float           m_fEmissiveMask = {};

	_Vec4            m_vRimLightColor = {};

	_bool			m_isPlayAnimation = {};
	_bool			m_isUpdatePos = {};

	_bool			m_isActiveTurnEffect[11] = {false, };
	_bool			m_isActiveInsertEffect[11] = {false, };
	_bool			m_isEndPhase1Effect = { false };
	_bool			m_isActiveCutSceneDrag = { false };
	_bool			m_isDeActiveCutSceneDrag = { false };
	_bool			m_isActiveStandElecEffect = { false };
	
	_float			m_fDragTime = {};

	_bool			m_isActiveElecTurnEffect = { false };
	_bool			m_isActiveShineEffect = { false };
	_bool			m_isActiveExplosionEffect = { false };
	_bool			m_isActiveLightingSmallEffect = { false };

	_Matrix					m_WorldMatrix = {};
	const _Matrix*			m_pParentMatrix = { nullptr };
	const _Matrix*			m_pSocketMatrix = { nullptr };

	_Vec3					m_vOffset = {};

private:
	void			Control_Phase1Effect(_float fTimeDelta);
	void			Control_Phase2Effect(_float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Ready_Effect();


public:
	static CRaxasia_Sword_CutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END