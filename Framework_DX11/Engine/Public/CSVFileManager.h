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
	void		FileLoadStart(const _char* FilePath, _int iDataStartRow);
	void		GetDataByRow(_char* DataBuffer, _int iDataColumn);
	void		FileLoadEnd();


private:
	_char m_szFilePath[100] = "none";
	ifstream m_CurrentFile;


public:
	static CCSVFileManager* Create();
	virtual void Free() override;

};

END