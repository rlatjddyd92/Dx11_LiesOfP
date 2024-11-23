#pragma once

#include "Base.h"

/* 1. 객체들을 그리는 순서에 따라서 보관하고. 정해진 순서대로 객체들의 렌더함수를 호출한다.  */
/* 2. 3D라고 하더라도 객체들을 그리는 순서가 상당히 중요하다. */
/* 3. 하늘, 블렌딩, UI관련된 객체들은 그리는 순서의 관리가 필수적으로 필요하다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_HEIGHT, RG_SHADOWOBJ, RG_NONBLEND, RG_NONLIGHT, RG_EFFECT, RG_BLEND, RG_PICKING, RG_UI, RG_DECAL, RG_DISTORTION, RG_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	SSAO_DESC*		Get_SSAODesc() { return &m_tSSAO; }
	HDR_DESC*		Get_HDRDesc() { return &m_tHDR; }
	BLOOM_DESC*		Get_BloomDesc() { return &m_tBloom; }
	DOF_DESC*		Get_DOFDesc() { return &m_tDOF; }
	RADIAL_DESC*	Get_RadialDesc() { return &m_tRadial; }

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	HRESULT Draw();

#ifdef _DEBUG

public:
	HRESULT Add_DebugObject(class CComponent* pDebugObject) {
		if (!m_isDebugRender)
			return S_OK;

		Safe_AddRef(pDebugObject);
		m_DebugObjects.push_back(pDebugObject);
		return S_OK;
	} 

#endif
private:
	// 다음 단계에 영향을 주지 않기 위해 초기화 용도의 SRV와 UAV
	ID3D11ShaderResourceView* m_pClearSRV[128] = { nullptr };
	ID3D11UnorderedAccessView* m_pClearUAV[8] = { nullptr };

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11DepthStencilView*		m_pLightDepthStencilView = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	_float2						m_vWinSize = { 1280.f, 720.f };
	
	list<class CGameObject*>	m_RenderObjects[RG_END];
	list<CGameObject*>			m_InstanceRenderObjects[RG_END];

private:
	class CShader*				m_pShader = { nullptr };
	class CShader*				m_pSSAOShader = { nullptr };
	class CShader*				m_pDistortionShader = { nullptr };
	class CShader*				m_pPostProcessShader = { nullptr };

	// 컴퓨트
	class CShader*				m_pBackShader = { nullptr };
	class CShader*				m_pHDRShader = { nullptr };
	class CShader*				m_pBloomShader = { nullptr };
	class CShader*				m_pBlurShader = { nullptr };

	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

	/* Bloom */
	_float						m_fSamplerRatio = { 6.f };
	BLOOM_DESC					m_tBloom;

	ID3D11DepthStencilView*		m_pDownSampleDepthStencilView0 = { nullptr };
	ID3D11DepthStencilView*		m_pDownSampleDepthStencilView1 = { nullptr };
	ID3D11DepthStencilView*		m_pDownSampleDepthStencilView2 = { nullptr };

	_bool						m_isUseBloom = { true };

	/* SSAO */
	SSAO_DESC					m_tSSAO;
	ID3D11Buffer*				m_pBuffer_SSAO = { nullptr };
	class CTexture*				m_pNoiseTexture_SSAO = { nullptr };

	/* Cascade */
	ID3D11DepthStencilView*		m_pCascadeDepthStencilViewArr = { nullptr };

	/* HDR */
	HDR_DESC					m_tHDR;

	/* DOF */
	DOF_DESC					m_tDOF;

	/* Radial Blur*/
	RADIAL_DESC					m_tRadial;

#ifdef _DEBUG
private:
	list<class CComponent*>		m_DebugObjects;
	_bool						m_isDebugRender = { false };
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_Height();
	HRESULT Render_ShadowObj();
	HRESULT Render_NonBlend();
	HRESULT Render_Decal();
	HRESULT Render_Lights();

	HRESULT Render_Deferred();

	HRESULT Render_SSAO();
	HRESULT Render_HDR();
	HRESULT Render_Bloom_Compute();
	HRESULT Render_Bloom();
	HRESULT Render_DOF();
	HRESULT Render_Radial();

	HRESULT Render_Effect();
	HRESULT Render_Distortion();

	HRESULT Render_Final();

	HRESULT Render_Picking();

	HRESULT Render_NonLights();
	HRESULT Render_Blend();
	HRESULT Render_UI();

	HRESULT Render_Cascade();

private:
	HRESULT Ready_LightDepthStencilView();
	HRESULT Copy_BackBuffer();
	HRESULT Ready_CascadeDepthStencilView();
	HRESULT Ready_SSAO();
	HRESULT Ready_HDR();
	HRESULT Ready_Bloom();
	HRESULT Ready_DOF();

	HRESULT Ready_Desc();


	HRESULT Ready_Shader();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();

#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END