
#include <iostream>
#include "MainArgs.hpp"
#include "MapData.hpp"
#include "tools.hpp"

#include "submains.hpp"

int main(int argc, char **argv)
{
  if(sodium_init() == -1){
    std::cerr << "error initializing libsodium" << std::endl;
    return 1;
  }

  MainArgs args(argc, argv);

  // cfg path
  if(!args.has("cfg")){
#ifdef _WIN32
    args.set("cfg", ced()+"/luis.conf"); 
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

#ifdef _WIN32
  if(!args.has("cfg-data-dir")) // use data/ near executable under Windows if not defined
    args.set("cfg-data-dir", ced()+"/data"); 
#endif

  if(!args.has("cfg-data-dir")){
    std::cerr << "data-dir not set" << std::endl;
    return 1;
  }

  // exec
  if(args.has("1")){ // command
    const std::string &cmd = args.get("1");
    if(cmd == "identity")
      return submain_identity(args);
    else if(cmd == "key")
      return submain_key(args);
    else if(cmd == "help"){
      std::cout << "LUIS commands:" << std::endl;

      std::cout << "IDENTITY" << std::endl;
      std::cout << "  identity list" << std::endl;
      std::cout << "    list identities" << std::endl;
      std::cout << "  identity create [--name <name>]" << std::endl;
      std::cout << "    create identity, display public key" << std::endl;
      std::cout << "  identity check <public_key>" << std::endl;
      std::cout << "    check if the identity keys are valid" << std::endl;
      std::cout << "  identity chname <public_key> <name>" << std::endl;
      std::cout << "    change identity name" << std::endl;
      std::cout << "  identity chpass <public_key>" << std::endl;
      std::cout << "    change identity password" << std::endl;
      std::cout << "  identity delete <public_key>" << std::endl;
      std::cout << "    delete identity" << std::endl;
      
      std::cout << "KEY" << std::endl;
      std::cout << "  key list" << std::endl;
      std::cout << "    list client/service keys" << std::endl;
      std::cout << "  key set <public_key> [--deny | --allow]" << std::endl;
      std::cout << "    register/deny/update client/service key" << std::endl;
      std::cout << "  key unset <public_key>" << std::endl;
      std::cout << "    delete client/service key" << std::endl;
    }
    else{
      std::cerr << "LUIS command \"" << cmd << "\" not found (try \"luis help\")" << std::endl;
      return 1;
    }
  }
  else // server
    return submain_server(args);

  return 0;
}
