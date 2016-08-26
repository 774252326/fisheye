#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <windows.h>

static void SetFolder(const std::string &fop)
{
	DWORD dwAttr = GetFileAttributesA(fop.c_str());
	if (dwAttr == 0xFFFFFFFF) //INVALID_FILE_ATTRIBUTES
		CreateDirectoryA(fop.c_str(), NULL);
};


static bool TestFile(const std::string &fp, unsigned long waitms = 0, unsigned long segms = 5)
{
	while (true)
	{
		HANDLE hfile = ::CreateFileA(fp.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hfile != INVALID_HANDLE_VALUE)
		{
			//the specified file exists and available to access
			CloseHandle(hfile);
			return true;
		}

		Sleep(segms);
		if (waitms > 0)
		{
			if (waitms <= segms)
				return false;
			waitms -= segms;
		}		
	}
};

#include <strsafe.h>

static void GetFolderContentList(std::vector<std::string> &fileList, std::vector<std::string> &folderList, std::string folder, bool removeParentSelf = true)
{

	WIN32_FIND_DATAA ffd;
	char szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// If the directory is not specified as a command-line argument,
	// print usage.


	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	StringCchLengthA(folder.c_str(), MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		return;
	}

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopyA(szDir, MAX_PATH, folder.c_str());
	StringCchCatA(szDir, MAX_PATH, ("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFileA(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

			if (removeParentSelf)
			{
				std::string tmpp(ffd.cFileName);

				if (tmpp.compare(".") == 0)
					continue;

				if (tmpp.compare("..") == 0)
					continue;
			}
			folderList.push_back(ffd.cFileName);
		}
		else
		{
			fileList.push_back(ffd.cFileName);
		}
	} while (FindNextFileA(hFind, &ffd) != 0);


	FindClose(hFind);

};

static void DeleteFolderR(std::string fop)
{
	DWORD dwAttr = GetFileAttributesA(fop.c_str());
	if (dwAttr != 0xFFFFFFFF)
	{
		std::vector<std::string> fnl, fol;
		GetFolderContentList(fnl, fol, fop);

		for (size_t j = 0; j < fnl.size(); j++)
		{
			std::string fp = fop + "\\" + fnl[j];
			::DeleteFileA(fp.c_str());
		}
		for (size_t j = 0; j < fol.size(); j++)
		{
			DeleteFolderR(fop + "\\" + fol[j]);
		}
		::RemoveDirectoryA(fop.c_str());
	}
};


static HANDLE OutPipe(std::string pipeName = "pipi", DWORD bufSize = 1024)
{
	std::string pn = "\\\\.\\pipe\\";
	pn = pn + pipeName;

	HANDLE hPipe = CreateNamedPipeA(pn.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		0, 1, bufSize, bufSize, 0, NULL);
	if (INVALID_HANDLE_VALUE == hPipe)
	{
		//std::cout << "CreateNamedPipe fail" << std::endl;
		hPipe = NULL;
		return hPipe;
	}

	HANDLE hEvent;
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!hEvent)
	{
		//std::cout << "CreateEvent fail" << std::endl;
		CloseHandle(hPipe);
		hPipe = NULL;
		return hPipe;
	}
	OVERLAPPED ovlap;
	ZeroMemory(&ovlap, sizeof(OVERLAPPED));
	ovlap.hEvent = hEvent;
	if (!ConnectNamedPipe(hPipe, &ovlap))
	{
		if (ERROR_IO_PENDING != GetLastError())
		{
			//std::cout << "ConnectNamedPipe fail" << std::endl;
			CloseHandle(hPipe);
			CloseHandle(hEvent);
			hPipe = NULL;
			return hPipe;
		}
	}

	if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE))
	{
		//std::cout << "WaitForSingleObject fail" << std::endl;
		CloseHandle(hPipe);
		CloseHandle(hEvent);
		hPipe = NULL;
		return hPipe;
	}
	CloseHandle(hEvent);

	return hPipe;
};

static HANDLE InPipe(std::string pipeName = "pipi")
{
	std::string pn = "\\\\.\\pipe\\";
	pn = pn + pipeName;

	HANDLE hPipe;
	if (!WaitNamedPipeA(pn.c_str(), NMPWAIT_WAIT_FOREVER))
	{
		//std::cout << "WaitNamedPipe fail" << std::endl;
		return NULL;
	}

	hPipe = CreateFileA(pn.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hPipe)
	{
		//std::cout << "CreateFile fail" << std::endl;
		hPipe = NULL;
		return NULL;
	}

	return hPipe;
};

/*
//?除文件或者文件?
bool DeleteFile(char * lpszPath) 
{ 
SHFILEOPSTRUCT FileOp={0}; 
FileOp.fFlags = FOF_ALLOWUNDO |   //允?放回回收站
FOF_NOCONFIRMATION; //不出?确???框
FileOp.pFrom = lpszPath; 
FileOp.pTo = NULL;      //一定要是NULL
FileOp.wFunc = FO_DELETE;    //?除操作
return SHFileOperation(&FileOp) == 0; 
}

//复制文件或文件?
bool CopyFile(char *pTo,char *pFrom)
{
SHFILEOPSTRUCT FileOp={0}; 
FileOp.fFlags = FOF_NOCONFIRMATION|   //不出?确???框
FOF_NOCONFIRMMKDIR ; //需要?直接?建一?文件?,不需用?确定
FileOp.pFrom = pFrom; 
FileOp.pTo = pTo; 
FileOp.wFunc = FO_COPY; 
return SHFileOperation(&FileOp) == 0; 
}

//移?文件或文件?
bool MoveFile(char *pTo,char *pFrom)
{
SHFILEOPSTRUCT FileOp={0}; 
FileOp.fFlags = FOF_NOCONFIRMATION|   //不出?确???框 
FOF_NOCONFIRMMKDIR ; //需要?直接?建一?文件?,不需用?确定
FileOp.pFrom = pFrom; 
FileOp.pTo = pTo; 
FileOp.wFunc = FO_MOVE; 
return SHFileOperation(&FileOp) == 0;   
}

*/


//?命名文件或文件?
//bool ReNameFile(const char *pTo, const char *pFrom)
//{
//	SHFILEOPSTRUCTA FileOp={0}; 
//	FileOp.fFlags = FOF_NOCONFIRMATION|FOF_SIMPLEPROGRESS ;   //不出?确???框 
//	FileOp.pFrom = pFrom; 
//	FileOp.pTo = pTo; 
//	FileOp.wFunc = FO_RENAME; 
//	return SHFileOperationA(&FileOp) == 0;   
//};
