#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader_NonVTX;
class CTrail_MultiPoint_Instance;
class CTexture;
class CShader_Compute;
END

BEGIN(Tools)
class CTrail_Effect_MP final : public CEffect_Base
{
public:
	typedef struct
	{
		// 움직임
		PARTICLE_TYPE		eType = { PT_END };
		_uint				iComputeState = { 0 };
		_float				fGravity = { 0.f };
		_Vec4				vPivot = { 0.f, 0.f, 0.f, 1.f };
		_Vec4				vMoveDir = { 0.f, -1.f, 0.f, 0.f };

		_Vec3				vOrbitAxis = { 0.f, 1.f, 0.f };
		_float				fOrbitAngle = { 90.f };

		_float				fRandomTimeInterval = { 0.f };
		_float				fRandomMoveRatio = { 0.f };

		_float				fAccelSpeed = { 0.f };
		_float				fAccelLimit = { 0.f };

		_float				fTailInterval = { 0.1f };	// 꼬리와 꼬리 사이 간격
		_float				fTailDistance = { 0.5f };	// 꼬리와 헤드 사이 간격 : 이걸 늘리면 더 렌덤하게 감.
		_Vec4				vTailMoveDir = { 0.f, 1.f, 0.f, 0.f };
		
		// 위치 초기화
		_Vec3				vPos = {};
		_Vec3				vRotation = {};
		_Vec3				vScale = { 1.f, 1.f, 1.f };

		// 셰이더
		_uint				iShaderIndex = { 0 };
		_uint				iGeomState = { 0 };
		_Vec2				vTexDevide = { 1.f ,1.f };
		_float				fSpriteSpeed = { 0.f };
		_Vec2				vScaling = { 1.f, 1.f };
		_float				fStartRotation = { 0.f };
		_float				fRotationPerSecond = { 0.f };
	}DEFAULT_DESC;

	typedef struct
	{
		_tchar		szDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_2[MAX_PATH] = L"";
		_tchar		szNormalTextureTag[MAX_PATH] = L"";
	} TEXT_DESC;

	typedef struct
	{
		_uint		iNumInstance = { 100 };
		_Vec3		vCenter = {};
		_Vec3		vRange = { 1.f, 1.f, 1.f };
		_Vec3		vExceptRange = {};
		_Vec2		vSize = { 0.5f, 0.1f };
		_Vec2		vSpeed = { 1.f, 2.f };
		_Vec2		vLifeTime = { 2.f, 4.f };
		_Vec4		vMinColor = { 0.f, 0.f, 0.f, 1.f };
		_Vec4		vMaxColor = { 0.f, 0.f, 0.f, 1.f };
		_uint		iTail_NumInstance = { 100 };
		_Vec2		vTail_Speed = { 1.f, 2.f };
		_Vec2		vTail_Size = {0.1f, 0.2f};
		_Vec2		vTail_LifeTime = { 0.5f, 1.f };
		_Vec4		vTail_MinColor = { 0.f, 0.f, 0.f, 1.f };
		_Vec4		vTail_MaxColor = { 1.f, 1.f, 1.f, 1.f };
	}BUFFER_DESC;

	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		DEFAULT_DESC DefaultDesc = {};
		TEXT_DESC TextDesc = {};
		BUFFER_DESC BufferDesc = {};
	}TRAIL_MP_DESC;

private:
	CTrail_Effect_MP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect_MP(const CTrail_Effect_MP& Prototype);
	virtual ~CTrail_Effect_MP() = default;

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
	void Set_Loop(_bool bLoop) override;
	void Set_Desc(TRAIL_MP_DESC Desc);
	TRAIL_MP_DESC Get_Desc() {
		return m_InitDesc;
	}
	TRAIL_MP_DESC* Get_InitDesc_Ptr() {
		return &m_InitDesc;
	}

private:
	class CShader_NonVTX* m_pHeadShaderCom = { nullptr };				// 테일만.
	class CShader_NonVTX* m_pTailShaderCom = { nullptr };				// 테일만.
	class CTrail_MultiPoint_Instance* m_pVIBufferCom = { nullptr };		
	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr, nullptr, nullptr, nullptr };

	class CShader_Compute* m_pSpreadCS = { nullptr };
	class CShader_Compute* m_pConvergeCS = { nullptr };
	class CShader_Compute* m_pMoveCS = { nullptr };

	// 초기화.
	class CShader_Compute* m_pResetCS = { nullptr };

private:
	DEFAULT_DESC m_DefaultDesc = {};
	TRAIL_MP_DESC m_InitDesc = {};


	_uint m_iTest = { 0 };
private:
	HRESULT Ready_Components(const TRAIL_MP_DESC& Desc);

public:
	static CTrail_Effect_MP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
