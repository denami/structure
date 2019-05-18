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


struct Item {
//	int info;
	DWORD PID; //PID process
	DWORD PPID; //Parent PID process
	std::string name; //Name of process
	Item* next;
};



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

struct Item * PrintProcessList(HANDLE CONST hStdOut, Item *first) {
	Item* p;
	PROCESSENTRY32 peProcessEntry;
	TCHAR szBuff[1024];
	DWORD dwTemp;
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return first;
	}
	int i = 0;
	peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &peProcessEntry);
	do {
		wsprintf(szBuff, L"=== %08X %s ===\r\n",
			peProcessEntry.th32ProcessID, peProcessEntry.szExeFile);
		//WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);

		//wstring ws(peProcessEntry.szExeFile);
		//string processName(ws.begin(), ws.end());


		if (i == 0)
		{
			first->PID = peProcessEntry.th32ProcessID;
			first->PPID = peProcessEntry.th32ParentProcessID;
		
			//TODO Fix
			//first->name = processName;
			first->next = NULL;
			i = 1;

		}
		else 
		{
			p = new Item;
			p->PID = peProcessEntry.th32ProcessID;
			p->PPID = peProcessEntry.th32ParentProcessID;
			p->next = first;
			first = p;
		}
		//PrintModuleList(hStdOut, peProcessEntry.th32ProcessID);
	} while (Process32Next(hSnapshot, &peProcessEntry));

	CloseHandle(hSnapshot);
	return (first);

}

struct procesElement
{
	DWORD PID; //PID process
	DWORD PPID; //Parent PID process
	std::string name; //Name of process
	struct procesElement* tail; // указатель на следующий элемент
	procesElement() {}
	procesElement(DWORD a) { PID = a;}
	procesElement(DWORD a, DWORD b) { PID = a; PPID = b; };
	procesElement(DWORD p, DWORD pp, procesElement* t) { PID = p; PPID = pp, tail = t; };

};


struct procesElement * init(DWORD pid) // а- значение первого узла
{
	struct procesElement* procesElement;
	// выделение памяти под корень списка
	procesElement = (struct procesElement*)malloc(sizeof(struct procesElement));
	procesElement->PID = pid;
	procesElement->tail = NULL; // это последний узел списка
	return(procesElement);
}

void listprint(Item* lst, DWORD curentpid, int count)
{
	struct Item* p;
	p = lst;
	
	do 
	{
		if (p->PPID == curentpid)
		{
			for (int i = 0; i <= count; i++)
			{
				printf(" ");
			}
			printf("%d %d \n", p->PID, p->PPID); // вывод значения элемента p
			listprint(lst, p->PID, count + 1);
		}
		p = p->next; // переход к следующему узлу
	} while (p != NULL);
}

int main(void)
{
	//Item* first = 0; //Указатель на начало списка
	Item* first = (struct Item*)malloc(sizeof(struct Item));
	Item* p;
	int i;
	std::string name;

	HANDLE CONST hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	first = PrintProcessList(hStdOut, first);

	listprint(first, 0, 0);


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
