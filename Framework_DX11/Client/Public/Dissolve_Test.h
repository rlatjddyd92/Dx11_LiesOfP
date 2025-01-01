#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader_NonVTX;
class CVIBuffer_Dissolve_Instance;
class CTexture;
class CShader_Compute;
class CModel;
END

BEGIN(Client)
class CDissolve_Test final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		class CModel* pModelCom = { nullptr };
		class CTransform* pPlayerTransformCom = { nullptr };
	}DISSOLVE_OBJECT_DESC;

private:
	CDissolve_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDissolve_Test(const CDissolve_Test& Prototype);
	virtual ~CDissolve_Test() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader_NonVTX*				m_pShaderCom = { nullptr };
	// 모델 만들 때 넣어준 테그 그대로 쓰고
	class CVIBuffer_Dissolve_Instance*	m_pVIBufferCom = { nullptr };
	// 이건 뭐 알아서 하고
	class CTexture*						m_pTextureCom = nullptr;
	// 디졸브 컴퓨트 셰이더
	class CShader_Compute*				m_pActionCS = { nullptr };

	// 플레이어 모델
	class CModel*						m_pModelCom = { nullptr };
	// 플레이어 트렌스폼
	class CTransform*					m_pPlayerTransformCom = { nullptr };

	// 렌더 셰이더에 전달
	_uint								m_iShaderIndex = { 0 };
	_uint								m_iGeomState = { 0 };
	_Vec2								m_vTexDevide = { 1.f ,1.f };
	_float								m_fSpriteSpeed = { 0.f };
	_Vec2								m_vStartScaling = { 1.f, 1.f };
	_Vec2								m_vScalingRatio = { 0.f, 0.f };
	_float								m_fStartRotation = { 0.f };
	_float								m_fRotationPerSecond = { 0.f };

private:
	HRESULT Ready_Componet();

public:
	static CDissolve_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END