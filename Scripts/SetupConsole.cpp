#include "SetupConsole.h"

#include <filesystem>

/// <summary>
/// Initialise the Console
/// </summary>
void SetupConsole::InitConsole()
{
	HWND window = GetConsoleWindow();

	ShowWindow(window, SW_HIDE);

	SetupPath();

	SetupName();

	CheckInstance();

	DefineConsole(window);

	ShowWindow(window, SW_SHOW);
}

/// <summary>
/// Size the Console
/// </summary>
/// <param name="outHandle"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void SetupConsole::ResizeConsole(HANDLE outHandle, int width, int height) const
{
	COORD buffer
	{
		static_cast<SHORT>(width), static_cast<SHORT>(height),
	};

	SetConsoleScreenBufferSize(outHandle, buffer);

	SMALL_RECT rect
	{
		0,
		0,
		static_cast<SHORT>(width - 1),
		static_cast<SHORT>(height - 1),
	};

	SetConsoleWindowInfo(outHandle, TRUE, &rect);
}

/// <summary>
/// Set the Font of the Console
/// </summary>
/// <param name="outHandle"></param>
/// <param name="size"></param>
void SetupConsole::SetFontSize(HANDLE outHandle, int size)
{
	CONSOLE_FONT_INFOEX info
	{
		sizeof info,
	};

	if (GetCurrentConsoleFontEx(outHandle, FALSE, &info))
	{
		info.dwFontSize.Y = static_cast<SHORT>(size);
		wcscpy_s(info.FaceName, fontName.data());
		SetCurrentConsoleFontEx(outHandle, FALSE, &info);
		this->textFontSize = size;
	}
}

/// <summary>
/// Set the Text Color of the Console
/// </summary>
/// <param name="indexColor"></param>
void SetupConsole::SetTextColor(int indexColor) const
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(consoleHandle, indexColor);
}

/// <summary>
/// Setup the Path of the Game
/// </summary>
void SetupConsole::SetupPath()
{
	std::wstring buffer(MAX_PATH, L'\0');
	GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

	std::filesystem::path exePath(buffer);
	this->pathGameFolder = exePath.parent_path().string() + "\\";
}

/// <summary>
/// Setup the Name of the Console
/// </summary>
void SetupConsole::SetupName() const
{
	SetConsoleTitleW(appName.data());
}

/// <summary>
/// Check if the Instance is Already Running
/// </summary>
void SetupConsole::CheckInstance() const
{
	HANDLE mutex = CreateMutexW(nullptr, TRUE, appName.data());

	if (!mutex || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (mutex)
		{
			CloseHandle(mutex);
		}

		exit(EXIT_SUCCESS);
	}
}

/// <summary>
/// Set the Position of the Console on the Desktop
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void SetupConsole::CenterWindow(HWND window, int screenWidth, int screenHeight) const
{
	RECT consoleRect{};
	GetWindowRect(window, &consoleRect);

	int x = (screenWidth - (consoleRect.right - consoleRect.left)) / 2;
	int y = (screenHeight - (consoleRect.bottom - consoleRect.top)) / 2;

	SetWindowPos(window, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

/// <summary>
/// Set the Console
/// </summary>
void SetupConsole::DefineConsole(HWND window)
{
	HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD inMode{};
	GetConsoleMode(in, &inMode);
	SetConsoleMode(in, inMode & ~ENABLE_PROCESSED_INPUT);

	RECT desk{};
	GetWindowRect(GetDesktopWindow(), &desk);
	int screenW = desk.right - desk.left;
	int screenH = desk.bottom - desk.top;

	ConsoleSettings consoleSettings{};
	ResizeConsole(out, consoleSettings.width, consoleSettings.height);

	for (const auto& entry : fontMap)
	{
		if (screenW >= entry.minWidth)
		{
			SetFontSize(out, entry.fontSize);
			break;
		}
	}

	Sleep(1);

	CenterWindow(window, screenW, screenH);

	CONSOLE_CURSOR_INFO ci
	{
		1,
		FALSE,
	};

	SetConsoleCursorInfo(out, &ci);

	LONG style = GetWindowLongW(window, GWL_STYLE);
	style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
	SetWindowLongW(window, GWL_STYLE, style);

	DWORD flags{};
	GetConsoleMode(in, &flags);
	SetConsoleMode(in, flags & ENABLE_EXTENDED_FLAGS);

	HMENU hMenu = GetSystemMenu(window, FALSE);
	int count = GetMenuItemCount(hMenu);
	for (int i = count - 1; i >= 0; --i)
	{
		UINT cmd = GetMenuItemID(hMenu, i);
		if (cmd != SC_CLOSE && cmd != SC_MINIMIZE)
		{
			DeleteMenu(hMenu, i, MF_BYPOSITION);
		}
	}

	DrawMenuBar(window);
}