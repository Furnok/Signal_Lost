#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "SetupConsole.h"
#include "InterfaceGame.h"

#include <iostream>
#include <unordered_map>

class InterfaceGame;

struct Choices
{
	int choiceNumber = 0;
	bool isDefault = false;
	int nextChapter = 0;
	bool reset = false;
	bool menu = false;
	bool quit = false;
	int nextScene = 0;
	int nextSceneNotTrust = 0;
	int trustNeed = 0;
	int addTrust = 0;
	std::string text = "";
};

struct Scene
{
	int number = 0;
	bool beepBack = false;
	int connection = -1;
	int timer = 0;
	std::string content = "";
	std::vector<Choices> choices;
};

class File
{
public:
	static std::string DateTime();

	void CreateFileErrors(SetupConsole& setupConsole);
	void ReadFileError(SetupConsole& setupConsole, const std::string& key) const;

	void FileLog(SetupConsole& setupConsole, InterfaceGame& interfaceGame, std::string& inputChoice);

	void SetNextChapter(SetupConsole& setupConsole, int nextChapter);

	void Read(SetupConsole& setupConsole, InterfaceGame& interfaceGame);

	[[nodiscard]] std::string GetPathErrorsFolder() const noexcept { return this->pathErrorsFolder; }
	[[nodiscard]] std::string GetPathLogsFolder() const noexcept { return this->pathLogsFolder; }
	[[nodiscard]] std::string GetContentChapter() const noexcept { return this->contentChapter; }
	[[nodiscard]] std::vector<Scene> GetContentScenes() const noexcept { return this->contentScenes; }

	void SetPathChapter(std::string value) { this->pathChapter = std::move(value); }
	void SetInitialise(bool value) { this->initialise = std::move(value); }

private:
	std::string ExtractTag(const std::string& text, const std::string& tag) const;
	std::vector<Choices> ParseChoices(const std::string& text) const;
	std::vector<Scene> ParseScenes(const std::string& fileContent) const;

	void CreateFileLog(SetupConsole& setupConsole, InterfaceGame& interfaceGame, std::string& inputChoice);
	void AddToFileLog(InterfaceGame& interfaceGame, std::string& inputChoice) const;

	std::string errorFolderName = "Errors";
	std::string logsFolderName = "Logs";

	std::string pathErrorsFolder = "";
	std::string pathLogsFolder = "";

	std::string errorsFileName = "Errors.txt";
	std::string logsFileName = "Logs_";

	std::string pathChapter = "";
	std::string contentChapter = "";
	std::vector<Scene> contentScenes;

	bool initialise = false;

	std::unordered_map<std::string, std::string> errors
	{
		{"BadFile", "The File is not a Text File!"},
		{"FileEmpty", "The File Chapter is Empty!"},
		{"NoSceneExist", "The Scene doesn't Exist!"},
		{"NoFile", "Need to have a File!"},
		{"NoChapterExist", "The Chapter doesn't Exist!"},
		{"ChapterNumber", "The Chapter Number Header is not a Number!"},
		{"ConnectionNumber", "The Connection Number Header is not a Number!"},
		{"TrustNumber", "The Trust Number Header is not a Number!"},
		{"SceneNumber", "The Scene Number Header is not a Number!"},
		{"HeaderCorrupt", "The Header is Corrupt!"},
	};
};