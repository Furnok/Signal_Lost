#include "Utils.h"

#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std::chrono;

/// <summary>
/// Clear the Console
/// </summary>
void Utils::ClearConsole() const noexcept
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!hOut) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;

    const DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD written{};
    FillConsoleOutputCharacter(hOut, TEXT(' '), size, { 0, 0 }, &written);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, size, { 0, 0 }, &written);
    SetConsoleCursorPosition(hOut, { 0, 0 });
}

/// <summary>
/// Clear the Area of the Console
/// </summary>
void Utils::ClearAreaConsole(int xStart, int yStart, int xEnd, int yEnd) const noexcept
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!hOut) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;

    xStart = max(0, xStart);
    yStart = max(0, yStart);
    xEnd = min(csbi.dwSize.X - 1, xEnd);
    yEnd = min(csbi.dwSize.Y - 1, yEnd);

    DWORD written{};
    const WORD attributes = csbi.wAttributes;

    for (int y = yStart; y <= yEnd; ++y)
    {
        const int width = xEnd - xStart + 1;
        const COORD startCoord = { static_cast<SHORT>(xStart), static_cast<SHORT>(y) };

        FillConsoleOutputCharacter(hOut, TEXT(' '), width, startCoord, &written);
        FillConsoleOutputAttribute(hOut, attributes, width, startCoord, &written);
    }

    SetConsoleCursorPosition(hOut, { static_cast<SHORT>(xStart), static_cast<SHORT>(yStart) });
}

/// <summary>
/// Position the Cursor
/// </summary>
/// <param name="posX"></param>
/// <param name="posY"></param>
void Utils::PosCursor(int posX, int posY) const noexcept
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ SHORT(posX), SHORT(posY) });
}

/// <summary>
/// Draw the Main Menu Title
/// </summary>
/// <param name="text"></param>
/// <param name="consoleWidth"></param>
void Utils::DrawMainMenuTitle(const std::string_view& text, const int consoleWidth) const
{
    constexpr auto delayLetter = 100ms;
    const int letterSize = 5;
    const int spaceLetter = 2;

    const int titleWidth = static_cast<int>(((text.size() - 1) * (letterSize + spaceLetter)) + letterSize);

    int posX = (consoleWidth - titleWidth) / 2;
    const int posY = 4;

    for (char ch : text)
    {
        DrawAscii(ch, posX, posY);

        posX += letterSize + spaceLetter;
        std::this_thread::sleep_for(delayLetter);
    }
}

/// <summary>
/// Draw the Game Title
/// </summary>
/// <param name="text"></param>
/// <param name="consoleWidth"></param>
void Utils::DrawGameTitle(const std::string_view& text, const int posX, const int posY) const
{
    const int letterSize = 5;
    const int spaceLetter = 2;

    int positionX = posX;

    for (char ch : text)
    {
        DrawAscii(ch, positionX, posY);

        positionX += letterSize + spaceLetter;
    }
}

/// <summary>
/// Draw a Box
/// </summary>
/// <param name="left"></param>
/// <param name="top"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void Utils::DrawBox(int left, int top, int width, int height) const
{
    if (width < 2 || height < 2) return;

    auto drawLine = [&](int y, char leftChar, char midChar, char rightChar)
        {
            PosCursor(left, y);

            std::cout << leftChar;

            for (int i = 1; i < width - 1; ++i)
            {
                std::cout << midChar;
            }

            std::cout << rightChar;
        };

    drawLine(top, char(220), char(220), char(220));

    for (int y = 1; y < height - 1; ++y)
    {
        drawLine(top + y, char(219), ' ', char(219));
    }

    drawLine(top + height - 1, char(223), char(223), char(223));
}

/// <summary>
/// Draw the Menu Items
/// </summary>
/// <param name="items"></param>
/// <param name="left"></param>
/// <param name="top"></param>
void Utils::DrawMenuItems(const std::vector<std::string_view>& items, int left, int top)
{
    for (std::size_t i = 0; i < items.size(); ++i)
    {
        PosCursor(left + 4, top + 2 + static_cast<int>(i) * 2);
        std::cout << items[i] << '\n';
    }
}

/// <summary>
/// Draw a Caracter ASCII
/// </summary>
/// <param name="ch"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
void Utils::DrawAscii(char ch, int posX, int posY) const
{
    ch = std::toupper(static_cast<unsigned char>(ch));
    auto it = ascii.find(ch);
    if (it == ascii.end()) return;

    UINT originalCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);

    for (size_t i = 0; i < it->second.size(); ++i)
    {
        PosCursor(posX, posY + static_cast<int>(i));
        std::cout << it->second[i];
    }

    SetConsoleOutputCP(originalCP);
}

/// <summary>
/// Draw the Connection Barre
/// </summary>
/// <param name="number"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
void Utils::DrawConnection(int number, int posX, int posY) const
{
    auto it = connection.find(number);
    if (it == connection.end()) return;

    UINT originalCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);

    for (size_t i = 0; i < it->second.size(); ++i)
    {
        PosCursor(posX, posY + static_cast<int>(i));
        std::cout << it->second[i];
    }

    SetConsoleOutputCP(originalCP);
}

/// <summary>
/// Draw the Trust Barre
/// </summary>
/// <param name="number"></param>
/// <param name="posX"></param>
/// <param name="posY"></param>
void Utils::DrawTrustBarre(int number, int posX, int posY) const
{
    auto it = trust.find(number);
    if (it == trust.end()) return;

    UINT originalCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);

    for (size_t i = 0; i < it->second.size(); ++i)
    {
        PosCursor(posX, posY + static_cast<int>(i));
        std::cout << it->second[i];
    }

    SetConsoleOutputCP(originalCP);
}
