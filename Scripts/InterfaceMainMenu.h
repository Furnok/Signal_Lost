#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "SetupConsole.h"
#include "Utils.h"

#include <vector>

class InterfaceMainMenu
{
public:
	void DisplayMainMenu(Utils& utils, SetupConsole& setupConsole);

	[[nodiscard]] bool GetDisplay() const noexcept { return this->display; }

	void SetDisplay(bool value) { this->display = std::move(value); }

private:
	void UpdateSettingSoundActivated(Utils& utils, SetupConsole& setupConsole, int right, int top) const;
	void UpdateSettingFontSize(Utils& utils, SetupConsole& setupConsole, int right, int top) const;
	void DisplaySettings(Utils& utils, SetupConsole& setupConsole);

	void DisplayCredits(Utils& utils, SetupConsole& setupConsole);

	static constexpr std::string_view gameTitle = "--SIGNAL LOST--";
	static constexpr std::string_view settingsTitle = "SETTINGS";
	static constexpr std::string_view creditsTitle = "CREDITS";

	static inline const std::vector<std::string_view> mainMenuItems =
	{
		"1 - Start",
		"2 - Settings",
		"3 - Credits",
		"4 - Quit"
	};

	const std::vector<int> FONT_SIZES = { 14, 18, 22, 26, 30, 34 };

	static inline const std::vector<std::string_view> settingsItems =
	{
		"1 - Sound:",
		"2 - FontSize:",
		"3 - Back",
	};

	const struct CreditEntry
	{
		std::string_view role;
		std::string_view name;
	};

	static inline const std::vector<CreditEntry> creditsProfiles =
	{
		{"Project Manager & Lead Game Designer:", "Baptiste PERRIN"},
		{"Game Designer:", "Loic VAREILLES"},
		{""},
		{"Lead Game Programmer:", "Killian FURNO"},
		{"Game Programmer:", "Adrien LECHAT"},
		{"Game Programmer:", "Hortense MARIE"},
		{"Game Programmer:", "Florian MARTINS"},
	};

	static inline const std::vector<std::string_view> creditsItems =
	{
		"1 - Back",
	};

	bool display = true;
};