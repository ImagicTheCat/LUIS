
#include "submains.hpp"

int submain_key(MainArgs &args)
{
  const std::string &cmd = args.get("2");
  
  if(cmd == "set"){
    if(args.has("3")){
      std::string public_key(args.get("3"));
      std::string path(args.get("cfg-data-dir")+"/keys/"+public_key);

      MapData mdata;
      mdata.loadFile(path);

      if(args.has("deny"))
        mdata.set("deny", "");
      else if(args.has("allow"))
        mdata.unset("deny");

      if(!mdata.writeFile(path)){
        std::cerr << "couldn't write client/service to " << path << std::endl;
        return 1;
      }
    }
    else{
      std::cerr << "missing public_key parameter" << std::endl;
      return 1;
    }
  }
  else if(cmd == "unset"){
    if(args.has("3")){
      std::string path(args.get("cfg-data-dir")+"/keys/"+args.get("3"));
      if(!Dir::rmfile(path))
        std::cerr << "couldn't delete " << path << std::endl;
    }
    else{
      std::cerr << "missing public_key parameter" << std::endl;
      return 1;
    }
  }
  else if(cmd == "list"){
    std::string dir(args.get("cfg-data-dir")+"/keys/");
    std::vector<std::string> pfiles;
    Dir::explode(dir, pfiles, Dir::SFILE);

    // header
    std::cout << "public_key                                                       deny" << std::endl;

    for(int i = 0; i < pfiles.size(); i++){
      MapData mdata;
      if(mdata.loadFile(pfiles[i])){
        std::string public_key(pfiles[i].substr(dir.size()));

        std::cout << public_key << " " << (mdata.has("deny") ? "yes" : "no ") << std::endl;
      }
      else
        std::cerr << "couldn't read " << pfiles[i] << std::endl;
    }
  }
  else{
    std::cerr << "key subcommand \"" << cmd << "\" not found" << std::endl;
    return 1;
  }

  return 0;
}
