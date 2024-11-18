#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTrail_OnePoint_Instance;
class CTexture;
END

BEGIN(Client)
class CTrail_Effect_OP final : public CEffect_Base
{
public:
	enum TRAIL_OP_TYPE
	{
		TYPE_UPDATE, TYPE_SPREAD, TYPE_END
	};

	enum TRAIL_OP_STATE
	{
		TOP_GROW = 0x0001,
		TOP_SHRINK = 0x0002,
		TOP_END
	};

	typedef struct
	{
		_uint		iNumInstance = { 20 };
		_float2		vLifeTime = { 2.f, 2.f };
	}BUFFER_DESC;

	typedef struct
	{
		// 움직임 제어용
		TRAIL_OP_TYPE	eType = TYPE_END;
		_float			fTrailInterval = { 0.1f };
		_float			fSpreadSpeed = { 0.f };
		_Vec3			vPos = { 0.f, 0.f, 0.f };
		// 셰이더 전달용
		_uint		iGeomState = { 0 };
		_Vec4		vColor = { 0.f, 0.f, 0.f, 1.f };

		_float		fScaling = { 1.f };
		_Vec2		vDivide = { 1.f, 1.f };
		_float		fSpriteSpeed = { 0.f };

		// 기타
		_uint		iShaderIndex = { 0 };
		_bool		bLoop = { false };
	} DEFAULT_DESC;


	typedef struct
	{
		_tchar		szDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_2[MAX_PATH] = L"";
		_tchar		szNormalTextureTag[MAX_PATH] = L"";
	} TEXT_DESC;

	typedef struct : public CEffect_Base::EFFECT_BASE_DESC
	{
		DEFAULT_DESC	DefaultDesc = {};
		TEXT_DESC		TextDesc = {};
		_wstring		strVIBufferTag = TEXT("");
	}TRAIL_OP_DESC;

private:
	CTrail_Effect_OP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect_OP(const CTrail_Effect_OP& Prototype);
	virtual ~CTrail_Effect_OP() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset() override;
	virtual void Set_Loop(_bool bLoop) override {
		m_DefaultDesc.bLoop = bLoop;
		if (true == bLoop)
			Reset();
	}

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTrail_OnePoint_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr, nullptr, nullptr, nullptr };

	DEFAULT_DESC m_DefaultDesc = {};
	DEFAULT_DESC m_InitDesc = {};


private:
	HRESULT Ready_Components(const TRAIL_OP_DESC& Desc);

public:
	static CTrail_Effect_OP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END