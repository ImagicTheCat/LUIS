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

#include "tools.hpp"

void split(const std::string& str, const std::string& separator, std::vector<std::string>& results)
{
  results.clear();
  if(str.size() == 0 || separator.size() == 0)
    results.push_back(str);
  else{
    int shift = 0;
    results.push_back(std::string());

    for(size_t i = 0; i < str.size(); i++){
      const char &c = str[i];
      
      if(c == separator[shift]){
        //searching for separator
        shift++;
        if(shift == separator.size()){
          results.push_back(std::string());
          shift = 0;
        }
      }
      else{
        std::string &current = results[results.size()-1];

        //complete with missing separator chars, reset shift and add char
        if(shift > 0){
          for(int j = 0; j < shift; j++)
            current += separator[j];
          shift = 0;
        }

        current += c;
      }
    }
  } 
}

std::string replace(const std::string& str, const std::string& substr, const std::string& rsubstr)
{
  std::string rstr(str);
  size_t n = 0;
  while((n = rstr.find(substr, n)) != std::string::npos){
    rstr.replace(n, substr.size(), rsubstr);
    n += rsubstr.size();
  }

  return rstr;
}

std::string ced()
{
  static char path[4096];
#ifdef _WIN32
  std::string fpath(path, GetModuleFileName(NULL, path, 4096));
#else
  ssize_t len = readlink("/proc/self/exe", path, 4096);
  std::string fpath(path, (len > 0) ? len : 0);
#endif

  // strip exec file
  return fpath.substr(0,fpath.find_last_of("/\\"));
}

std::string buf2hex(const std::string& data){ return buf2hex(data.c_str(), data.size()); }
std::string buf2hex(const char* data, int size)
{
  std::string ret;
  unsigned char *p = (unsigned char*)data;

  for(int i = 0; i < size; i++){
    unsigned char left = (*p >> 4) & 0x0F;
    unsigned char right = *p & 0x0F;

    if(left > 15);
    else if(left >= 10)
      ret += 'a'+(left-10);
    else if(left >= 0)
      ret += '0'+left;

    if(right > 15);
    else if(right >= 10)
      ret += 'a'+(right-10);
    else if(right >= 0)
      ret += '0'+right;

    p++;
  }

  return ret;
}

std::string hex2buf(const std::string& data){ return hex2buf(data.c_str(), data.size()); }
std::string hex2buf(const char* data, int size)
{
  std::string ret;

  for(int i = 0; i < size; i+=2){
    if(i+1 < size){
      unsigned char byte = (hex2byte(data[i]) << 4)+hex2byte(data[i+1]);
      ret.append((char*)&byte, 1);
    }
  }

  return ret;
}
