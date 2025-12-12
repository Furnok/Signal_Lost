#include "InterfaceGame.h"

#include <chrono>
#include <string>
#include <windows.h>
#include <regex>
#include <future>
#include <iostream>
#include <thread>
#include <conio.h>

using namespace std::chrono;

/// <summary>
/// Display the Transition to a Chapter
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
/// <param name="file"></param>
void InterfaceGame::DisplayTransitionChapter(Utils& utils, SetupConsole& setupConsole)
{
	utils.ClearConsole();

	constexpr auto delayStart = 200ms;
	constexpr auto delayTitle = 500ms;
	constexpr auto delayLetter = 100ms;
	constexpr auto delayTransition = 2s;
	const int letterSize = 5;
	const int spaceLetter = 2;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	const int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	const std::string chapterName = this->chapterName;
	const std::string title = std::string(this->textChapter) + std::to_string(this->chapterNumber);
	const int titleWidth = static_cast<int>(((title.size() - 1) * (letterSize + spaceLetter)) + letterSize);
	int posX = (consoleWidth - titleWidth) / 2;;
	const int posY = ((consoleHeight - letterSize) / 2) - spaceLetter;

	setupConsole.SetTextColor(2);
	std::this_thread::sleep_for(delayStart);

	for (char ch : title)
	{
		utils.DrawAscii(ch, posX, posY);

		posX += letterSize + spaceLetter;;
		std::this_thread::sleep_for(delayLetter);
	}

	setupConsole.SetTextColor(7);
	std::this_thread::sleep_for(delayTitle);

	const int nameX = (consoleWidth - static_cast<int>(chapterName.size())) / 2;
	const int nameY = (consoleHeight / 2) + spaceLetter;

	UINT originalCP = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);

	utils.PosCursor(nameX, nameY);
	std::cout << this->chapterName << '\n';

	SetConsoleOutputCP(originalCP);

	std::this_thread::sleep_for(delayTransition);
}

/// <summary>
/// Display the Game UI
/// </summary>
/// <param name="utils"></param>
void InterfaceGame::DisplayInterfaceGame(Utils& utils, SetupConsole& setupConsole)
{
	utils.ClearConsole();

	constexpr auto delay = 200ms;

	std::this_thread::sleep_for(delay);

	DisplayHeader(utils, setupConsole);

	utils.DrawBox(2, 27, 96, 13);
}

/// <summary>
/// Display the Connexion Barre
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
/// <param name="file"></param>
void InterfaceGame::DisplayConnectionBarre(Utils& utils, SetupConsole& setupConsole)
{
	setupConsole.SetTextColor(1);

	const int posX = 2;
	const int posY = 1;

	int connectionPoint = this->connectionPoint;

	utils.DrawConnection(connectionPoint, posX, posY);

	setupConsole.SetTextColor(7);
}

/// <summary>
/// Display the Trust Barre
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
/// <param name="file"></param>
void InterfaceGame::DisplayTrustBarre(Utils& utils, SetupConsole& setupConsole)
{
	setupConsole.SetTextColor(1);

	const int posX = 53;
	const int posY = 2;

	int trustPoint = this->trustPoint;

	utils.DrawTrustBarre(trustPoint, posX, posY);

	setupConsole.SetTextColor(7);
}

/// <summary>
/// Display the Trust Percentage
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
/// <param name="file"></param>
void InterfaceGame::DisplayTrustPercentage(Utils& utils, SetupConsole& setupConsole)
{
	setupConsole.SetTextColor(2);

	int posX = 93;
	const int posY = 1;

	int trustPoint = this->trustPoint;

	int hundreds = trustPoint / 100;
	int tens = (trustPoint / 10) % 10;
	int ones = trustPoint % 10;

	utils.DrawAscii('0' + hundreds, posX, posY);

	posX += 7;

	utils.DrawAscii('0' + tens, posX, posY);

	posX += 7;

	utils.DrawAscii('0' + ones, posX, posY);

	posX += 7;

	utils.DrawAscii('%', posX, posY);

	setupConsole.SetTextColor(7);
}

void InterfaceGame::BeepBackground(std::future<void> stopFuture, SetupConsole& setupConsole)
{
	if (setupConsole.GetSoundActivated())
	{
		std::this_thread::sleep_for(200ms);

		while (stopFuture.wait_for(100ms) == std::future_status::timeout && this->beepBackground)
		{
			for (int i = 0; i < 3; i++)
			{
				if (this->beepBackground)
				{
					Beep(700, 300);
				}
			}

			std::this_thread::sleep_for(300ms);
		}
	}
}

void InterfaceGame::BeepText(std::future<void> stopFuture, SetupConsole& setupConsole)
{
	if (setupConsole.GetSoundActivated() && !this->beepBackground)
	{
		Beep(800, 300);
	}
}

