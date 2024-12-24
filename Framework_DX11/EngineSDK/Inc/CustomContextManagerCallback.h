#pragma once
#include "Base.h"

BEGIN(Engine)

class CCustomContextManagerCallback : public DxContextManagerCallback
{
public:
	CCustomContextManagerCallback(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// DxContextManagerCallback을(를) 통해 상속됨
	virtual void acquireContext() override;
	virtual void releaseContext() override;
	virtual ID3D11Device* getDevice() const override;
	virtual ID3D11DeviceContext* getContext() const override;
	virtual bool synchronizeResources() const override;

public:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	bool mSynchronizeResources = false;
};

END