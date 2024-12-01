#pragma once
#include "Base.h"


/* 컴포넌트의 원형을 레벨별로 보관한다. */
/* 복제하여 리턴한다.*/

BEGIN(Engine)

class CWorldOctree_Manager final : public CBase
{
private:
	CWorldOctree_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWorldOctree_Manager() = default;

public:
	HRESULT		Initialize();
	void		Update();
	void Render();

	void Create_Octree(_Vec3 vMinPos, _Vec3 vMaxPos);		//옥트리 생성
	_bool Is_In_FrustumCulledOctree(vector<_int>& pWorldOctreeIndex);
	_bool Is_Active_Octree() { return m_bActive_Octree; }
	void Change_Active_Octree() {m_bActive_Octree = !m_bActive_Octree;}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	unordered_set<_int> m_FrustumCulledNodes;
	class CWorldOctree* m_pWorldOctree = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	_bool m_bActive_Octree = { true };

public:
	static CWorldOctree_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END