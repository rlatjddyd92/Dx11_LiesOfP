#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Tools)

class CTargetBall final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_bool* pUpdateCtr	= { nullptr };
		_vector* pPos		= { nullptr };
	}TargetBall_DESC;

private:
	CTargetBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTargetBall(const CTargetBall& Prototype);
	virtual ~CTargetBall() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

	_vector*		m_pTargetPos{ nullptr };
	_bool*			m_pUpdateCtr{ nullptr };

private:
	BoundingSphere* m_pOriginalBoundingDesc = { nullptr };
	BoundingSphere* m_pBoundingDesc = { nullptr };

private:
	HRESULT Ready_Components();
	virtual class CComponent* Change_Component(const _wstring& strComponentTag, CComponent* pComponent, _uint iPartObjIndex = 0) { return nullptr; };//받아온 컴포넌트로 교체하고 이전의 컴포넌트를 반환

public:
	static CTargetBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END