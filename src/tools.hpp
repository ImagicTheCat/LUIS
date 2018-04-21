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

#ifndef H_TOOLS
#define H_TOOLS

#include <cmath>
#include <ctime>
#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <WinBase.h>
#include <Shellapi.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif

/* WIN32 compat. */
#ifdef _WIN32
inline double round(double x){ return x < 0.0 ? ceil(x - 0.5) : floor(x + 0.5); }
#endif
/* end */

/**
 * \brief Get current executable directory path
 */
std::string ced();

/**
 * \brief Split a string 
 * \param str input string
 * \param separator string used to split
 * \param[out] results (never empty)
 */
void split(const std::string& str, const std::string& separator, std::vector<std::string>& results);

/**
 * \brief Replace substrings by a string
 * \param str input string
 * \param substr substring to replace
 * \param rsubstr replacement
 * \return processed string
 */
std::string replace(const std::string& str, const std::string& substr, const std::string& rsubstr);

/**
 * \brief Bytes to hex-string
 */
std::string buf2hex(const char* data, int size);
std::string buf2hex(const std::string& data);

inline unsigned int hex2byte(char hex)
{
  if(hex >= '0' && hex <= '9')
    return hex-'0';
  if(hex >= 'A' && hex <= 'F')
    return hex-'A'+10;
  if(hex >= 'a' && hex <= 'f')
    return hex-'a'+10;

  return 0;
}

/**
 * \brief hex-string to bytes
 */
std::string hex2buf(const char* data, int size);
std::string hex2buf(const std::string& data);

/**
 * \brief Sanitize string
 * \param str string to sanitize
 * \param allowed allowed characters
 * \return string with only allowed characters
 */
std::string sanitize(const std::string& str, const std::string& allowed);

/**
 * \brief Sanitize check string
 * \param str string to check
 * \param allowed allowed characters
 * \return true if containing only allowed characters
 */
bool sanitizeCheck(const std::string& str, const std::string& allowed);



#endif
