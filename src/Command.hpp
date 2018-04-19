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

#ifndef H_COMMAND
#define H_COMMAND

#include <string>

#ifdef __WIN32 

#include <windows.h>
#define popen(a,b) _popen(a,b)
#define pclose(a) _pclose(a)

#endif

/**
 * \brief Execute command (popen wrapper)
 */
class Command{
  public:
    Command(const std::string &cmd, const std::string &mode): f(NULL)
    {
      f = popen(cmd.c_str(), mode.c_str());
    }

    ~Command(){ close(); }

    /**
     * \brief Wait the command to finish and close
     * \param[out] data command output (filled)
     */
    void wait(std::string &data)
    {
      if(f){
        const unsigned int buffer_size = 4096;
        char buffer[buffer_size];
        size_t rsize;
        while(rsize = read((unsigned char*)buffer, buffer_size)){
          data.append(buffer, rsize);
          if(rsize < buffer_size)
            break;
        }

        close();
      }
    }

    /**
     * \brief Close command (blocking)
     */
    void close()
    {
      if(f){
        pclose(f);
        f = NULL;
      }
    }

    /**
     * \brief Read from command output (r mode)
     * \param[out] buffer
     * \param size maximum size to read (buffer size)
     * \return read size
     */
    size_t read(unsigned char *buffer, size_t size)
    {
      if(f)
        return fread(buffer, 1, size, f);
      else
        return 0;
    }

    /**
     * \brief Write to command input (w mode)
     * \param buffer
     * \param size buffer size
     * \return true on success
     */
    bool write(unsigned char *buffer, size_t size)
    {
      if(f)
        return fwrite(buffer, size, 1, f) == 1;
      else
        return false;
    }


    operator bool()const{ return f != NULL; }

    const FILE* getHandle()const{ return f; }

  private:
    FILE *f;
};

#endif
