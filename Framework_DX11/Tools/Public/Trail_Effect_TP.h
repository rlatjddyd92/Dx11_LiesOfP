#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTrail_TwoPoint_Instance;
class CTexture;
END

BEGIN(Tools)
class CTrail_Effect_TP final :public CEffect_Base
{
public:
	typedef struct
	{
		_uint		iNumInstance = { 10 };
		_float2		vLifeTime = { 2.f, 2.f };
	}BUFFER_DESC;

	typedef struct
	{
		// 움직임
		_Vec3		vTopOffset = {};
		_Vec3		vBottomOffset = {};

		// 셰이더
		_uint		iNumInstance = { 10 };
		_Vec4		vColor = { 0.f, 0.f, 0.f, 1.f };
		_float		fAlphaSpeed = { 1.f };

		// 기타
		_uint		iShaderIndex = { 0 };
		_bool		bLoop = { false };

		_Vec2		vTileRepeat = { 1.f, 1.f };

		_float		fDuration = { 0.f };
		_bool		bFall = { false };
		_float		fFallingSpeed = { 0.f };
	} DEFAULT_DESC;

	typedef struct
	{
		_tchar		szDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_2[MAX_PATH] = L"";
		_tchar		szNormalTextureTag[MAX_PATH] = L"";
	}TEXT_DESC;

	typedef struct : public CEffect_Base::EFFECT_BASE_DESC
	{
		DEFAULT_DESC DefaultDesc = {};
		TEXT_DESC TextDesc = {};
		BUFFER_DESC BufferDesc = {};
	}TRAIL_TP_DESC;


private:
	CTrail_Effect_TP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect_TP(const CTrail_Effect_TP& Prototype);
	virtual ~CTrail_Effect_TP() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset() override;
	virtual HRESULT Save(_wstring strFilePath) override;

	void Set_Loop(_bool bLoop) override {
		m_DefaultDesc.bLoop = bLoop;
		if (true == bLoop)
			Reset();
	}

public:
	void Set_Desc(const TRAIL_TP_DESC& desc);
	TRAIL_TP_DESC Get_Desc() {
		return m_InitDesc;
	}
	TRAIL_TP_DESC* Get_InitDesc_Ptr() {
		return &m_InitDesc;
	}
private:
	class CShader* m_pShaderCom = { nullptr };
	class CTrail_TwoPoint_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr, nullptr, nullptr, nullptr };

	DEFAULT_DESC m_DefaultDesc = {};
	TRAIL_TP_DESC m_InitDesc = {};

	_float m_fAlpha = 0.f;

	_float m_fAccumulateTime = { 0.f };

private:
	HRESULT Ready_Components(const TRAIL_TP_DESC& Desc);

public:
	static CTrail_Effect_TP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
