
#include "submains.hpp"

int submain_identity(MainArgs &args)
{
  const std::string &cmd = args.get("2");
  
  if(cmd == "create"){ // generate new identity
    unsigned char seed[32], public_key[32], private_key[64], sign[64];

    if(ed25519_create_seed(seed) == 0){
      // create keypair
      ed25519_create_keypair(public_key, private_key, seed);

      // convert to hex
      std::string hpublic_key(buf2hex((const char*)public_key, 32));
      std::string hprivate_key(buf2hex((const char*)private_key, 64));

      // set data
      MapData midentity;
      if(args.has("name"))
        midentity.set("name", args.get("name"));
      midentity.set("private_key", hprivate_key);

      // write data
      std::string path(args.get("cfg-data-dir")+"/identities/"+hpublic_key);
      if(midentity.writeFile(path))
        std::cout << hpublic_key << std::endl;
      else{
        std::cerr << "couldn't write identity to " << path << std::endl;
        return 1;
      }
    }
    else{
      std::cerr << "couldn't create seed" << std::endl;
      return 1;
    }
  }
  else if(cmd == "check"){
    if(args.has("3")){
      const std::string &hpublic_key = args.get("3");
      std::string path(args.get("cfg-data-dir")+"/identities/"+hpublic_key);

      MapData mdata;
      if(mdata.loadFile(path)){
        if(mdata.has("passphrase"))
          std::cout << "private_key is encrypted" << std::endl;
        else{
          // sign and verify
          bool ok = false;

          std::string private_key(hex2buf(mdata.get("private_key")));
          std::string public_key(hex2buf(hpublic_key));
          const std::string message("check");
          unsigned char sign[64];

          if(private_key.size() == 64 && public_key.size() == 32){
            ed25519_sign(sign, (const unsigned char*)message.c_str(), message.size(), (const unsigned char*)public_key.c_str(), (const unsigned char*)private_key.c_str());
            ok = (ed25519_verify(sign, (const unsigned char*)message.c_str(), message.size(), (const unsigned char*)public_key.c_str()) == 1);
          }

          std::cout << (ok ? "valid" : "invalid") << std::endl;
        }
      }
      else{
        std::cerr << "couldn't read " << path << std::endl;
        return 1;
      }
    }
    else{
      std::cerr << "missing public_key parameter" << std::endl;
      return 1;
    }
  }
  else if(cmd == "chname"){
    if(args.has("3")){
      const std::string &hpublic_key = args.get("3");
      std::string path(args.get("cfg-data-dir")+"/identities/"+hpublic_key);

      MapData mdata;
      if(mdata.loadFile(path)){
        mdata.set("name", args.get("4"));
        mdata.writeFile(path);
      }
      else{
        std::cerr << "couldn't read " << path << std::endl;
        return 1;
      }
    }
    else{
      std::cerr << "missing public_key parameter" << std::endl;
      return 1;
    }
  }
  else if(cmd == "chpass"){
    if(args.has("3")){
      const std::string &hpublic_key = args.get("3");
      std::string path(args.get("cfg-data-dir")+"/identities/"+hpublic_key);

      MapData mdata;
      if(mdata.loadFile(path)){
        // ...
      }
      else{
        std::cerr << "couldn't read " << path << std::endl;
        return 1;
      }
    }
    else{
      std::cerr << "missing public_key parameter" << std::endl;
      return 1;
    }
  }
  else if(cmd == "delete"){
    if(args.has("3")){
      std::string path(args.get("cfg-data-dir")+"/identities/"+args.get("3"));
      if(!Dir::rmfile(path))
        std::cerr << "couldn't delete " << path << std::endl;
    }
    else{
      std::cerr << "missing public_key parameter" << std::endl;
      return 1;
    }
  }
  else if(cmd == "list"){
    std::string dir(args.get("cfg-data-dir")+"/identities/");
    std::vector<std::string> pfiles;
    Dir::explode(dir, pfiles, Dir::SFILE);

    // header
    std::cout << "public_key                                                       pass name" << std::endl;

    for(int i = 0; i < pfiles.size(); i++){
      MapData mdata;
      if(mdata.loadFile(pfiles[i])){
        std::string public_key(pfiles[i].substr(dir.size()));

        std::cout << public_key << " " << (mdata.has("passphrase") ? "yes" : "no ") << "  " << mdata.get("name") << std::endl;
      }
      else
        std::cerr << "couldn't read " << pfiles[i] << std::endl;
    }
  }
  else{
    std::cerr << "identity subcommand \"" << cmd << "\" not found" << std::endl;
    return 1;
  }

  return 0;
}
