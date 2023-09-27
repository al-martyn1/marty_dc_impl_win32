#pragma once


#include "umba/umba.h"
#include "umba/cmd_line.h"
#include "umba/filename.h"
#include "umba/program_location.h"

namespace nut_helpers {


enum FindNutFlags
{
    lookDefault             = 0, // no looking
    lookInBaseFolder        = 1,
    lookInSubFolder         = 2,
    lookForCommon           = 4,
    stripUnderscoredTail    = 0x10
};

bool findNut( std::string  &strScript
            , std::string  &fullScriptFileName
            , FindNutFlags findFlags         = (FindNutFlags)(FindNutFlags::lookInSubFolder | FindNutFlags::stripUnderscoredTail | FindNutFlags::lookForCommon)
            , std::string  nutName           = std::string()
            , std::string  path              = std::string()
            , std::string  nutsSubFolderName = std::string()
            )
{
    std::string exeFullName = umba::program_location::getExeName<std::string>();

    if (nutName.empty())
    {
        nutName = umba::filename::getName(exeFullName);

        if (findFlags&FindNutFlags::stripUnderscoredTail)
        {
            auto pos = nutName.find('_');
            if (pos!=nutName.npos)
            {
                nutName.erase(pos);
            }
        }

        nutName = umba::filename::appendExtention(nutName, std::string(".nut"));
    }

    if (path.empty())
    {
        path = umba::filename::getPath(exeFullName);
    }
    

    if (findFlags&FindNutFlags::lookInSubFolder)
    {
        if (nutsSubFolderName.empty())
        {
            nutsSubFolderName = "nuts";
        }
    }

    marty_fs_adapters::SimpleFileApi<std::string> fsApi;

    while(true)
    {
        if (findFlags&FindNutFlags::lookInSubFolder)
        {
            {
                std::string testFileName = umba::filename::appendPath(umba::filename::appendPath(path, nutsSubFolderName), nutName);
                strScript = fsApi.readFile(testFileName);
                if (!strScript.empty())
                {
                    fullScriptFileName = testFileName;
                    return true;
                }
            }

            if (findFlags&FindNutFlags::lookForCommon)
            {
                std::string testFileName = umba::filename::appendPath(umba::filename::appendPath(path, nutsSubFolderName), std::string("common.nut"));
                strScript = fsApi.readFile(testFileName);
                if (!strScript.empty())
                {
                    fullScriptFileName = testFileName;
                    return true;
                }
            }
        
        }

        if (findFlags&FindNutFlags::lookInBaseFolder)
        {
            {
                std::string testFileName = umba::filename::appendPath(path, nutName);
                strScript = fsApi.readFile(testFileName);
                if (!strScript.empty())
                {
                    fullScriptFileName = testFileName;
                    return true;
                }
            }

            if (findFlags&FindNutFlags::lookForCommon)
            {
                std::string testFileName = umba::filename::appendPath(path, std::string("common.nut"));
                strScript = fsApi.readFile(testFileName);
                if (!strScript.empty())
                {
                    fullScriptFileName = testFileName;
                    return true;
                }
            }

        }

        std::string newPath = umba::filename::getPath(path);
        if (newPath==path)
        {
            break;
        }

        path = newPath;
    }

    return false;

}






} // namespace nut_helpers


