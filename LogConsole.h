#pragma once

namespace LogConsole
{
	bool Open();
	void Close();

	void setFontColor(int color);
	void resetFontColor();

	void Print(const wchar_t* format, ...);
}

#define LogConsoleOpen LogConsole::Open
#define LogConsoleClose LogConsole::Close

static void __cdecl _assertConsol(wchar_t const* message, wchar_t const* file, unsigned int line)
{
	MessageBeep(MB_ICONWARNING);
	//Beep(900, 200);

	wchar_t _message[256] = {};
	swprintf_s(_message, L"Expression : %s\nFile : %s\nLine : %d\n", message, file, line);

	// 	wchar_t _programName[MAX_PATH + 1] = {};
	// 	GetModuleFileNameW(NULL, _programName, MAX_PATH);

	LogConsole::setFontColor(FOREGROUND_RED);
	LogConsole::Print(_message);
	LogConsole::resetFontColor();

	WriteFileLog("Log\\AssertConsol", NULL, "a+", _message);
}

#ifdef NDEBUG

#define ASSERTCONSOL(expression) ((void)0)

#else

#define ASSERTCONSOL(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (_assertConsol(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )

#endif

inline void PrintToConsol(const wchar_t* format, ...)
{
#ifdef _DEBUG
	LogConsole::Print(format);
#endif
}