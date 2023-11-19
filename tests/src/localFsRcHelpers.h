#pragma once

#include "marty_fs_adapters/simple_file_api.h"

#include "umba/umba.h"
#include "umba/cmd_line.h"
#include "umba/filename.h"
#include "umba/program_location.h"

#include <utility>

namespace helpers {


inline
std::vector<std::string> parseWinCommandLine( const std::string &cmdLine )
{
    std::vector<std::string> resVec;
    std::string curArg;

    bool inQuot   = false;
    // bool prevQuot = false;

    // Делаем пока по простому, если надо будет - сделаем по сложному
    // По-простому - это аргумент может быть в кавычках, чтобы пробел входил в аргумент, а не разделял их, больше кавычки не используются
    for(auto ch : cmdLine)
    {
        if (inQuot)
        {
            if (ch=='\"')
            {
                inQuot = false;
            }
            else
            {
                curArg.append(1, ch);
            }
        }
        else
        {
            if (ch=='\"')
            {
                inQuot = true;
            }
            else if (ch==' ')
            {
                if (!curArg.empty())
                {
                    resVec.emplace_back(curArg);
                    curArg.clear();
                }
            }
            else
            {
                curArg.append(1, ch);
            }
        
        }
    } // for

    if (!curArg.empty())
    {
        resVec.emplace_back(curArg);
    }

    return resVec;
}


inline
bool findResource( std::vector<std::uint8_t> &rcData
                 , std::string               &rcFullFileName
                 , std::string                rcName    // only ext allowed started with dot, exe name used
                 , bool                       binaryResource
                 , std::string                rcRootPath      = std::string()
                 , std::vector<std::string>   subPaths  = std::vector<std::string>{ "rc", "res", "." }
            )
{
    std::string exeFullName = umba::program_location::getExeName<std::string>();
    //std::string exePath     = umba::filename::getPath(exeFullName);


    if (rcName.empty() || rcName[0]=='.')
    {
        std::string ext = rcName;
        rcName = umba::filename::getName(exeFullName);

        // if (findFlags&FindNutFlags::stripUnderscoredTail)
        // {
            auto pos = rcName.find('_');
            if (pos!=rcName.npos)
            {
                rcName.erase(pos);
            }
        // }

        rcName = umba::filename::appendExtention(rcName, ext);
    }
    else // not empty and not starts with dot
    {
        // std::string rcPath = umba::filename::getPath(rcName);
    }

    if (rcRootPath.empty())
    {
        rcRootPath = umba::filename::getPath(exeFullName);
    }


    marty_fs_adapters::SimpleFileApi<std::string> fsApi;

    auto readFileText = [&](const std::string &fileName)
    {
        std::string strText = fsApi.readFile( fileName );
        return std::vector<std::uint8_t>((const std::uint8_t*)strText.data(), ((const std::uint8_t*)strText.data())+strText.size() );
    };

    while(true)
    {
        for(auto subPath : subPaths)
        {
            std::string fullPath     = umba::filename::appendPath(rcRootPath, subPath);
            std::string fullFileName = umba::filename::appendPath(fullPath  , rcName);

            std::vector<std::uint8_t> data = binaryResource ? fsApi.readFileBinary(fullFileName) : readFileText(fullFileName);
            
            if (!data.empty())
            {
                std::swap(rcData        , data);
                std::swap(rcFullFileName, fullFileName);
                return true;
            }
        }

        std::string newPath = umba::filename::getPath(rcRootPath);
        if (newPath==rcRootPath)
        {
            break;
        }

        rcRootPath = newPath;

    }

    return false;

}




} // namespace helpers



