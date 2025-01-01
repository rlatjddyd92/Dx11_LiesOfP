#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_3D : public CBase
{
protected:
	CUIPage_3D();
	virtual ~CUIPage_3D() = default;










private: // <- 디바이스 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END