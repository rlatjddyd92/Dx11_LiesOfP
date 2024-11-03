#pragma once

#include "Base.h"

/* 뷰, 투영행렬를 보관한다. */
/* 매 프레임마다 Update함수를 호출하면서 들고있었던 뷰,투영행렬의 역행렬을  카메라의 위치 미리 구해둔다. */

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
	}

public:
	const _Matrix& Get_Transform(D3DTRANSFORMSTATE eState) const {
		return m_TransformMatrices[eState];
	}
	const _Matrix& Get_Transform_Inverse(D3DTRANSFORMSTATE eState) const {
		return m_TransformInverseMatrices[eState];
	}

	const _Vec4& Get_CamPosition_Vec4() const {
		return m_vCamPosition; }
	const _Vec3& Get_CamPosition_Vec3() const {
		return _Vec3(m_vCamPosition);
	}


public:
	HRESULT Initialize();
	void Update();

private:
	_Matrix			m_TransformMatrices[D3DTS_END];
	_Matrix			m_TransformInverseMatrices[D3DTS_END];
	_Vec4			m_vCamPosition = {};

public:
	static CPipeLine*	Create();
	virtual void Free() override;
};

END