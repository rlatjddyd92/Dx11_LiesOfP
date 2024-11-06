#include"CSVFile_Manager.h"

CCSVFile_Manager::CCSVFile_Manager()
{


}

HRESULT CCSVFile_Manager::Initialize()
{
	return S_OK;
}

HRESULT CCSVFile_Manager::FileOpenByRow(const _char* FilePath, _bool bIsRead)
{
	FileClose();

	if (bIsRead)
	{
		m_CurrentFile_Read.open(FilePath);
		string line;

		if (!m_CurrentFile_Read.is_open())
			return E_FAIL;
	}
	else
	{
		m_CurrentFile_Write.open(FilePath);
		string line;

		if (!m_CurrentFile_Write.is_open())
			return E_FAIL;
	}
	
	return S_OK;
}

_bool CCSVFile_Manager::LoadDataByRow(vector<_wstring>* vecDataBuffer)
{
	if (!m_CurrentFile_Read.is_open())
		return false;

	string line;
	if (getline(m_CurrentFile_Read, line))
	{
		stringstream ss(line);
		string cell;

		while (std::getline(ss, cell, ',')) 
		{
			int wstrSize = MultiByteToWideChar(CP_UTF8, 0, cell.c_str(), -1, nullptr, 0);
			_wstring strData(wstrSize, 0);
			MultiByteToWideChar(CP_UTF8, 0, cell.c_str(), -1, &strData[0], wstrSize);
			vecDataBuffer->push_back(strData);
		}

		return true;
	}
	else
		return false;
}

_bool CCSVFile_Manager::SaveDataByRow(vector<_wstring>& vecDataBuffer)
{
	if (!m_CurrentFile_Write.is_open())
		return false;

	string line;
	
	for (auto& iter : vecDataBuffer)
	{
		if (!line.empty()) line += ',';
		int strSize = WideCharToMultiByte(CP_UTF8, 0, iter.c_str(), -1, nullptr, 0, nullptr, nullptr);
		string str(strSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, iter.c_str(), -1, &str[0], strSize, nullptr, nullptr);
		line += str;
	}
	
	m_CurrentFile_Write << line << '\n';

	return true;
}

void CCSVFile_Manager::FileClose()
{
	m_CurrentFile_Read.close();
	m_CurrentFile_Write.close();
}

HRESULT CCSVFile_Manager::LoadDataByFile(const _char* FilePath, vector<vector<_wstring>>* vecDataBuffer)
{
	if (FAILED(FileOpenByRow(FilePath, true)))
		return E_FAIL;

	while (1)
	{
		vector<_wstring> vecRow;

		if (LoadDataByRow(&vecRow))
			vecDataBuffer->push_back(vecRow);
		else
			break;
	}

	FileClose();
	
	return S_OK;
}

HRESULT CCSVFile_Manager::SaveDataByFile(const _char* FilePath, vector<vector<_wstring>>& vecDataBuffer)
{
	if (FAILED(FileOpenByRow(FilePath, false)))
		return E_FAIL;

	for (auto& iter : vecDataBuffer)
	{
		if (!SaveDataByRow(iter))
			break;
	}

	FileClose();

	return S_OK;
}

CCSVFile_Manager* CCSVFile_Manager::Create()
{
	CCSVFile_Manager* pInstance = new CCSVFile_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCSVFile_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCSVFile_Manager::Free()
{
	__super::Free();
	FileClose();
}
