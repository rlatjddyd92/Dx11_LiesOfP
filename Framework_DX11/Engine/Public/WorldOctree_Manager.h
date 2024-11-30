#pragma once
#include "Base.h"


/* 컴포넌트의 원형을 레벨별로 보관한다. */
/* 복제하여 리턴한다.*/

BEGIN(Engine)

class CWorldOctree_Manager final : public CBase
{
private:
	CWorldOctree_Manager();
	virtual ~CWorldOctree_Manager() = default;

public:
	HRESULT		Initialize();
	void		Update();

	void Create_Octree(_Vec3 vMinPos, _Vec3 vMaxPos);		//옥트리 생성
	//화면에 걸리는 octree 노드 계산 함수
	//m_FrustumCulledNodes를 전달할지 오브젝트꺼를 받아올지 그건 좀 고민 해야함
private:
	vector<_int> m_FrustumCulledNodes;
	class CWorldOctree* m_pWorldOctree = { nullptr };

public:
	static CWorldOctree_Manager* Create();
	virtual void Free() override;
};

END