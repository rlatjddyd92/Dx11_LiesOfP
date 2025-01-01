#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CVIBuffer_2DPolygon;
END

BEGIN(Tools)


class CUIRender_3D final : public CGameObject
{

private:
	CUIRender_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIRender_3D(const CUIRender_3D& Prototype);
	virtual ~CUIRender_3D() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	vector<CVIBuffer_2DPolygon*> m_vecVIBuffer_2DPolygon_Com;

private:
	HRESULT Ready_Components();


public:
	static CUIRender_3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END