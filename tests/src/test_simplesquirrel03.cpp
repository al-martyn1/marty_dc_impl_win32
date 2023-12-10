/*! \file
    \brief Test for virtual filesystem
*/

#include "umba/umba.h"
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <memory>
#include <random>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <functional>
#include <utility>


// #pragma warning(disable:4717)

#include "umba/filesys.h"
#include "marty_virtual_fs/app_paths_impl.h"
#include "marty_virtual_fs/virtual_fs_impl.h"
#include "marty_virtual_fs/i_filesystem.h"
#include "marty_virtual_fs/filesystem_impl.h"



int main( int argc, char* argv[] )
{
    UMBA_USED(argc);
    UMBA_USED(argv);

    // umba::filesys::createDirectoryEx(std::string("C:/temp/aaa/bbb/cc"), true);

    // auto appPaths = marty_virtual_fs::AppPathsImpl();

    {
        auto pAppPathsImpl   = std::make_shared<marty_virtual_fs::AppPathsImpl>();
    
        // https://en.cppreference.com/w/cpp/memory/shared_ptr
        // https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
        auto pAppPaths       = std::static_pointer_cast<marty_virtual_fs::IAppPaths>(pAppPathsImpl);
        auto pAppPathsCommon = std::static_pointer_cast<marty_virtual_fs::IAppPathsCommon>(pAppPathsImpl);
    
        pAppPathsCommon->setAppCommonHomeSubPath("dotNut");
    
        std::string exeFullName;
        std::string exeName;
        std::string exePath;
        std::string binRoot;
        std::string appRoot;
        std::string appHome;
        std::string appTemp;
    
        pAppPaths->getAppExeFullPathName(exeFullName);
        pAppPaths->getAppExeName(exeName);
        pAppPaths->getAppExePath(exePath);
        pAppPaths->getAppBinRootPath(binRoot);
        pAppPaths->getAppRootPath(appRoot);
        pAppPaths->getAppHomePathEx(appHome, true);
        pAppPaths->getAppTempPathEx(appTemp, true);
    
        std::cout << "exeFullName: " << exeFullName << "\n";
        std::cout << "exeName: " << exeName << "\n";
        std::cout << "exePath: " << exePath << "\n";
        std::cout << "binRoot: " << binRoot << "\n";
        std::cout << "appRoot: " << appRoot << "\n";
        std::cout << "appHome: " << appHome << "\n";
        std::cout << "appTemp: " << appTemp << "\n";
    }


    // auto pVfs = std::make_shared<marty_virtual_fs::VirtualFsImpl>();
    auto pFsImpl = std::make_shared<marty_virtual_fs::FileSystemImpl>();
    auto pVfs    = std::static_pointer_cast<marty_virtual_fs::VirtualFsImpl>(pFsImpl);

    pVfs->createMachineFilesystemMountPoints();

    {
        auto pAppPathsImpl   = std::make_shared<marty_virtual_fs::AppPathsImpl>();
        auto pAppPaths       = std::static_pointer_cast<marty_virtual_fs::IAppPaths>(pAppPathsImpl);

        std::string appHome;
        if (pAppPaths->getAppHomePathEx(appHome, true))
        {
            pVfs->addMountPoint( "~Home", appHome);
        }

        std::string appTemp;
        if (pAppPaths->getAppTempPathEx(appTemp, true))
        {
            pVfs->addMountPoint( "$Temp", appTemp);
        }
    }

    auto pFileSystem = std::static_pointer_cast<marty_virtual_fs::IFileSystem>(pFsImpl);


    auto listDir = [&](const std::string &path)
    {
        // std::vector<DirectoryEntryInfoA> 
        std::cout << "List of '" << path << "'\n";
        auto rootList = pFileSystem->enumerateDirectory(path);
        for(const auto &e : rootList)
        {
            std::cout << "  " << ((e.fileTypeFlags&marty_virtual_fs::FileTypeFlags::directory)==0 ? "  " : "D ") << e.entryName << "\n";
        }

        std::cout << "\n";
    };

    
    listDir("/");
    listDir("/C/Users");
    listDir("/D/Tmp");
    listDir("/~Home");

    return 0;
}

