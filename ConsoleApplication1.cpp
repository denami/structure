#include <iostream>
#include <windows.h> 
#include <tlhelp32.h> 
#include <tchar.h> 
#include <stdio.h> 
#include <string.h> 

using namespace std;

#define MAX_LINES = 20;

struct Item {
	DWORD PID; //PID process
	DWORD PPID; //Parent PID process
	//std::wstring name;
	std::string name;
	int isPrinted = 0;

	Item* next;
};

void displayMeny()
{



}


struct Item * PrintProcessList(HANDLE CONST hStdOut, Item *process) {
	Item* p;
	PROCESSENTRY32 peProcessEntry; //Структура содержащая список процессов
	TCHAR szBuff[1024];
	DWORD dwTemp;
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //Cоздает снапшот запущенных процессов
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return process;
	}
	int i = 0;
	peProcessEntry.dwSize = sizeof(PROCESSENTRY32);  //Set size of PROCESSENTRY32 structure
	Process32First(hSnapshot, &peProcessEntry); //Receieve handle to the snapshot. Hendle to first element
	do {
		wsprintf(szBuff, L"=== %08X %s ===\r\n", peProcessEntry.th32ProcessID);
			//, peProcessEntry.szExeFile);
		//WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);

		//wsprintf("=== %d %d ===\r\n", peProcessEntry.th32ProcessID, peProcessEntry.szExeFile);

		//_tprintf(TEXT("\n Process ID = 0x%08X"), peProcessEntry.th32ProcessID);

		wstring ws(peProcessEntry.szExeFile);
		//std::string s(peProcessEntry.szExeFile);  //Variable s to store process exe name
		string str(ws.begin(), ws.end());
		if (i == 0)
		{
			process->PID = peProcessEntry.th32ProcessID;
			process->PPID = peProcessEntry.th32ParentProcessID;
			process->name = str;
			process->next = NULL;
			i = 1;
		}
		else 
		{
			p = new Item;
			p->PID = peProcessEntry.th32ProcessID;
			p->PPID = peProcessEntry.th32ParentProcessID;
			p->name = str;
			p->next = process;
			process = p;
		}
	}
	while (Process32Next(hSnapshot, &peProcessEntry)); //Move handle to next element

	CloseHandle(hSnapshot); //Closes an open object handle
	return (process);

}

BOOL processInList(Item* lst, DWORD pid)
{
	struct Item* p;  //handle for fist element of list
	p = lst;

	do
	{
		if (pid == p->PID)
			return true;
		p = p->next;
	} 
	while (p != NULL);
	return false;
}

BOOL listprint(Item* lst, DWORD curentpid, int count, int line)
{
	struct Item* p;  //handle for fist element of list
	p = lst;
	//printf("curent pid %d cpace %d \n", curentpid, count);
	do 
	{
		if (p->isPrinted == 0 &&  p->PPID == curentpid)
		{
			for (int i = 0; i <= count; i++)
			{
				printf(" ");
			}
			//printf("%d %d %s\n", p->PID, p->PPID, p->name);
			printf("%d  %s\n", p->PID,  p->name);
			line++;
			p->isPrinted = 1;
			listprint(lst, p->PID, count + 1, line);
		}
		if (p->isPrinted == 0 && !processInList(lst, p->PPID))
		{
			//printf("%d %d %s\n", p->PID, p->PPID, p->name);
			printf("%d %s\n", p->PID, p->name);
			line++;
			p->isPrinted = 1;
			listprint(lst, p->PID, 0, line);
			
		}
		p = p->next; //Move handle to next element
	} while (p != NULL);
	return 0;
}

int main(void)
{
	//Item* first = 0; //Указатель на начало списка
	Item* first = (struct Item*)malloc(sizeof(struct Item));
	std::string name;

	HANDLE CONST hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	first = PrintProcessList(hStdOut, first);


	//_tprintf(TEXT("\n WARNING: %s failed with error %d (%s)"), L"aa", 11, L"02");

	listprint(first, 0, 0, 0);



	ExitProcess(0);

	//GetProcessList();
	return 0;

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
