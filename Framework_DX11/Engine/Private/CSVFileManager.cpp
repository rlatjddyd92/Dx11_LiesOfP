#include"CSVFileManager.h"

CCSVFileManager::CCSVFileManager()
{


}

HRESULT CCSVFileManager::Initialize()
{
	return S_OK;
}

void CCSVFileManager::FileLoadStart(const _char* FilePath, _int iDataStartRow)
{
}

void CCSVFileManager::GetDataByRow(_char* DataBuffer, _int iDataColumn)
{
}

void CCSVFileManager::FileLoadEnd()
{
}

CCSVFileManager* CCSVFileManager::Create()
{
	CCSVFileManager* pInstance = new CCSVFileManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCSVFileManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCSVFileManager::Free()
{
	__super::Free();
}
