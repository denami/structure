#include <iostream>
#include <windows.h> 
#include <tlhelp32.h> 
#include <tchar.h> 
#include <stdio.h>
#include <string.h> 


using namespace std;

#define MAX_LINES 20

struct Item {
	DWORD PID;          //PID process
	DWORD PPID;         //Parent PID process
	int isPrinted = 0;  //Flag for marck process ass displaed
	WCHAR name[MAX_PATH];//Process exe file
	Item* next;         //handle to nex Item
};


int killProcess(DWORD p)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, p);
	if (TerminateProcess(hProcess, 0))
	{
		printf("Prosecc  %d killed sucessful.\n", p);
		return 0;
	}
	else
	{
		printf("Prosecc %d didn't killed.\n", p);
		return 1;
	}
	return 1;
}

void displayKillMeny()
{
	printf("====================================\n");
	printf("Type PID number for kill: \n");
	DWORD p;

	cin >> p;

	killProcess(p);

	printf("\n");
}

void displayMeny()
{
	printf("=====================================\n");
	printf("| For kill process:       \t  1 |\n");
	printf("| For print next page type:\t  2 |\n");
	printf("| For quit type:          \t  3 |\n");
	printf("=====================================\n");
	char c;
	int choise;

	cin >> choise;
	switch (choise)
	{
	case 1:
	{
		displayKillMeny();
		break;
	}
	case 2:
	{
		return;
		break;
	}
	case 3:
	{
		ExitProcess(0);
		break;
	}
	default:
		return;
		break;
	}
}

struct Item * PrintProcessList(Item *process) {
	Item* p;
	PROCESSENTRY32 peProcessEntry; //Structure stored prosess list
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //Take snapshot for runned processes
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return process;
	}
	int i = 0;
	peProcessEntry.dwSize = sizeof(PROCESSENTRY32);  //Set size of PROCESSENTRY32 structure
	Process32First(hSnapshot, &peProcessEntry);      //Receieve handle to the snapshot. Hendle to first element
	do {
		DWORD PID = peProcessEntry.th32ProcessID;
		DWORD PPID = peProcessEntry.th32ParentProcessID;
		if (i == 0)  //Check for first iteration
		{
			process->PID = PID;
			process->PPID = PPID;
			copy(begin(peProcessEntry.szExeFile), end(peProcessEntry.szExeFile), begin(process->name));
			process->next = NULL;
			i = 1;
		}
		else 
		{
			p = new Item; //Create new Item
			p->PID = PID;
			p->PPID = PPID;
			copy(begin(peProcessEntry.szExeFile), end(peProcessEntry.szExeFile), begin(p->name));
			p->next = process; //Set handle for current Item
			process = p;       //Set hendle to new Item
		}
	}
	while (Process32Next(hSnapshot, &peProcessEntry)); //Move handle to next element

	CloseHandle(hSnapshot); //Closes an open object handle
	return (process);       //Return handle to first Item in list

}

//Method for check by PID for is this process in list
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
	p = lst;         //Counter for printed lines

	TCHAR szBuff[1024];
	DWORD dwTemp;
	HANDLE CONST hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	do 
	{
		if (p->isPrinted == 0 &&  p->PPID == curentpid)
		{
			for (int i = 0; i <= count; i++)
			{
				printf(" ");
			}
			wsprintf(szBuff, L"%d  %s\n", p->PID, p->name);
			WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);
			line++;
			p->isPrinted = 1;
			listprint(lst, p->PID, count + 1, 0); //Print  #MAX_LINES for each
		}
		if (p->isPrinted == 0 && !processInList(lst, p->PPID))
		{
			wsprintf(szBuff, L"%d  %s\n", p->PID, p->name);
			WriteConsole(hStdOut, szBuff, lstrlen(szBuff), &dwTemp, NULL);
			line++;
			p->isPrinted = 1;
			listprint(lst, p->PID, 1, 0);   //1 setted because next element will be child for current
		}
		if (line == MAX_LINES)
		{
			displayMeny();
			line = 0;
		}

		p = p->next; //Move handle to next element
	} while (p != NULL);
	return 0;
}

int main(void)
{
	Item* first = new Item; //Create first Item (get memory for structure);
	first = PrintProcessList(first);

	listprint(first, 0, 0, 0);
	displayMeny();

	return 0;
}
