// SsdSimCmd.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <sstream>

#include "SimFramework/Framework.h"
#include "HostComm/Ipc/MessageClient.hpp"

using namespace boost::program_options;

bool parseCommandLine(int argc, const char* argv[], std::string& hardwarespecFilename, bool& interactive)
{
    try
    {
        options_description generalOptions{ "General" };
        generalOptions.add_options()
            ("help,h", "Help")
            ("hardwarespec", value<std::string>(), "Hardware specifications")
            ("interactive", "Interactive actions");

        variables_map vm;
        store(parse_command_line(argc, argv, generalOptions), vm);
        if (vm.count("hardwarespec"))
        {
            hardwarespecFilename = vm["hardwarespec"].as<std::string>();
        }
        else
        {
            std::cout << "hardwarespec is required!";
            return false;
        }
        if (vm.count("interactive"))
        {
            interactive = true;
        }
        notify(vm);
        if (vm.count("help"))
            std::cout << generalOptions << '\n';
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
        return false;
    }
    return true;
}

void printInteractiveMenu()
{
    std::stringstream sstr;
    sstr << std::endl << "SSD Simulation Commands";
    sstr << std::endl << "----";
    sstr << std::endl << "0. Exit application";
    sstr << std::endl << "----";
    sstr << std::endl;

    std::cout << sstr.str();
}

void handleInteractiveCmd()
{
    bool exit = false;
    while (!exit)
    {
        // print interactive menu
        printInteractiveMenu();

        int userSelectedCmd = -1;
        std::cout << "User input :";
        std::cin >> userSelectedCmd;

        if (0 == userSelectedCmd)
        {
            // send command exit
			//TODO: implement way to retrieve message server name
            std::shared_ptr<MessageClient<SimFrameworkCommand>> client = std::make_shared<MessageClient<SimFrameworkCommand>>("SsdSimMainMessageServer");
            Message<SimFrameworkCommand> *message = client->AllocateMessage(sizeof(SimFrameworkCommand), false);
            message->_Data._Code = SimFrameworkCommand::Code::Exit;
            client->Push(message);

            // exit app
            exit = true;
        }
        else // handle for other commands
        {
            std::cout << "Invalid commands";
            std::cout << std::endl;
        }
    }
}

int main(int argc, const char* argv[])
{
    std::string hardwarespecFilename = "";
    bool interactive = false;

    if (parseCommandLine(argc, argv, hardwarespecFilename, interactive))
    {
        if (!hardwarespecFilename.empty())
        {
            try
            {
                Framework framework;
                framework.Init(hardwarespecFilename);

                // start framework async if interative option enabled
                if (true == interactive)
                {
                    auto fwFuture = std::async(std::launch::async, &Framework::operator(), &framework);

                    // handle interactive commands
                    handleInteractiveCmd();
                }
                else // run framework in sync if no interactive option
                {
                    framework.operator()();
                }
            }
            catch (const Framework::Exception &err)
            {
                std::cout << err.what();
                return -1;
            }
        }
    }

    return 0;
}


