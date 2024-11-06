#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const { return m_szName; }

	_matrix				Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	void				Set_CombinedTransformationMatrix(_matrix CombinedMatrix) { XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix); }

	const _float4x4*	Get_CombinedTransformationMatrix_Ptr() const { return &m_CombinedTransformationMatrix; }

	_matrix				Get_TransformationMatrix() { return XMLoadFloat4x4(&m_TransformationMatrix); }
	void				Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

	_int				Get_ParentBoneIndex() { return m_iParentBoneIndex; }

	_bool				Get_IsChildOf_Boundary() { return m_bIsChildOf_Boundary; }

public:
	virtual HRESULT Initialize(HANDLE* pFile, _int iParentBoneIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

	void Setting_ParentBoneName(class CModel* pModel);

	void				Update_Boundary(const vector<CBone*>& Bones, _int iCurBoneIndex, _int iBoundaryBoneIndex);

private:
	/* 뼈의 이름 */
	_char		m_szName[MAX_PATH] = {};
	_char		m_szParentName[MAX_PATH] = {};

	/* 부모 기주으로 표현된 이 뼈만의 상태행렬. */
	_float4x4	m_TransformationMatrix = {};

	/* 이 뼈의 최종적인 상태 행렬 ( 내 상태 행렬 * 부모의 최종상태행렬 ) */
	/* m_TransformationMatrix * Parent's m_CombinedTransformationMatrix */
	_float4x4	m_CombinedTransformationMatrix = {};

	_int		m_iParentBoneIndex = { -1 };

	//애니메이션 상하체 분리를 위한 준비
	_bool		m_bIsChildOf_Boundary = {false};

public:
	static CBone* Create(HANDLE* pFile, _int iParentBoneIndex);
	CBone* Clone();
	virtual void Free() override;
};

END