// RomCode.cpp : Defines the exported functions for the DLL application.
//

#include <memory>

#include "InterfaceQueues.h"

void Execute(std::shared_ptr<InterfaceQueues> InterfaceQueues)
{
    if (!InterfaceQueues->CustomProtocolCommandQueue.empty())
    {
        CustomProtocolCommand *command = InterfaceQueues->CustomProtocolCommandQueue.front();
        InterfaceQueues->CustomProtocolCommandQueue.pop();
        switch (command->Command)
        {
            case CommandCode::DownloadAndExecute:
            {
                // Do download and execute
            } break;
        }
    }
}




// Next
// Add the interface queue structure with atomic            Done
// RomCode check the interface queue to detect command
// Download file
// Load (may be change the SetExecute function
// 
