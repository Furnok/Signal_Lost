#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "SetupConsole.h"
#include "Utils.h"
#include "File.h"
#include "InterfaceMainMenu.h"

#include <future>

class File;
struct Scene;

class InterfaceGame
{
public:
	void DisplayTransitionChapter(Utils& utils, SetupConsole& setupConsole);

	void DisplayInterfaceGame(Utils& utils, SetupConsole& setupConsole);

	void DisplayConnectionBarre(Utils& utils, SetupConsole& setupConsole);
	void DisplayTrustBarre(Utils& utils, SetupConsole& setupConsole);
	void DisplayTrustPercentage(Utils& utils, SetupConsole& setupConsole);

	void BeepBackground(std::future<void> stopSignal, SetupConsole& setupConsole);

	std::string UserInput(std::future<void> stopFuture, bool& finish, Scene& scene);

	const void DisplayText(Utils& utils, SetupConsole& setupConsole, File& file, InterfaceMainMenu& interfaceMainMenu);

	bool Timer(Utils& utils, SetupConsole& setupConsole);
	void TimerShow(Utils& utils, SetupConsole& setupConsole);

	[[nodiscard]] int GetChapterNumber() const noexcept { return this->chapterNumber; }
	[[nodiscard]] std::string GetChapterName() const noexcept { return this->chapterName; }
	[[nodiscard]] int GetConnectionPoint() const noexcept { return this->connectionPoint; }
	[[nodiscard]] int GetTrustPoint() const noexcept { return this->trustPoint; }
	[[nodiscard]] int GetSceneNumber() const noexcept { return this->sceneNumber; }

	[[nodiscard]] bool GetDisplay() const noexcept { return this->display; }

	void SetDisplay(bool value) { this->display = std::move(value); }
	void SetChapterNumber(int value) { this->chapterNumber = std::move(value); }
	void SetChapterName(std::string value) { this->chapterName = std::move(value); }
	void SetConnectionPoint(int value) { this->connectionPoint = std::move(value); }
	void SetTrustPoint(int value) { this->trustPoint = std::move(value); }
	void SetSceneNumber(int value) { this->sceneNumber = std::move(value); }

private:
	void DisplayHeader(Utils& utils, SetupConsole& setupConsole);

	WORD GetColorCode(const std::string& colorName);

	void BeepText(std::future<void> stopFuture, SetupConsole& setupConsole);

	void DisplayTextWithCommands(Utils& utils, SetupConsole& setupConsole, const std::string& content, auto delayCaracter);

	void InputChoice(Utils& utils, SetupConsole& setupConsole, Scene& scene, File& file, InterfaceMainMenu& interfaceMainMenu);

	static constexpr std::string_view textChapter = "CHAPTER-";
	static constexpr std::string_view trustTitle = "TRUST";

	int chapterNumber = 0;
	std::string chapterName = "Empty";
	int connectionPoint = 0;
	int trustPoint = 0;
	int sceneNumber = 0;
	int timer = 0;
	bool isTimer = false;
	bool noBeepSound = false;
	bool beepBackground = false;

	std::future<void> beepFuture;
	std::promise<void> stopBeepPromise;

	std::future<void> beepTextFuture;
	std::promise<void> stopBeepTextPromise;

	std::future<std::string> timerFuture;
	std::promise<void> stopTimerPromise;

	bool display = true;
};