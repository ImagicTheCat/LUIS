
#include <iostream>
#include "MainArgs.hpp"
#include "MapData.hpp"

#include "submains.hpp"

int main(int argc, char **argv)
{
  MainArgs args(argc, argv);

  // cfg path
  if(!args.has("cfg")){
#ifdef _WIN32
    // path relative to executable
    char path[4096];
    std::string fpath(path, GetModuleFileName(NULL, path, 4096));
    std::string epath = fpath.substr(0,fpath.find_last_of("/\\"));

    args.set("cfg", epath+"/luis.conf"); 
    args.set("cfg-data-dir", epath+"/data"); 
#else
    args.set("cfg", "/etc/luis/luis.conf"); // GNU/Linux path
#endif
  }

  // load cfg file / populate cfg args with config file (if not defined)
  MapData mconf;
  if(mconf.loadFile(args.get("cfg"))){
    for(std::map<std::string, std::string>::const_iterator it = mconf.getMap().begin(); it != mconf.getMap().end(); it++){
      std::string key = "cfg-"+it->first;
      if(!args.has(key))
        args.set(key, it->second);
    }
  }
  else{
    std::cerr << "can't load config file \"" << args.get("cfg") << "\"" << std::endl;
    return 1;
  }

  // exec
  if(args.has("1")){ // command
    const std::string &cmd = args.get("1");
    if(cmd == "list")
      return submain_list(args);
    else{
      std::cerr << "LUIS command \"" << cmd << "\" not found" << std::endl;
      return 1;
    }
  }
  else // server
    return submain_server(args);

  return 0;
}
