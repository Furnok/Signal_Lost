#include "File.h"

#include <filesystem>
#include <fstream>
#include <conio.h>
#include <regex>

using namespace std::chrono;

/// <summary>
/// Get the Date and the Time
/// </summary>
/// <returns></returns>
std::string File::DateTime()
{
	const auto now = std::chrono::system_clock::now();

	const time_t now_time_t = std::chrono::system_clock::to_time_t(now);

	tm local{};
	localtime_s(&local, &now_time_t);

	char buffer[20];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &local);

	return buffer;
}

/// <summary>
/// Create the File Errors
/// </summary>
/// <param name="setupConsole"></param>
void File::CreateFileErrors(SetupConsole& setupConsole)
{
	std::filesystem::path path = std::filesystem::path(setupConsole.GetPathGameFolder()) / errorFolderName;
	this->pathErrorsFolder = path.string() + '\\';

	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directories(path);
	}

	const std::filesystem::path errorFile = path / this->errorsFileName;

	if (!std::filesystem::exists(errorFile))
	{
		std::ofstream outfile(errorFile, std::ios::out);

		for (auto& [key, msg] : errors)
		{
			outfile << key << '=' << msg << std::endl;
		}

		outfile.close();
	}
}

/// <summary>
/// Read the File Errors
/// </summary>
/// <param name="setupConsole"></param>
/// <param name="key"></param>
void File::ReadFileError(SetupConsole& setupConsole, const std::string& key) const
{
	system("cls");

	UINT originalCP = GetConsoleOutputCP();
	SetConsoleOutputCP(CP_UTF8);

	setupConsole.SetTextColor(4);

	const std::filesystem::path path = std::filesystem::path(GetPathErrorsFolder()) / this->errorsFileName;

	std::ifstream infile(path, std::ios::in);

	if (!infile)
	{
		std::cout << "Error: Unable to Open the Errors File!" << std::endl;
	}
	else
	{
		std::string line;

		while (getline(infile, line))
		{
			if (auto pos = line.find('='); pos != std::string::npos)
			{
				if (line.substr(0, pos) == key)
				{
					std::cout << line.substr(pos + 1) << std::endl;

					infile.close();

					setupConsole.SetTextColor(7);

					(void)_getch();

					exit(EXIT_SUCCESS);
				}
			}
		}

		std::cout << "Error: Unable to Find the Error!" << std::endl;
	}

	infile.close();

	SetConsoleOutputCP(originalCP);

	setupConsole.SetTextColor(7);

	(void)_getch();

	exit(EXIT_SUCCESS);
}

/// <summary>
/// Logs Manager for the Logs File
/// </summary>
/// <param name="setupConsole"></param>
/// <param name="inputChoice"></param>
void File::FileLog(SetupConsole& setupConsole, InterfaceGame& interfaceGame, std::string& inputChoice)
{
	if (this->pathLogsFolder.empty())
	{
		CreateFileLog(setupConsole, interfaceGame, inputChoice);
	}
	else
	{
		AddToFileLog(interfaceGame, inputChoice);
	}
}

void File::SetNextChapter(SetupConsole& setupConsole, int nextChapterNumber)
{
	std::filesystem::path currentPath(this->pathChapter);
	std::filesystem::path folder = currentPath.parent_path();
	std::string filename = "Chapter-" + std::to_string(nextChapterNumber) + ".txt";
	std::filesystem::path nextPath = folder / filename;

	std::cout << nextPath << std::endl;

	if (std::filesystem::exists(nextPath))
	{
		this->pathChapter = nextPath.string();
	}
	else
	{
		constexpr auto delayTransition = 60s;
		std::this_thread::sleep_for(delayTransition);

		ReadFileError(setupConsole, "NoChapterExist");
	}
}

