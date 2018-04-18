
#ifndef H_MAPDATA
#define H_MAPDATA

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

/*
 * define a simple textual map format, one key per line:
 * <key_without_space> <space> <data> <CR LF>
 *
 * ex:
 * "foo_bar 1 2 3 4 5\r\n"
 * "bar_foo test\r\n"
 * ...
 */

class MapData{
  public:
    bool has(const std::string &key) const;
    const std::string& get(const std::string &key) const;

    // set key/value
    // key: should not contain spaces/CRLF
    // value: should not contain CRLF
    void set(const std::string &key, const std::string &value);

    void clear(){ map.clear(); }

    // load map data from passed string (don't clear before)
    void load(const std::string &data);

    // write map data to passed string
    void write(std::string &data) const;

    // load map data from file (don't clear before)
    // return true on success
    bool loadFile(const std::string &path);

    // write map data to file
    // return true on success
    bool writeFile(const std::string &path) const;

    const std::map<std::string, std::string>& getMap()const{ return map; }

  private:
    std::map<std::string, std::string> map;
    std::string empty_string;
};

#endif