WORD InterfaceGame::GetColorCode(const std::string& colorName)
{
	static std::unordered_map<std::string, WORD> colors =
	{
		{"Black", 0},
		{"Blue", 1},
		{"Green", 2},
		{"Cyan", 3},
		{"Red", 4},
		{"Magenta", 5},
		{"Yellow", 6},
		{"White", 7},
		{"Gray", 8},
		{"LightBlue", 9},
		{"LightGreen", 10},
		{"LightCyan", 11},
		{"LightRed", 12},
		{"LightMagenta", 13},
		{"LightYellow", 14},
		{"BrightWhite", 15}
	};

	auto it = colors.find(colorName);
	return it != colors.end() ? it->second : 7;
}

void InterfaceGame::DisplayTextWithCommands(Utils& utils, SetupConsole& setupConsole, const std::string& content, auto delayCaracter)
{
	std::regex tokenRegex(R"(\[([A-Za-z]+)=([^\]]+)\])");
	std::sregex_iterator it(content.begin(), content.end(), tokenRegex);
	std::sregex_iterator end;

	size_t lastPos = 0;

	while (lastPos < content.size())
	{
		std::smatch match;
		bool found = std::regex_search(content.begin() + lastPos, content.end(), match, tokenRegex);

		if (found)
		{
			size_t tokenPos = match.position(0) + lastPos;

			for (size_t i = lastPos; i < tokenPos; ++i)
			{
				std::cout << content[i] << std::flush;
				std::this_thread::sleep_for(delayCaracter);
			}

			std::string command = match[1];
			std::string value = match[2];

			if (command == "Wait")
			{
				int number = 0;

				try
				{
					number = std::stoi(value);
				}
				catch (...)
				{

				}

				std::this_thread::sleep_for(std::chrono::milliseconds(number));
			}
			else if (command == "Space")
			{
				int number = 1;

				try
				{
					number = std::clamp(std::stoi(value), 0, 10);
				}
				catch (...)
				{

				}

				for (int i = 0; i < number; ++i)
				{
					std::cout << "\n";
				}

				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(hConsole, &csbi);

				utils.PosCursor(2, csbi.dwCursorPosition.Y);
			}
			else if (command == "Color")
			{
				WORD colorCode = GetColorCode(value);
				setupConsole.SetTextColor(colorCode);
			}
			else if (command == "Beep")
			{
				this->stopBeepTextPromise = std::promise<void>();
				std::future<void> stopFuture = stopBeepTextPromise.get_future();

				this->beepTextFuture = std::async(std::launch::async, &InterfaceGame::BeepText, this, std::move(stopFuture), std::ref(setupConsole));
			}

			lastPos = tokenPos + match.length(0);
		}
		else
		{
			for (size_t i = lastPos; i < content.size(); ++i)
			{
				std::cout << content[i] << std::flush;
				std::this_thread::sleep_for(delayCaracter);
			}
			break;
		}
	}

	setupConsole.SetTextColor(7);
	std::cout << "\n";
}

const void InterfaceGame::DisplayText(Utils& utils, SetupConsole& setupConsole, File& file, InterfaceMainMenu& interfaceMainMenu)
{
	constexpr auto delayTransition = 200ms;
	std::this_thread::sleep_for(delayTransition);

	std::vector<Scene> scenes = file.GetContentScenes();

	int index = this->sceneNumber - 1;

	if (index >= 0 && index < static_cast<int>(scenes.size()))
	{
		Scene scene = scenes[index];

		if (scene.beepBack)
		{
			this->beepBackground = true;
			this->stopBeepPromise = std::promise<void>();
			std::future<void> stopFuture = stopBeepPromise.get_future();

			this->beepFuture = std::async(std::launch::async, &InterfaceGame::BeepBackground, this, std::move(stopFuture), std::ref(setupConsole));
		}

		if (scene.connection >= 0)
		{
			this->connectionPoint = scene.connection;
			DisplayConnectionBarre(utils, setupConsole);
		}

		if (scene.timer > 0)
		{
			this->isTimer = true;
			this->timer = scene.timer;
		}

		utils.PosCursor(2, 9);

		UINT originalCP = GetConsoleOutputCP();
		SetConsoleOutputCP(CP_UTF8);

		constexpr auto delayCaracter = 20ms;
		DisplayTextWithCommands(utils, setupConsole, scene.content, delayCaracter);

		const int posX = 6;
		int posY = 29;

		setupConsole.SetTextColor(1);

		for (auto& c : scene.choices)
		{
			utils.PosCursor(posX, posY);

			std::cout << c.choiceNumber << " - " << c.text << "\n";

			posY += 2;
		}

		setupConsole.SetTextColor(7);

		SetConsoleOutputCP(originalCP);

		InputChoice(utils, setupConsole, scene, file, interfaceMainMenu);
	}
	else
	{
		file.ReadFileError(setupConsole, "NoSceneExist");
	}
}

std::string InterfaceGame::UserInput(std::future<void> stopFuture, bool& finish, Scene& scene)
{
	std::string input = "";

	while (!finish)
	{
		if (_kbhit())
		{
			char ch = _getch();
			input = ch;

			try
			{
				int choice = std::stoi(std::string(1, ch));

				if (choice > 0 && choice <= scene.choices.size())
				{
					break;
				}
			}
			catch (...)
			{

			}
		}
	}

	return input;
}

