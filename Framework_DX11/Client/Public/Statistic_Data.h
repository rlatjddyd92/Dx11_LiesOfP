#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)

END




BEGIN(Client)

class CStatistic_Data : public CBase
{
private:
	CStatistic_Data();
	virtual ~CStatistic_Data() = default;



public:
	virtual void Free() override;

};

END
