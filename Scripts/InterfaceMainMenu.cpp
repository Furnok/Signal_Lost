#include "InterfaceMainMenu.h"

#include <conio.h>
#include <thread>
#include <chrono>
#include <windows.h>
#include <iostream>

using namespace std::chrono;

/// <summary>
/// Display the Game Main Menu
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
void InterfaceMainMenu::DisplayMainMenu(Utils& utils, SetupConsole& setupConsole)
{
	constexpr auto delay = 200ms;

	while (1)
	{
		utils.ClearConsole();

		setupConsole.SetTextColor(2);
		std::this_thread::sleep_for(delay);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		const int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

		utils.DrawMainMenuTitle(gameTitle, consoleWidth);

		setupConsole.SetTextColor(7);
		std::this_thread::sleep_for(delay);

		const int boxWidth = 21;
		const int boxHeight = 11;
		const int boxLeft = (consoleWidth - boxWidth) / 2;
		const int boxTop = (consoleHeight - boxHeight) / 2;

		utils.DrawBox(boxLeft, boxTop, boxWidth, boxHeight);

		setupConsole.SetTextColor(1);
		std::this_thread::sleep_for(delay);

		UINT originalCP = GetConsoleOutputCP();
		SetConsoleOutputCP(CP_UTF8);

		utils.DrawMenuItems(mainMenuItems, boxLeft, boxTop);

		SetConsoleOutputCP(originalCP);

		setupConsole.SetTextColor(7);

		bool newDisplay = false;

		while (!newDisplay)
		{
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			switch (_getch())
			{
			case '1':
			{
				return;
			}
			case '2':
			{
				DisplaySettings(utils, setupConsole);
				newDisplay = true;
				break;
			}
			case '3':
			{
				DisplayCredits(utils, setupConsole);
				newDisplay = true;
				break;
			}
			case '4':
			{
				utils.PosCursor(0, 30);
				exit(EXIT_SUCCESS);
			}
			default:
			{
				break;
			}
			}
		}
	}
}

/// <summary>
/// Update the UI Settings Sound
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
/// <param name="right"></param>
/// <param name="top"></param>
void InterfaceMainMenu::UpdateSettingSoundActivated(Utils& utils, SetupConsole& setupConsole, int right, int top) const
{
	setupConsole.SetTextColor(2);

	utils.PosCursor(right, top);
	std::cout << (setupConsole.GetSoundActivated() ? "ON " : "OFF") << "\n";

	setupConsole.SetTextColor(7);
}

/// <summary>
/// Update the UI Settings FontSize
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
/// <param name="right"></param>
/// <param name="top"></param>
void InterfaceMainMenu::UpdateSettingFontSize(Utils& utils, SetupConsole& setupConsole, int right, int top) const
{
	setupConsole.SetTextColor(2);

	utils.PosCursor(right, top);
	std::cout << setupConsole.GetFontSize() << "px\n";

	setupConsole.SetTextColor(7);
}

/// <summary>
/// Display the Settings
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
void InterfaceMainMenu::DisplaySettings(Utils& utils, SetupConsole& setupConsole)
{
	utils.ClearConsole();

	constexpr auto delay = 200ms;

	std::this_thread::sleep_for(delay);
	setupConsole.SetTextColor(2);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	const int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	utils.DrawMainMenuTitle(settingsTitle, consoleWidth);

	setupConsole.SetTextColor(7);
	std::this_thread::sleep_for(delay);

	const int boxWidth = 26;
	const int boxHeight = 9;
	const int boxLeft = (consoleWidth - boxWidth) / 2;
	const int boxTop = (consoleHeight - boxHeight) / 2;

	utils.DrawBox(boxLeft, boxTop, boxWidth, boxHeight);

	setupConsole.SetTextColor(1);
	std::this_thread::sleep_for(delay);

	UINT originalCP = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);

	utils.DrawMenuItems(settingsItems, boxLeft, boxTop);

	SetConsoleOutputCP(originalCP);

	setupConsole.SetTextColor(7);

	UpdateSettingSoundActivated(utils, setupConsole, boxLeft + boxWidth - 8, boxTop + 2);
	UpdateSettingFontSize(utils, setupConsole, boxLeft + boxWidth - 8, boxTop + 4);

	while (1)
	{
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

		switch (_getch())
		{
		case '1':
		{
			setupConsole.SetSoundActivated(!setupConsole.GetSoundActivated());
			UpdateSettingSoundActivated(utils, setupConsole, boxLeft + boxWidth - 8, boxTop + 2);
			break;
		}
		case '2':
		{
			HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
			int currentSize = setupConsole.GetFontSize();
			auto it = std::find(FONT_SIZES.begin(), FONT_SIZES.end(), currentSize);

			if (it != FONT_SIZES.end())
			{
				++it;

				if (it == FONT_SIZES.end())
				{
					it = FONT_SIZES.begin();
				}

				setupConsole.SetFontSize(out, *it);
			}

			UpdateSettingFontSize(utils, setupConsole, boxLeft + boxWidth - 8, boxTop + 4);
			break;
		}
		case '3':
		{
			return;
		}
		default:
		{
			break;
		}
		}
	}
}

/// <summary>
/// Display the Credits
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
void InterfaceMainMenu::DisplayCredits(Utils& utils, SetupConsole& setupConsole)
{
	utils.ClearConsole();

	constexpr auto delay = 200ms;

	setupConsole.SetTextColor(2);
	std::this_thread::sleep_for(delay);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	const int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	utils.DrawMainMenuTitle(creditsTitle, consoleWidth);

	setupConsole.SetTextColor(7);
	std::this_thread::sleep_for(delay);

	const int boxWidth = 62;
	const int boxHeight = 17;
	const int boxLeft = (consoleWidth - boxWidth) / 2;
	const int boxTop = (consoleHeight - boxHeight) / 2;

	utils.DrawBox(boxLeft, boxTop, boxWidth, boxHeight);

	setupConsole.SetTextColor(1);
	std::this_thread::sleep_for(delay);

	std::size_t maxRole = 0;
	for (auto& c : creditsProfiles)
	{
		maxRole = max(maxRole, c.role.size());
	}

	UINT originalCP = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);

	for (std::size_t i = 0; i < creditsProfiles.size(); ++i)
	{
		utils.PosCursor(boxLeft + 4, boxTop + 2 + static_cast<int>(i) * 2);
		setupConsole.SetTextColor(1);
		std::cout << std::left << std::setw(static_cast<int>(maxRole) + 2) << creditsProfiles[i].role;
		setupConsole.SetTextColor(2);
		std::cout << creditsProfiles[i].name << '\n';
	}

	SetConsoleOutputCP(originalCP);

	setupConsole.SetTextColor(7);
	std::this_thread::sleep_for(delay);

	const int box2Width = 16;
	const int box2Height = 5;
	const int box2Left = (consoleWidth - box2Width) / 2;
	const int box2Top = ((consoleHeight - box2Height) / 2) + 14;

	utils.DrawBox(box2Left, box2Top, box2Width, box2Height);

	setupConsole.SetTextColor(1);
	std::this_thread::sleep_for(delay);

	SetConsoleOutputCP(CP_UTF8);

	utils.DrawMenuItems(creditsItems, boxLeft, boxTop);

	SetConsoleOutputCP(originalCP);

	setupConsole.SetTextColor(7);

	while (1)
	{
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

		switch (_getch())
		{
		case '1':
		{
			return;
		}
		default:
		{
			break;
		}
		}
	}
}