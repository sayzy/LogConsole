#include "stdafx.h"
#include "LogConsole.h"

#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <strsafe.h>

using namespace std;
using namespace LogConsole;

#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)

namespace
{
	BOOL WINAPI ConsoleHandler(DWORD dwCtrlType)
	{
		switch (dwCtrlType)
		{
		case CTRL_C_EVENT:
			return TRUE;    //this just disables Ctrl-C
		case CTRL_CLOSE_EVENT:
			::FreeConsole();
			return TRUE;
		default:
			return FALSE;
		}
	}

	void AdjustConsoleBuffer(const int16_t minLength = 500)
	{
		HWND console_hwnd = GetConsoleWindow();

		HMENU console_menu = ::GetSystemMenu(console_hwnd, FALSE);
		if (console_menu != NULL) DeleteMenu(console_menu, SC_CLOSE, MF_BYCOMMAND);

		CONSOLE_SCREEN_BUFFER_INFO conInfo;

		// Set the screen buffer to be big enough to scroll some text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
		if (conInfo.dwSize.Y < minLength)
			conInfo.dwSize.Y = minLength;

		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);

		SetConsoleTitle("Debug Mode Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_WHITE);

		setlocale(LC_ALL, "chs");
	}
}

bool LogConsole::Open()
{
#ifdef DSERVER_OBJECT
#ifdef _DEBUG
	if (!AllocConsole())
	{
		MessageBox(NULL, "The console window was not created", NULL, MB_ICONEXCLAMATION);
		return false;
	}

	HANDLE stdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (INVALID_HANDLE_VALUE == stdHandle)
	{
		::FreeConsole();
		return false;
	}

	AdjustConsoleBuffer();

	//Use my console Handlers
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
#endif
#endif

	return true;
}

void LogConsole::Close()
{
#ifdef DSERVER_OBJECT
#ifdef _DEBUG
	::FreeConsole();
#endif
#endif
}

void LogConsole::setFontColor(int color)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (info.wAttributes & 0xf0) | (color & 0xf));
}

void LogConsole::resetFontColor()
{
	setFontColor(FOREGROUND_WHITE);
}

void LogConsole::Print(const wchar_t* format, ...)
{
	static const int maxTraceLength = 1024;

	wchar_t buffer[maxTraceLength];
	{
		va_list args;
		va_start(args, format);
		::StringCchVPrintfW(buffer, maxTraceLength, format, args);
		va_end(args);
	}

	DWORD length = static_cast<DWORD>(wcslen(buffer));
	DWORD written = 0;
	::WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),
		buffer,
		length,
		&written, NULL);
}