void InterfaceGame::InputChoice(Utils& utils, SetupConsole& setupConsole, Scene& scene, File& file, InterfaceMainMenu& interfaceMainMenu)
{
	bool succed = false;
	std::string input = "";

	while (!succed)
	{
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

		if (this->isTimer)
		{
			bool finish = false;

			this->stopTimerPromise = std::promise<void>();
			std::future<void> stopFuture = stopTimerPromise.get_future();

			this->timerFuture = std::async(std::launch::async, &InterfaceGame::UserInput, this, std::move(stopFuture), std::ref(finish), std::ref(scene));

			while (!finish)
			{
				finish = Timer(utils, setupConsole);

				if (finish)
				{
					succed = true;

					input = this->timerFuture.get();

					input = "1";

					for (const auto& choice : scene.choices)
					{
						if (choice.isDefault)
						{
							input = std::to_string(choice.choiceNumber);
							break;
						}
					}

					break;
				}
				else if (this->timerFuture.wait_for(std::chrono::seconds(1)) == std::future_status::ready)
				{
					succed = true;

					input = this->timerFuture.get();

					break;
				}

				this->timer -= 1;
			}
		}
		else
		{
			char ch = _getch();
			input = ch;

			try
			{
				int choice = std::stoi(std::string(1, ch));

				if (choice > 0 && choice <= scene.choices.size())
				{
					succed = true;
				}
			}
			catch (...)
			{
				succed = false;
			}
		}
	}

	if (succed)
	{
		this->beepBackground = false;
		this->timer = false;
		this->isTimer = false;

		utils.ClearAreaConsole(0, 8, 120, 26);
		utils.ClearAreaConsole(3, 28, 96, 38);
		utils.ClearAreaConsole(98, 28, 120, 38);

		file.FileLog(setupConsole, *this, input);

		Choices choice = scene.choices[std::stoi(input) - 1];

		if (choice.menu)
		{
			file.SetInitialise(false);
			interfaceMainMenu.SetDisplay(true);
			this->display = true;
		}
		else if (choice.quit)
		{
			exit(EXIT_SUCCESS);
		}
		else
		{
			if (choice.nextChapter != 0)
			{
				if (choice.reset)
				{
					file.SetInitialise(false);
				}

				this->chapterNumber = choice.nextChapter;
				this->display = true;

				file.SetNextChapter(setupConsole, choice.nextChapter);
			}

			if (choice.nextScene != 0)
			{
				if (this->trustPoint >= choice.trustNeed)
				{
					this->sceneNumber = choice.nextScene;
				}
				else
				{
					this->sceneNumber = choice.nextSceneNotTrust;
				}
			}

			if (choice.addTrust != 0)
			{
				this->trustPoint = (std::clamp(this->trustPoint + choice.addTrust, 0, 100) / 25) * 25;
				DisplayTrustBarre(utils, setupConsole);
				DisplayTrustPercentage(utils, setupConsole);
			}
		}
	}
}

bool InterfaceGame::Timer(Utils& utils, SetupConsole& setupConsole)
{
	bool result = false;

	TimerShow(utils, setupConsole);

	if (!this->noBeepSound && setupConsole.GetSoundActivated() && !this->beepBackground)
	{
		Beep(500, 300);
	}

	setupConsole.SetTextColor(7);

	if (this->timer <= 0)
	{
		result = true;
		this->noBeepSound = false;
	}

	return result;
}

void InterfaceGame::TimerShow(Utils& utils, SetupConsole& setupConsole)
{
	int posX = 100;
	const int posY = 31;

	int timer = this->timer;

	if (timer <= 10)
	{
		setupConsole.SetTextColor(4);
	}
	else
	{
		setupConsole.SetTextColor(2);
	}

	int hundreds = timer / 100;
	int tens = (timer / 10) % 10;
	int ones = timer % 10;

	utils.DrawAscii('0' + hundreds, posX, posY);

	posX += 7;

	utils.DrawAscii('0' + tens, posX, posY);

	posX += 7;

	utils.DrawAscii('0' + ones, posX, posY);
}

/// <summary>
/// Display the Header
/// </summary>
/// <param name="utils"></param>
/// <param name="setupConsole"></param>
/// <param name="file"></param>
void InterfaceGame::DisplayHeader(Utils& utils, SetupConsole& setupConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	int posX = 0;
	int posY = 7;

	for (int i = 0; i < consoleWidth; i++)
	{
		utils.PosCursor(posX + i, posY);
		std::cout << char(223) << "\n";
	}

	posX += 14;

	for (int i = 0; i < posY; i++)
	{
		utils.PosCursor(posX, i);
		std::cout << char(219) << "\n";
	}

	DisplayConnectionBarre(utils, setupConsole);

	setupConsole.SetTextColor(2);

	utils.DrawGameTitle(trustTitle, 17, 1);

	setupConsole.SetTextColor(7);

	utils.DrawBox(52, 1, 39, 5);

	DisplayTrustBarre(utils, setupConsole);

	DisplayTrustPercentage(utils, setupConsole);
}