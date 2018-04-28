
#include "submains.hpp"

std::string ask_pass(MainArgs &args)
{
  std::string output;
  std::string cmd(args.get("cfg-cmd-password-term"));
  Command command(cmd, "r");
  if(command)
    command.wait(output);
  else
    std::cerr << "fail to call command \"" << cmd << "\"" << std::endl;

  return output;
}

int submain_identity(MainArgs &args)
{
  const std::string &cmd = args.get("2");
  
  if(cmd == "create"){ // generate new identity
    unsigned char public_key[32], private_key[64], sign[64];

    // create keypair
    if(crypto_sign_keypair(public_key, private_key) == 0){
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
        std::string public_key(hex2buf(hpublic_key));
        std::string private_key(hex2buf(mdata.get("private_key")));

        bool unlocked = true;

        if(mdata.has("passphrase")){
          unlocked = false;

          // decrypt private key
          std::string salt(hex2buf(mdata.get("pass_salt")));
          std::string nonce(hex2buf(mdata.get("pass_nonce")));
          unsigned long long int ops_limit = crypto_pwhash_OPSLIMIT_SENSITIVE;
          size_t mem_limit = crypto_pwhash_MEMLIMIT_SENSITIVE;

          if(mdata.has("pass_ops_limit")){
            std::stringstream ss(mdata.get("pass_ops_limit"));
            ss >> ops_limit;
          }

          if(mdata.has("pass_mem_limit")){
            std::stringstream ss(mdata.get("pass_mem_limit"));
            ss >> mem_limit;
          }

          if(salt.size() == crypto_pwhash_SALTBYTES && nonce.size() == crypto_secretbox_NONCEBYTES
              && private_key.size() == 64+crypto_secretbox_MACBYTES){
            std::cout << "Password: " << std::flush;
            std::string old_password(ask_pass(args));
            std::cout << std::endl;

            // generate key from password
            unsigned char key[crypto_secretbox_KEYBYTES];
            if(crypto_pwhash(key, crypto_secretbox_KEYBYTES, old_password.c_str(), old_password.size(), (const unsigned char*)salt.c_str(), ops_limit, mem_limit, crypto_pwhash_ALG_DEFAULT) == 0){
              // decrypt private key
              unsigned char _private_key[64];
              if(crypto_secretbox_open_easy(_private_key, (const unsigned char*)private_key.c_str(), private_key.size(), (const unsigned char*)nonce.c_str(), key) == 0){
                // decrypted
                private_key = std::string((const char*)_private_key, 64);
                unlocked = true;
              }
            }
            else{
              std::cerr << "couldn't generate password key" << std::endl;
              return 1;
            }
          }
        }

        if(unlocked){
          // sign and verify
          bool ok = false;

          const std::string message("check");
          unsigned char sign[64];

          if(private_key.size() == 64 && public_key.size() == 32){
            crypto_sign_detached(sign, NULL, (const unsigned char*)message.c_str(), message.size(), (const unsigned char*)private_key.c_str());
            ok = (crypto_sign_verify_detached(sign, (const unsigned char*)message.c_str(), message.size(), (const unsigned char*)public_key.c_str()) == 0);
          }

          std::cout << (ok ? "valid" : "invalid") << std::endl;
        }
        else{
          std::cerr << "couldn't unlock private_key" << std::endl;
          return 1;
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
        if(!mdata.writeFile(path))
          std::cerr << "couldn't write identity to " << path << std::endl;
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
      if(mdata.loadFile(path)){ // load identity
        // load private key (maybe encrypted)
        std::string private_key(hex2buf(mdata.get("private_key")));

        bool unlocked = true;

        if(mdata.has("passphrase")){
          unlocked = false;

          // decrypt private key
          std::string salt(hex2buf(mdata.get("pass_salt")));
          std::string nonce(hex2buf(mdata.get("pass_nonce")));
          unsigned long long int ops_limit = crypto_pwhash_OPSLIMIT_SENSITIVE;
          size_t mem_limit = crypto_pwhash_MEMLIMIT_SENSITIVE;

          if(mdata.has("pass_ops_limit")){
            std::stringstream ss(mdata.get("pass_ops_limit"));
            ss >> ops_limit;
          }

          if(mdata.has("pass_mem_limit")){
            std::stringstream ss(mdata.get("pass_mem_limit"));
            ss >> mem_limit;
          }

          if(salt.size() == crypto_pwhash_SALTBYTES && nonce.size() == crypto_secretbox_NONCEBYTES
              && private_key.size() == 64+crypto_secretbox_MACBYTES){
            std::cout << "Old password: " << std::flush;
            std::string old_password(ask_pass(args));
            std::cout << std::endl;

            // generate key from password
            unsigned char key[crypto_secretbox_KEYBYTES];
            if(crypto_pwhash(key, crypto_secretbox_KEYBYTES, old_password.c_str(), old_password.size(), (const unsigned char*)salt.c_str(), ops_limit, mem_limit, crypto_pwhash_ALG_DEFAULT) == 0){
              // decrypt private key
              unsigned char _private_key[64];
              if(crypto_secretbox_open_easy(_private_key, (const unsigned char*)private_key.c_str(), private_key.size(), (const unsigned char*)nonce.c_str(), key) == 0){
                // decrypted
                private_key = std::string((const char*)_private_key, 64);
                unlocked = true;
              }
            }
            else{
              std::cerr << "couldn't generate password key" << std::endl;
              return 1;
            }
          }
        }

        if(unlocked){
          std::cout << "New password (empty to remove): " << std::flush;
          std::string new_password(ask_pass(args));
          std::cout << std::endl;
          std::cout << "Confirm password: " << std::flush;
          std::string confirm_password(ask_pass(args));
          std::cout << std::endl;

          if(new_password == confirm_password){
            if(new_password.size() > 0){
              // generate salt
              unsigned char salt[crypto_pwhash_SALTBYTES];
              randombytes_buf(salt, crypto_pwhash_SALTBYTES);

              // generate key from password
              unsigned char key[crypto_secretbox_KEYBYTES];

              unsigned long long int ops_limit = crypto_pwhash_OPSLIMIT_SENSITIVE;
              size_t mem_limit = crypto_pwhash_MEMLIMIT_SENSITIVE;

              if(crypto_pwhash(key, crypto_secretbox_KEYBYTES, new_password.c_str(), new_password.size(), salt, ops_limit, mem_limit, crypto_pwhash_ALG_DEFAULT) == 0){
                // generate nonce
                unsigned char nonce[crypto_secretbox_NONCEBYTES];
                randombytes_buf(nonce, crypto_secretbox_NONCEBYTES);

                unsigned char _private_key[64+crypto_secretbox_MACBYTES];
                if(crypto_secretbox_easy(_private_key, (const unsigned char*)private_key.c_str(), 64, nonce, key) == 0){
                  // write salt, nonce, private_key
                  mdata.set("passphrase", "");
                  mdata.set("pass_salt", buf2hex(std::string((const char*)salt, crypto_pwhash_SALTBYTES)));
                  mdata.set("pass_nonce", buf2hex(std::string((const char*)nonce, crypto_secretbox_NONCEBYTES)));
                  mdata.set("private_key", buf2hex(std::string((const char*)_private_key, 64+crypto_secretbox_MACBYTES)));
                  std::stringstream ss;
                  ss << ops_limit;
                  mdata.set("pass_ops_limit", ss.str());
                  ss.str(std::string());
                  ss.clear();
                  ss << mem_limit;
                  mdata.set("pass_mem_limit", ss.str());

                  if(mdata.writeFile(path)){
                    std::cout << "private_key is now encrypted" << std::endl;
                  }
                  else{
                    std::cerr << "couldn't write identity to " << path << std::endl;
                    return 1;
                  }
                }
                else{
                  std::cerr << "couldn't encrypt private_key" << std::endl;
                  return 1;
                }
              }
              else{
                std::cerr << "couldn't generate password key" << std::endl;
                return 1;
              }
            }
            else{
              // remove pass
              mdata.unset("passphrase");
              mdata.unset("pass_salt");
              mdata.unset("pass_nonce");
              mdata.set("private_key", buf2hex(private_key));

              if(mdata.writeFile(path)){
                std::cout << "private_key is now unencrypted" << std::endl;
              }
              else{
                std::cerr << "couldn't write identity to " << path << std::endl;
                return 1;
              }
            }
          }
          else{
            std::cerr << "passwords don't match" << std::endl;
            return 1;
          }
        }
        else{
          std::cerr << "couldn't unlock private_key" << std::endl;
          return 1;
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
