#include <windows.h>
#include <cstdio>
#include <vector>
#include "util/WinRegs.h"
#include "util/CommonWinFuns.h"
#include <Shlwapi.h>
#include <fstream>
#include "util/StringEx.h"
#include "util/File.h"
#include <algorithm>

#pragma comment(lib, "shlwapi.lib")

static std::wstring WideStringUpperCase(const std::wstring& str)
{
	std::wstring ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(), toupper);
	return ret;
}

int main(int argc, char** argv)
{
	std::vector<std::wstring> skip =
	{
		L"{1f4de370-d627-11d1-ba4f-00a0c91eedba}",
		L"{450D8FBA-AD25-11D0-98A8-0800361B1103}",
		L"{645FF040-5081-101B-9F08-00AA002F954E}",
		L"{e17d4fc0-5564-11d1-83f2-00a0c90dc849}",
		L"{04731B67-D933-450a-90E6-4ACD2E9408FE}",
		L"{21EC2020-3AEA-1069-A2DD-08002B30309D}",
		L"{26EE0668-A00A-44D7-9371-BEB064C98683}",
		L"{4336a54d-038b-4685-ab02-99bb52d3fb8b}",
		L"{450D8FBA-AD25-11D0-98A8-0800361B1103}",
		L"{59031a47-3f72-44a7-89c5-5595fe6b30ee}",
		L"{645FF040-5081-101B-9F08-00AA002F954E}",
		L"{89D83576-6BD1-4c86-9454-BEB04E94C819}",
		L"{9343812e-1c37-4a49-a12e-4b2d810d956b}",
		L"{BD7A2E7B-21CB-41b2-A086-B309680C6B7E}",
		L"{ED228FDF-9EA8-4870-83b1-96b02CFE0D52}",
		L"{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}",
		L"{031E4825-7B94-4dc3-B131-E946B44C8DD5}",
		L"{04731B67-D933-450a-90E6-4ACD2E9408FE}",
		L"{11016101-E366-4D22-BC06-4ADA335C892B}",
		L"{26EE0668-A00A-44D7-9371-BEB064C98683}",
		L"{4336a54d-038b-4685-ab02-99bb52d3fb8b}",
		L"{450D8FBA-AD25-11D0-98A8-0800361B1103}",
		L"{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}",
		L"{59031a47-3f72-44a7-89c5-5595fe6b30ee}",
		L"{645FF040-5081-101B-9F08-00AA002F954E}",
		L"{89D83576-6BD1-4c86-9454-BEB04E94C819}",
		L"{8FD8B88D-30E1-4F25-AC2B-553D3D65F0EA}",
		L"{9343812e-1c37-4a49-a12e-4b2d810d956b}",
		L"{98D99750-0B8A-4c59-9151-589053683D73}",
		L"{B4FB3F98-C1EA-428d-A78A-D1F5659CBA93}",
		L"{BD7A2E7B-21CB-41b2-A086-B309680C6B7E}",
		L"{daf95313-e44d-46af-be1b-cbacea2c3065}",
		L"{e345f35f-9397-435c-8f95-4e922c26259e}",
		L"{ED228FDF-9EA8-4870-83b1-96b02CFE0D52}",
		L"{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}",
		L"{F3F5824C-AD58-4728-AF59-A1EBE3392799}"
	};
	for (int i = 0; i != skip.size(); i++)
	{
		skip[i] = WideStringUpperCase(skip[i]);
	}
	DisableWow64FsRedir();
	//
	// CLSID,find InprocServer32 default value which not exists.
	//
	std::wstring CLSIDRegPath = L"SOFTWARE\\Classes\\CLSID";
	CWinRegKey reg(HKEY_LOCAL_MACHINE, CLSIDRegPath.c_str(), FALSE);
	std::vector<std::wstring> v = reg.EnumSubKeys();
	std::vector<std::wstring> InfoMissed;
	std::vector<std::wstring> CLSIDMissed;
	for (auto i : v)
	{
		std::wstring k = WideStringUpperCase(i);
		if (std::find(skip.begin(),skip.end(),k) != skip.end())
		{
			continue;
		}
		wprintf(L"%s", i.c_str());
		CWinRegKey RegShellFolder(HKEY_LOCAL_MACHINE, (CLSIDRegPath + L"\\" + i + L"\\ShellFolder").c_str(), FALSE);
		HKEY temp;
		if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, (CLSIDRegPath + L"\\" + i + L"\\ShellFolder").c_str(),&temp))
		{
			RegCloseKey(temp);
			wprintf(L" [ShellFolder]\n");
			continue;
		}
		CWinRegKey RegCLSIDSubKey(HKEY_LOCAL_MACHINE, (CLSIDRegPath + L"\\" + i + L"\\InprocServer32").c_str(), FALSE);
		wchar_t DefaultValue[256] = { 0 };
		if (!RegCLSIDSubKey.ReadString(L"", DefaultValue, sizeof(DefaultValue)))
			wprintf(L" N/A\n");
		else
		{
			wchar_t ExpandValue[256] = { 0 };
			ExpandEnvironmentStrings(DefaultValue, ExpandValue, _countof(ExpandValue));
			if (PathFileExists(ExpandValue))
			{
				wprintf(L" %s\n", ExpandValue);
			}
			else
			{
				wchar_t WindowsDir[256] = { 0 };
				GetWindowsDirectoryW(WindowsDir, _countof(WindowsDir));
				PathAddBackslashW(WindowsDir);
				if (PathFileExists((std::wstring(WindowsDir) + ExpandValue).c_str()))
				{
					wprintf(L" %s\n", ExpandValue);
				}
				else if(PathFileExists((std::wstring(WindowsDir) + L"System32\\" + ExpandValue).c_str()))
				{
					wprintf(L" %s\n", ExpandValue);
				}
				else
				{
					wprintf(L" %s [MISSED]\n", ExpandValue);
					InfoMissed.push_back(i + L"|" + ExpandValue);
					CLSIDMissed.push_back(i);
				}
			}
		}
	}
	if (!InfoMissed.empty())
	{
		std::ofstream ofs;
		ofs.open("Missed.txt");
		for (auto i : InfoMissed)
		{
			if (ofs.is_open())
			{
				ofs << String::fromStdWString(i).c_str() << std::endl;
			}
		}
		ofs.close();
	}
	if (!CLSIDMissed.empty())
	{
		std::ofstream ofs;
		ofs.open("CLSIDMissed.txt");
		for (auto i : CLSIDMissed)
		{
			if (ofs.is_open())
			{
				ofs << String::fromStdWString(i).c_str() << std::endl;
			}
		}
		ofs.close();
	}
	//
	// Interface, find clsid not exists.
	//
	std::wstring InterfaceRegPath = L"SOFTWARE\\Classes\\Interface";
	CWinRegKey regInterface(HKEY_LOCAL_MACHINE, InterfaceRegPath.c_str(), FALSE);
	std::vector<std::wstring> v2 = regInterface.EnumSubKeys();
	std::vector<std::wstring> InterfaceMissed;
	for (int i = 0; i != CLSIDMissed.size(); i++)
	{
		std::wstring OriginalValue = CLSIDMissed[i];
		std::wstring UpperCaseValue = OriginalValue;
		std::transform(OriginalValue.begin(), OriginalValue.end(), UpperCaseValue.begin(), toupper);
		CLSIDMissed[i] = UpperCaseValue;
	}
	for (auto i : v2)
	{
		CWinRegKey regInterfaceSubKey(HKEY_LOCAL_MACHINE, (InterfaceRegPath + L"\\" + i + L"\\ProxyStubClsid32").c_str(), FALSE);
		wchar_t DefaultValue[256] = { 0 };
		if (!regInterfaceSubKey.ReadString(L"", DefaultValue, sizeof(DefaultValue)))
			wprintf(L" N/A\n");
		else
		{
			std::wstring t = DefaultValue;
			std::transform(t.begin(), t.end(), t.begin(), toupper);
			if (find(CLSIDMissed.begin(),CLSIDMissed.end(),t) != CLSIDMissed.end())
			{
				InterfaceMissed.push_back(DefaultValue);
			}
		}
	}
	if (!InterfaceMissed.empty())
	{
		std::ofstream ofs;
		ofs.open("InterfaceMissed.txt");
		if (ofs.is_open())
		{
			for (auto i : InterfaceMissed)
			{
				ofs << String::fromStdWString(i).c_str() << std::endl;
			}
			ofs.close();
		}
	}
	RestoreWow64FsRedir();
	system("pause");
	return 0;
}