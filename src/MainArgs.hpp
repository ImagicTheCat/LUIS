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

#ifndef H_MAINARGS
#define H_MAINARGS

#include <iostream>
#include <string>
#include <sstream>
#include <map>

/*
 * -- command 
 * $ ./cmd arg1 arg2 -x foo --word "bar" arg3 -czf 
 * -- will give
 * 0: ./cmd
 * 1: arg1
 * 2: arg2
 * 3: arg3
 * x: foo
 * word: bar
 * c: 
 * z:
 * f:
 */
class MainArgs{
  public:
    MainArgs(int argc, char **argv);

    bool has(const std::string& key) const;
    const std::string& get(const std::string& key) const;

    const std::map<std::string, std::string>& getKeyValues() const { return kargs; }

    void set(const std::string& key, const std::string& value = "");

  private:
    std::map<std::string, std::string> kargs;
    std::string empty_str;
};

#endif
