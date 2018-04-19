// ripped from https://github.com/Archzenvald/Shido

/*  
 * Copyright (C) 2016 Archzenvald <archzenvald[]gmail com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution. 
 */

#ifndef H_DIR
#define H_DIR

#include <iostream>
#include <string>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <cstdio>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <shlwapi.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "tools.hpp"

/**
 * \brief Directory tools (experimental)
 */
class Dir{
  public:
    enum{
      SFILE = 1,
      SDIR = 2,
      RECURSIVE = 4,
      SPARENT = 8
    };

    /**
     * \brief List directories/files from a path
     * \param path directory path
     * \param[out] list paths
     * \param options listing options
     * \param first (internal usage)
     * \return paths like "mypath/dir/file"
     *
     * can include (S)DIR, (S)FILE, (S)PARENT (.. and .) and do RECURSIVE
     * do not use RECURSIVE and (S)PARENT, cause infinite research
     */
    static void explode(const std::string& path, std::vector<std::string>& list, int options = SDIR | SFILE, bool first = true);

    /**
     * \brief Make directory 
     * \param path
     */
    static void make(const std::string& path);

    /**
     * \brief Make directory (recursive) (only support UNIX path notation)
     * \param path
     */
    static void rmake(const std::string& path);

    /**
     * \brief Remove a file
     * \return true if removed
     */
    static bool rmfile(const std::string& path);

    /**
     * \brief Remove a directory (if empty)
     * \return true if removed
     */
    static bool rmdir(const std::string& path);

    /**
     * \brief Check if a directory exists
     */
    static bool checkdir(const std::string& path);

    /**
     * \brief Check if a path is relative (in function of the platform)
     * \return true if relative
     */
    static bool isRelative(const std::string& path)
    {
#ifdef _WIN32
        return PathIsRelative(path.c_str());
#else
        return (path.size() == 0 || path[0] != '/');
#endif
    }
    
  private:
    Dir(); 
};

#endif
