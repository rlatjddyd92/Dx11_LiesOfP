#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

#include "UIPage_Defines.h"
#include "Interface_Enums.h"

// [UIRender_Batching]
// 
// 1. UI 렌더 구조가 이원화되고 복잡해지는 문제 해결 -> 일원화 
// 2. 배치 형식으로 드로우 콜 최소화 (목표 1회) -> 최적화 
// 
// 방식 
// 1. 모든 UI 객체가 UIRender_Batching에 렌더 정보를 보냄 
// 2. UI 렌더는 취합된 정보를 무조건 입력 순서대로 처리하고 지움 (queue)
// 3. 배칭 방식으로 하나의 버퍼에 찍어 한번에 처리 
//  (스크롤 객체 등은 한 번 더 찍어야 할 수 있음)
// 
// 원칙 
// 1. 절대 렌더가 UI 위치, 색상 조정 등에 대한 추가 연산하지 않는다 (반드시 렌더만 진행)
//  -> Move,Bar에 따른 계산도 다 해서 넘길 것
// 2. UIRender_Batching내부 구조체에 그려야 할 사항만 적어 넣고 이후에는 렌더가 배칭 및 렌더만 한다 
//  -> 이를 통해 정보 전달은 느려지지만(레퍼런스 -> 정보 복사) 렌더링은 더 빠르게 한다 
//  -> 이와 함께 여러 방식으로 분화되고 있는 UI 렌더 방식을 통일하여 코드를 단순화 한다 
//  -> 특히 직교 투영, 드랍/상태이상 안내/인벤토리 등 반복적/유동적으로 그려야 하는 객체를 효율적으로 관리한다 
// 
// 
// 
// 
// 
// 
// 
// 
// 







BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)


class CUIRender_Batching final : public CUIObject
{
public:
	typedef struct UITEXTURE_INFO
	{
		_tchar* strTexturePath = {};
		_tchar* strTextureTag = {};
		CTexture* Texture = { nullptr };
	}UTEXTURE;

	typedef struct UIRENDER_INFO
	{
		// UI 객체의 종류 
		_bool bIsItem = false;

		// 그릴 내용 
		_Vec2 vPosition = { 0.f,0.f };
		_Vec2 vSize = { 0.f,0.f };

		_int iTexture = -1;
		_bool bIsMultiple = false;
		_Vec4 vColor_Texture = { 0.f,0.f,0.f,0.f };
		_float fAlpha_Strash = 0.3f;

		TEXT_TYPE eText_Type = TEXT_TYPE::TEXT_END;
		_int iFont = 14;
		_wstring strText = {};
		_Vec4 vColor_Text = { 0.f,0.f,0.f,0.f };

		_float fTurn = 0.f;

		// 스크롤 영역
		SCROLL_AREA eArea = SCROLL_AREA::SCROLL_END;

	}URENDER;

	enum class UI_SHADER
	{
		SHADER_NORMAL,
		SHADER_CHANGE_COLOR,
		SHADER_MULTIPLE_COLOR,
		SHADER_END
	};



private:
	CUIRender_Batching(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIRender_Batching(const CUIRender_Batching& Prototype);
	virtual ~CUIRender_Batching() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



public:
	void Input_Render_Info(UIRENDER_INFO* pInfo)
	{
		if (pInfo == nullptr)
			return;

		m_queueRender.push(pInfo);
	}


	HRESULT Make_Texture(_int iTextureIndex);
	HRESULT Make_Texture_Item(_int iTextureIndex);

	HRESULT Render_Part(CUIPage::UPART& pPart, CUIPage& pPage, _bool bTopMove);

	void Set_Scroll_Area(SCROLL_AREA eArea, _Vec2 vPos, _Vec2 vSize);
	void Select_Scroll_Area(SCROLL_AREA eArea) 
	{ 
		m_eNow_Area = eArea;
		m_pContext->RSSetViewports(1, m_vecViewPort[_int(m_eNow_Area)]);
	}

	void Set_Scroll_Y_Offset(SCROLL_AREA eArea, _float fOffset) { m_vecfScrollY_Offset_Max[_int(eArea)] = fOffset; }

public:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };



	class CVIBuffer_Rect* m_pVIBufferCom_FullScreen = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom_Scroll = { nullptr };
private:
	HRESULT Ready_Components();

	void Ready_Font();
	HRESULT Ready_Texture_UIPart();
	HRESULT Ready_Texture_ItemIcon();

	queue<UIRENDER_INFO*> m_queueRender; // <- 이번 프레임에 그릴 내용, 

	vector<UTEXTURE*> m_vecTextureInfo_UIPart;
	vector<UTEXTURE*> m_vecTextureInfo_ItemIcon;
	vector<_tchar*> m_vecFont_tchar;

	vector<class CShader*> m_vecShader_UI;
	vector<D3D11_VIEWPORT*> m_vecViewPort;

	SCROLL_AREA m_eNow_Area = SCROLL_AREA::SCROLL_NONE;
	vector<_float> m_vecfScrollY_Offset_Max;



	// 아래 내용은 다음 단계 

	ID3D11ShaderResourceView* m_Array_UI_Texture[MAX_TEXTURES_UI]; // 텍스처의 SRV 배열
	ID3D11ShaderResourceView* m_Array_Item_Texture[MAX_TEXTURES_ITEM]; // 텍스처의 SRV 배열
	ID3D11SamplerState* m_pSamplerState;

public:
	static CUIRender_Batching* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END