/// <summary>
/// Read the Chapter File
/// </summary>
/// <param name="setupConsole"></param>
void File::Read(SetupConsole& setupConsole, InterfaceGame& interfaceGame)
{
	std::string contentLine = "";
	bool headerRead = false;

	std::regex pairRegex(R"(\[([^\]=]+)=([^\]]+)\])");
	const std::vector<std::string> requiredKeys = { "Chapter", "Name", "Connection", "Trust", "StartScene" };
	std::unordered_map<std::string, bool> keyFound;
	for (auto& key : requiredKeys)
	{
		keyFound[key] = false;
	}

	this->contentChapter.clear();

	if (std::filesystem::path(this->pathChapter).extension() != ".txt")
	{
		ReadFileError(setupConsole, "BadFile");
	}

	if (std::filesystem::file_size(this->pathChapter) == 0)
	{
		ReadFileError(setupConsole, "FileEmpty");
	}

	std::ifstream infile(this->pathChapter, std::ios::in);

	while (getline(infile, contentLine))
	{
		this->contentChapter += contentLine;

		if (!headerRead)
		{
			headerRead = true;

			for (std::smatch match; regex_search(contentLine, match, pairRegex); contentLine = match.suffix())
			{
				const std::string key = match[1];
				const std::string value = match[2];

				auto safeInt = [&](const char* errKey)
					{
						try
						{
							return stoi(value);
						}
						catch (...)
						{
							ReadFileError(setupConsole, errKey);
							return 0;
						}
					};

				if (key == "Chapter")
				{
					interfaceGame.SetChapterNumber(std::clamp(safeInt("ChapterNumber"), 0, 999999999));
					keyFound[key] = true;
				}
				else if (key == "Name")
				{
					interfaceGame.SetChapterName(value.substr(0, 117));
					keyFound[key] = true;
				}
				else if (key == "Connection")
				{
					if (!initialise)
					{
						interfaceGame.SetConnectionPoint(std::clamp(safeInt("ConnectionNumber"), 0, 4));
					}

					keyFound[key] = true;
				}
				else if (key == "Trust")
				{
					if (!initialise)
					{
						interfaceGame.SetTrustPoint((std::clamp(safeInt("TrustNumber"), 0, 100) / 25) * 25);
					}

					keyFound[key] = true;
				}
				else if (key == "StartScene")
				{
					if (!initialise)
					{
						interfaceGame.SetSceneNumber(max(0, safeInt("SceneNumber")));
					}

					keyFound[key] = true;
				}
			}
		}
	}

	for (auto& [key, found] : keyFound)
	{
		if (!found)
		{
			ReadFileError(setupConsole, "HeaderCorrupt");
		}
	}

	initialise = true;

	infile.close();

	this->contentScenes = ParseScenes(this->contentChapter);
}

/// <summary>
/// Extract the Tag
/// </summary>
/// <param name="text"></param>
/// <param name="tag"></param>
/// <returns></returns>
std::string File::ExtractTag(const std::string& text, const std::string& tag) const
{
	size_t pos = text.find("[" + tag + "=");
	if (pos == std::string::npos) return "";

	size_t start = pos + tag.size() + 2;
	while (start < text.size() && (text[start] == '\n' || text[start] == '\r' || text[start] == ' ')) start++;

	int bracketCount = 0;
	size_t i = start;
	for (; i < text.size(); ++i) {
		if (text[i] == '[') bracketCount++;
		else if (text[i] == ']') {
			if (bracketCount == 0) break;
			bracketCount--;
		}
	}

	return text.substr(start, i - start);
}

