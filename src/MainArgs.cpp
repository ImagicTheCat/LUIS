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


#include "MainArgs.hpp"

MainArgs::MainArgs(int argc, char **argv)
{
  std::string buffer;
  std::string key;
  size_t anon_count = 0;

  for(size_t i = 0; i < argc; i++){
    std::string str(argv[i]);

    if(str[0] == '-'){ //param type
      key.clear();

      if(str.size() > 1){
        if(str[1] == '-'){ //word param "--word"
          key = str.substr(2);
          set(key); //predefine key
        }
        else{
          //one or multiple letter params "-abc", define them all
          for(size_t k = 1; k < str.size(); k++){
            std::string sk;
            sk = str[k];
            set(sk);
          }

          if(str.size() == 2) //one letter param, set key
            key = str[1];
        }
      }
    }
    else{ //other arguments as buffers
      if(key.size() == 0){ //no key waiting, is anonymous argument
        std::stringstream ss;
        ss << anon_count++;
        set(ss.str(), str);
      }
      else //key value, add to buffer
        buffer = str;
    }

    //set key from last key/buffer
    if(key.size() > 0 && buffer.size() > 0){
      set(key, buffer);
      key.clear();
      buffer.clear();
    }
  }
}

bool MainArgs::has(const std::string& key) const
{
  return kargs.find(key) != kargs.end();
}

void MainArgs::set(const std::string& key, const std::string& value)
{
  std::map<std::string,std::string>::iterator it = kargs.find(key);
  if(it != kargs.end())
    it->second = value;
  else
    kargs.insert(std::pair<std::string, std::string>(key, value));
}

const std::string& MainArgs::get(const std::string& key) const
{
  std::map<std::string,std::string>::const_iterator it = kargs.find(key);
  if(it != kargs.end())
    return it->second;
  else
    return empty_str;
}
