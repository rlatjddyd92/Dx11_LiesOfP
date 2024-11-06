#pragma once
#include "Base.h"
#include <fstream>
#include <sstream>

BEGIN(Engine)

class CCSVFileManager final : public CBase
{
public:
	

private:
	CCSVFileManager();
	virtual ~CCSVFileManager() = default;


public:
	HRESULT		Initialize();
	HRESULT		FileLoadStart(const _char* FilePath, _int iDataStartRow);
	_bool		GetDataByRow(_char** DataBuffer, _int iDataColumn, _int iDataTextMaxLength = 200);
	void		FileLoadEnd();


private:
	_char m_szFilePath[100] = "none";
	ifstream m_CurrentFile;


public:
	static CCSVFileManager* Create();
	virtual void Free() override;

};

END