/// <summary>
/// Parse the Scene Choices
/// </summary>
/// <param name="text"></param>
/// <returns></returns>
std::vector<Choices> File::ParseChoices(const std::string& text) const
{
	std::vector<Choices> choices;
	size_t pos = 0;

	while ((pos = text.find("[Choice", pos)) != std::string::npos)
	{
		if (choices.size() >= 5)
		{
			break;
		}

		size_t start = pos;
		int bracketCount = 0;
		size_t i = pos;

		for (; i < text.size(); ++i) {
			if (text[i] == '[') bracketCount++;
			else if (text[i] == ']') bracketCount--;
			if (bracketCount == 0) break;
		}

		std::string choiceText = text.substr(start, i - start + 1);

		Choices c;


		size_t choicePos = text.find("[Choice", pos);
		if (choicePos != std::string::npos)
		{
			size_t equalPos = text.find('=', choicePos);
			if (equalPos != std::string::npos)
			{
				std::string numberStr = text.substr(choicePos + 7, equalPos - (choicePos + 7));

				int number = 0;

				try
				{
					number = std::stoi(numberStr);
				}
				catch (...)
				{

				}

				c.choiceNumber = number;
			}
		}

		size_t defPos = choiceText.find("[Default=");
		if (defPos != std::string::npos)
		{
			size_t end = choiceText.find(']', defPos);
			std::string value = choiceText.substr(defPos + 9, end - defPos - 9);
			c.isDefault = (value == "True" || value == "true");
		}

		size_t nextChapterPos = choiceText.find("[NextChapter=");
		if (nextChapterPos != std::string::npos)
		{
			size_t end = choiceText.find(']', nextChapterPos);
			std::string value = choiceText.substr(nextChapterPos + 13, end - nextChapterPos - 13);

			int number = -1;

			try
			{
				number = std::clamp(std::stoi(value), 0, 999999999);
			}
			catch (...)
			{

			}

			c.nextChapter = number;
		}

		size_t resetPos = choiceText.find("[Reset=");
		if (resetPos != std::string::npos)
		{
			size_t end = choiceText.find(']', resetPos);
			std::string value = choiceText.substr(resetPos + 7, end - resetPos - 7);
			c.reset = (value == "True" || value == "true");
		}

		size_t menuPos = choiceText.find("[Menu=");
		if (menuPos != std::string::npos)
		{
			size_t end = choiceText.find(']', menuPos);
			std::string value = choiceText.substr(menuPos + 6, end - menuPos - 6);
			c.menu = (value == "True" || value == "true");
		}

		size_t quitPos = choiceText.find("[Quit=");
		if (quitPos != std::string::npos)
		{
			size_t end = choiceText.find(']', quitPos);
			std::string value = choiceText.substr(quitPos + 6, end - quitPos - 6);
			c.quit = (value == "True" || value == "true");
		}

		size_t nextScenePos = choiceText.find("[NextScene=");
		if (nextScenePos != std::string::npos)
		{
			size_t end = choiceText.find(']', nextScenePos);
			std::string value = choiceText.substr(nextScenePos + 11, end - nextScenePos - 11);

			int number = -1;

			try
			{
				number = max(0, std::stoi(value));
			}
			catch (...)
			{

			}

			c.nextScene = number;
		}

		size_t nextSceneNoTrustPos = choiceText.find("[NextSceneNoTrust=");
		if (nextSceneNoTrustPos != std::string::npos)
		{
			size_t end = choiceText.find(']', nextSceneNoTrustPos);
			std::string value = choiceText.substr(nextSceneNoTrustPos + 18, end - nextSceneNoTrustPos - 18);

			int number = -1;

			try
			{
				number = max(0, std::stoi(value));
			}
			catch (...)
			{

			}

			c.nextSceneNotTrust = number;
		}

		size_t trustNeedPos = choiceText.find("[TrustNeed=");
		if (trustNeedPos != std::string::npos)
		{
			size_t end = choiceText.find(']', trustNeedPos);
			std::string value = choiceText.substr(trustNeedPos + 11, end - trustNeedPos - 11);

			int number = 0;

			try
			{
				number = (std::clamp(std::stoi(value), 0, 100) / 25) * 25;
			}
			catch (...)
			{

			}

			c.trustNeed = number;
		}

		size_t trustAddPos = choiceText.find("[AddTrust=");
		if (trustAddPos != std::string::npos)
		{
			size_t end = choiceText.find(']', trustAddPos);
			std::string value = choiceText.substr(trustAddPos + 10, end - trustAddPos - 10);

			int number = 0;

			try
			{
				number = (std::clamp(std::stoi(value), -100, 100) / 25) * 25;
			}
			catch (...)
			{

			}

			c.addTrust = number;
		}

		size_t contentPos = choiceText.find("[Content=");
		if (contentPos != std::string::npos)
		{
			size_t end = choiceText.find(']', contentPos);
			c.text = choiceText.substr(contentPos + 9, end - contentPos - 9);
		}

		choices.push_back(c);
		pos = i + 1;
	}

	return choices;
}

