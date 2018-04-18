
#include "MapData.hpp"

bool MapData::has(const std::string &key) const
{
  return map.find(key) != map.end();
}

const std::string& MapData::get(const std::string &key) const
{
  std::map<std::string, std::string>::const_iterator it = map.find(key);
  if(it != map.end())
    return it->second;
  else
    return empty_string;
}

void MapData::set(const std::string &key, const std::string &value)
{
  map.insert(std::pair<std::string, std::string>(key, value));
}

void MapData::load(const std::string &data)
{
  std::istringstream in(data);
  std::string line;

  while(std::getline(in, line)){
    size_t spos = line.find(" ");
    if(spos != std::string::npos) // set key/value
      set(line.substr(0, spos), line.substr(spos));
  }
}

void MapData::write(std::string &data) const
{
  data.clear();
  for(std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); it++)
    data += it->first+" "+it->second+"\r\n";
}

bool MapData::loadFile(const std::string &path)
{
  std::ifstream file(path.c_str());
  if(file){
    std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    load(data);
    return true;
  }
  else
    return false;
}

bool MapData::writeFile(const std::string &path) const
{
  std::ofstream file(path.c_str(), std::ios::trunc);
  if(file){
    std::string data;
    write(data);

    file.write(data.c_str(), data.size());

    return true;
  }
  else
    return false;
}
