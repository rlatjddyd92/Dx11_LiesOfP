#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Render(const _tchar * pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
	HRESULT Render_Center(const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
	// 24-11-22 김성용 
	// 오른쪽 정렬로 텍스트 그리기
	HRESULT Render_Right(const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);

	// 24-12-26 김성용 
	// 텍스트 크기 확인
	_Vec4 Measure_Font(_wstring strText);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	SpriteBatch*				m_pBatch = { nullptr };
	SpriteFont*					m_pFont = { nullptr };

	ID3D11BlendState*			m_pBlendState = { nullptr };
	ID3D11SamplerState*			m_pSamplerState = { nullptr };

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

END