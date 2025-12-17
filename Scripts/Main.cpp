#include "Main.h"

/// <summary>
/// Main Function
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[])
{
    SetupConsole setupConsole;
    File file;
    InterfaceMainMenu interfaceMainMenu;
    InterfaceGame interfaceGame;
    Utils utils;

    setupConsole.InitConsole();

    file.CreateFileErrors(setupConsole);

    if (argc < 2)
    {
        file.ReadFileError(setupConsole, "NoFile");
    }
    else
    {
        std::string path = argv[1];
        file.SetPathChapter(path);

        while (1)
        {
            file.Read(setupConsole, interfaceGame);

            if (interfaceMainMenu.GetDisplay())
            {
                interfaceMainMenu.SetDisplay(false);
                //interfaceMainMenu.DisplayMainMenu(utils, setupConsole);
            }

            if (interfaceGame.GetDisplay())
            {
                interfaceGame.SetDisplay(false);
                //interfaceGame.DisplayTransitionChapter(utils, setupConsole);
            }

            interfaceGame.DisplayInterfaceGame(utils, setupConsole);

            while (!interfaceMainMenu.GetDisplay() && !interfaceGame.GetDisplay())
            {
                interfaceGame.DisplayText(utils, setupConsole, file, interfaceMainMenu);
            }
        }
    }

    std::cout << "\nPress Enter to exit...\n";
    std::cin.get();

    return 0;
}