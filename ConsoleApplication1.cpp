// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



#include <iostream> 
#include <windows.h> 
#include <tlhelp32.h> 
#include <tchar.h> 
#include <stdio.h> 
#include <string.h> 
#include <vector> 



using namespace std;



VOID PrintModuleList(HANDLE CONST hStdOut, DWORD CONST dwProcessId) {
	MODULEENTRY32 meModuleEntry;
	TCHAR szBuff[1024];
	DWORD dwTemp;
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE, dwProcessId);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return;
	}

	meModuleEntry.dwSize = sizeof(MODULEENTRY32);
	Module32First(hSnapshot, &meModuleEntry);
	do {
		wsprintf(szBuff, L"  ba: %08X, bs: %08X, %s\r\n",
			meModuleEntry.modBaseAddr, meModuleEntry.modBaseSize,
			meModuleEntry.szModule);
		WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);
	} while (Module32Next(hSnapshot, &meModuleEntry));

	CloseHandle(hSnapshot);
}

VOID PrintProcessList(HANDLE CONST hStdOut) {
	PROCESSENTRY32 peProcessEntry;
	TCHAR szBuff[1024];
	DWORD dwTemp;
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return;
	}

	peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &peProcessEntry);
	do {
		wsprintf(szBuff, L"=== %08X %s ===\r\n",
			peProcessEntry.th32ProcessID, peProcessEntry.szExeFile);
		WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);
		//PrintModuleList(hStdOut, peProcessEntry.th32ProcessID);
	} while (Process32Next(hSnapshot, &peProcessEntry));

	CloseHandle(hSnapshot);
}

struct process
{
	DWORD PID; // поле данных
	char name[] = "";
	struct list* ptr; // указатель на следующий элемент
};




int main(void)
{

	HANDLE CONST hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	PrintProcessList(hStdOut);

	DWORD PID = 0x00051D28;
	DWORD dwTemp;
	TCHAR szBuff[1024];

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (TerminateProcess(hProcess, 0))
	{
		wsprintf(szBuff, L"=== %08X ---- Process was kiled ------ ===\r\n", PID);
		WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);
	}
	else
	{
		wsprintf(szBuff, L"=== %08X -----Can't kill process ----- ===\r\n", PID);
		WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);
	}

	ExitProcess(0);

	//GetProcessList();
	return 0;
}



/*int main()
{
    std::cout << "Hello World!\n";
}
*/
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
