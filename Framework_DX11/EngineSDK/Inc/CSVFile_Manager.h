#pragma once
#include "Base.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

BEGIN(Engine)

class CCSVFile_Manager final : public CBase
{
public:
	

private:
	CCSVFile_Manager();
	virtual ~CCSVFile_Manager() = default;


public:
	HRESULT		Initialize();
	HRESULT		FileOpenByRow(const _char* FilePath, _bool bIsRead); // bIsRead가 true면 읽기 false면 쓰기
	_bool		LoadDataByRow(vector<_wstring>* vecDataBuffer);
	_bool		SaveDataByRow(vector<_wstring>& vecDataBuffer);
	void		FileClose();

	HRESULT		LoadDataByFile(const _char* FilePath, vector<vector<_wstring>>* vecDataBuffer);
	HRESULT		SaveDataByFile(const _char* FilePath, vector<vector<_wstring>>& vecDataBuffer);

	_bool		IsFileRead() { return m_CurrentFile_Read.is_open(); }
	_bool		IsFileWrite() { return m_CurrentFile_Write.is_open(); }

private:
	//_char m_szFilePath[100] = "none";
	ifstream m_CurrentFile_Read;
	ofstream m_CurrentFile_Write;


public:
	static CCSVFile_Manager* Create();
	virtual void Free() override;

};

END