#pragma once


#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_2DPolygon.h"

#include "Trail_OnePoint_Instance.h"
#include "Trail_TwoPoint_Instance.h"
#include "Trail_MultiPoint_Instance.h"

#include "VIBuffer_Dissolve_Instance.h"

#include "Navigation.h"
#include "Collider.h"
#include "Texture.h"
#include "Shader.h"
#include "Shader_NonVTX.h"
#include "Shader_Compute.h"
#include "Model.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "Fsm.h"
#include "RigidBody.h"
#include "Sound.h"


/* 컴포넌트의 원형을 레벨별로 보관한다. */
/* 복제하여 리턴한다.*/

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	HRESULT Add_ModelPrototype(_uint iLevelIndex, const _char* strPrototypeTag, class CComponent* pPrototype);
	map<const _char*, class CComponent*> Get_ModelPrototypes(_uint iLevelIndex) { return m_pModelPrototypes[iLevelIndex]; }

	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

	class CComponent* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

private:
	map<const _wstring, class CComponent*>*		m_pPrototypes = { nullptr };
	_uint										m_iNumLevels = { 0 };

	//툴 모델 구분용
	map<const _char*, class CComponent*>*		m_pModelPrototypes = { nullptr };

private:
	class CComponent* Find_ModelPrototype(_uint iLevelIndex, const _char* strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END