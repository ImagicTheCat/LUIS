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


#include "Dir.hpp"

void Dir::explode(const std::string& path, std::vector<std::string>& list, int options, bool first)
{
  DIR* dir = opendir(path.c_str());

  //is dir
  if(dir){
    //add the current folder
    if(options & SDIR)
      list.push_back(path);

    //fetch children
    struct dirent* child;
    while((child = readdir(dir)) != NULL && (options & RECURSIVE || first)){
      std::string name(child->d_name);
      if((name != ".." && name != ".") || (options & SPARENT))
        explode(path+(path[path.size()-1] != '/' ? "/" : "")+name, list, options, false);
    }

    closedir(dir);
  }
  //check if file
  else if(options & SFILE){
#ifdef _WIN32
    struct _stat data;
    int r = _stat(path.c_str(),&data);
    if(r == 0 && data.st_mode & _S_IFREG)
      list.push_back(path);
#else
    struct stat data;
    int r = stat(path.c_str(),&data);
    if(r == 0 && S_ISREG(data.st_mode))
      list.push_back(path);
#endif
  }
}

bool Dir::checkdir(const std::string& path)
{
  DIR* dir = opendir(path.c_str());
  if(dir){
    closedir(dir);
    return true;
  }
  else
    return false;
}

void Dir::make(const std::string& path)
{
#ifdef _WIN32
  _mkdir(path.c_str());
#else
  mkdir(path.c_str(), 0777);
#endif
}

void Dir::rmake(const std::string& path)
{
  std::vector<std::string> steps;
  split(path,"/",steps);

  std::string cpath;
  for(int i = 0; i < steps.size(); i++){
    cpath += steps[i]+"/";
    if(steps[i].size() > 0)
      make(cpath);
  }
}

bool Dir::rmfile(const std::string& path)
{
  return remove(path.c_str()) == 0;
}

bool Dir::rmdir(const std::string& path)
{
#ifdef _WIN32
  return _rmdir(path.c_str()) == 0;
#else
  return ::rmdir(path.c_str()) == 0;
#endif
}