/// <summary>
/// Parse the Scenes
/// </summary>
/// <param name="fileContent"></param>
/// <returns></returns>
std::vector<Scene> File::ParseScenes(const std::string& fileContent) const
{
	std::vector<Scene> scenes;
	size_t pos = 0;

	while ((pos = fileContent.find("[Scene=", pos)) != std::string::npos)
	{
		size_t start = pos;
		int bracketCount = 0;
		size_t i = pos;

		for (; i < fileContent.size(); ++i)
		{
			if (fileContent[i] == '[') bracketCount++;
			else if (fileContent[i] == ']') bracketCount--;
			if (bracketCount == 0) break;
		}

		std::string sceneText = fileContent.substr(start, i - start + 1);

		size_t eq = sceneText.find('=');
		size_t firstBracket = sceneText.find('[');
		Scene scene;

		int number = 0;

		try
		{
			number = std::stoi(sceneText.substr(eq + 1, firstBracket - eq - 1));
		}
		catch (...)
		{

		}

		scene.number = number;

		std::string beepValue = ExtractTag(sceneText, "BeepBack");
		scene.beepBack = (beepValue == "True" || beepValue == "true");

		std::string connValue = ExtractTag(sceneText, "Connection");
		if (!connValue.empty())
		{
			number = 0;

			try
			{
				number = std::clamp(std::stoi(connValue), 0, 4);
			}
			catch (...)
			{

			}

			scene.connection = number;
		}

		std::string timerValue = ExtractTag(sceneText, "Timer");
		if (!timerValue.empty())
		{
			number = 0;

			try
			{
				number = std::clamp(std::stoi(timerValue), 0, 999);
			}
			catch (...)
			{

			}

			scene.timer = number;
		}

		std::string contentValue = ExtractTag(sceneText, "Content");
		scene.content = contentValue;

		std::string choicesValue = ExtractTag(sceneText, "Choices");
		scene.choices = ParseChoices(choicesValue);

		scenes.push_back(scene);
		pos = i + 1;
	}

	return scenes;
}

/// <summary>
/// Create the File Logs
/// </summary>
/// <param name="setupConsole"></param>
/// <param name="inputChoice"></param>
void File::CreateFileLog(SetupConsole& setupConsole, InterfaceGame& interfaceGame, std::string& inputChoice)
{
	std::filesystem::path path = std::filesystem::path(setupConsole.GetPathGameFolder()) / this->logsFolderName;
	this->pathLogsFolder = path.string() + '\\';

	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directories(path);
	}

	this->logsFileName += DateTime() + ".txt";

	std::ofstream outfile(path / this->logsFileName, std::ios::out);

	outfile << "Chapter: " << interfaceGame.GetChapterNumber() << ", " << "Scene: " << interfaceGame.GetSceneNumber() << ", " << "Input Choice: " << inputChoice << "\n";

	outfile.close();
}

/// <summary>
/// Add to the File Logs
/// </summary>
/// <param name="inputChoice"></param>
void File::AddToFileLog(InterfaceGame& interfaceGame, std::string& inputChoice) const
{
	std::filesystem::path path = std::filesystem::path(GetPathLogsFolder()) / this->logsFileName;

	std::ofstream outfile(path, std::ios::app);

	outfile << "Chapter: " << interfaceGame.GetChapterNumber() << ", " << "Scene: " << interfaceGame.GetSceneNumber() << ", " << "Input Choice: " << inputChoice << "\n";

	outfile.close();
}