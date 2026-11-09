#include "HelloGame/HelloGame.hpp"

#include <exception>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[])
{
    const bool smokeTest = argc == 2 && std::string_view(argv[1]) == "--smoke-test";

    try
    {
        HelloGame game(smokeTest);
        game.Run();
        if (smokeTest)
            std::cout << "cna-template: Game::Run returned; destroying game\n";
    }
    catch (const std::exception& error)
    {
        // Game::Run has logged the error and rethrown it, as XNA does. Letting it leave main()
        // calls std::terminate(); under MSVC's Debug runtime the resulting abort() opens a modal
        // dialog that blocks a headless run until it is killed. End with a failure code instead,
        // as an unhandled exception ends a .NET game.
        std::cerr << "cna-template: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
