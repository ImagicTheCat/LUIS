
#include "submains.hpp"

void request_contract(MainArgs &args, const std::string &body, std::string &response)
{
  static const std::string ahex = "0123456789abcdef";
  static const std::string aname = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_ ";
  static const std::string atitle = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_ ";

  Contract contract;
  contract.load(body);

  if(contract.verify(false)){
    if(contract.getDataSteps().size() > 0){
      // service data
      const MapData &service_data = contract.getDataSteps()[0];
      std::string service_key = sanitize(service_data.get("public_key"), ahex);
      std::string service_path(args.get("cfg-data-dir")+"/keys/"+service_key);
      MapData service_fdata;
      bool service_registered = service_fdata.loadFile(service_path);

      // client data
      const MapData &client_data = contract.getDataSteps()[contract.getDataSteps().size()-1];
      std::string client_key = sanitize(client_data.get("public_key"), ahex);
      std::string client_path(args.get("cfg-data-dir")+"/keys/"+client_key);
      MapData client_fdata;
      bool client_registered = client_fdata.loadFile(client_path);

      // check client/service
      if(!service_fdata.has("deny") && !client_fdata.has("deny")){
        // prepare command

        // create identity list
        std::vector<std::pair<std::string, MapData> > identities; // key/name

        if(service_data.has("identity")) // pre-selected identity
          identities.push_back(std::pair<std::string, MapData>(service_data.get("identity"), MapData()));
        else{ // append all identities
          std::vector<std::string> idfiles;
          std::string dir(args.get("cfg-data-dir")+"/identities/");
          Dir::explode(dir, idfiles, Dir::SFILE);
          for(size_t i = 0; i < idfiles.size(); i++){
            std::string public_key(idfiles[i].substr(dir.size()));
            identities.push_back(std::pair<std::string, MapData>(public_key, MapData()));
          }
        }

        // check identities
        for(int i = identities.size()-1; i >= 0; i--){
          std::pair<std::string, MapData> &identity = identities[i];

          identity.first = sanitize(identity.first, ahex);
          std::string identity_path(args.get("cfg-data-dir")+"/identities/"+identity.first);

          if(!identity.second.loadFile(identity_path)) // erase entry if not found
            identities.erase(identities.begin()+i);
        }

        std::string command_data;

        // write identities
        std::stringstream ss;
        ss << identities.size();
        command_data += ss.str()+"\r\n";

        for(size_t i = 0; i < identities.size(); i++){
          std::pair<std::string, MapData> &identity = identities[i];
          command_data += identity.first+" "+(identity.second.has("passphrase") ? "pass" : "nopass")+" "+identity.second.get("name")+"\r\n";
        }

        // write service and client
        command_data += service_key+(service_registered ? " registered ":" unregistered ")+sanitize(service_data.get("name"), aname)+"\r\n";
        command_data += client_key+(client_registered ? " registered ":" unregistered ")+sanitize(client_data.get("name"), aname)+"\r\n";

        // write title
        command_data += sanitize(service_data.get("title"), atitle)+"\r\n";

        // write contract
        std::string contract_str;
        contract.write(contract_str);
        command_data += contract_str;

        // exec contract command
        std::string cmd_out;
        Command cmd(args.get("cfg-cmd-contract")+" "+buf2hex(command_data), "r");
        if(cmd)
          cmd.wait(cmd_out); // should return "<id_identity> <pass>" or "blacklist service|client|both"
        else
          std::cerr << "fail to call command \"" << args.get("cfg-cmd-contract") << "\"" << std::endl;

        size_t sep = cmd_out.find(" ");
        if(sep != std::string::npos){
          std::string out1(cmd_out.substr(0, sep)), out2(cmd_out.substr(sep+1));
          if(out1 == "blacklist"){ // blacklist
          }
          else{ // sign contract
            std::stringstream ss;
            ss.str(out1);
            size_t id = 0;
            ss >> id;

            if(id < identities.size()){
              std::pair<std::string, MapData> &identity = identities[id];

              // load identity public_key/private_key
              std::string private_key(hex2buf(identity.second.get("private_key")));
              std::string public_key(hex2buf(identity.first));

              bool unlocked = true;

              if(identity.second.has("passphrase")){
                unlocked = false;

                // decrypt private key
                std::string salt(hex2buf(identity.second.get("pass_salt")));
                std::string nonce(hex2buf(identity.second.get("pass_nonce")));
                unsigned long long int ops_limit = crypto_pwhash_OPSLIMIT_SENSITIVE;
                size_t mem_limit = crypto_pwhash_MEMLIMIT_SENSITIVE;

                if(identity.second.has("pass_ops_limit")){
                  std::stringstream ss(identity.second.get("pass_ops_limit"));
                  ss >> ops_limit;
                }

                if(identity.second.has("pass_mem_limit")){
                  std::stringstream ss(identity.second.get("pass_mem_limit"));
                  ss >> mem_limit;
                }

                if(salt.size() == crypto_pwhash_SALTBYTES && nonce.size() == crypto_secretbox_NONCEBYTES
                    && private_key.size() == 64+crypto_secretbox_MACBYTES){

                  // generate key from password
                  unsigned char key[crypto_secretbox_KEYBYTES];
                  if(crypto_pwhash(key, crypto_secretbox_KEYBYTES, out2.c_str(), out2.size(), (const unsigned char*)salt.c_str(), ops_limit, mem_limit, crypto_pwhash_ALG_DEFAULT) == 0){
                    // decrypt private key
                    unsigned char _private_key[64];
                    if(crypto_secretbox_open_easy(_private_key, (const unsigned char*)private_key.c_str(), private_key.size(), (const unsigned char*)nonce.c_str(), key) == 0){
                      // decrypted
                      private_key = std::string((const char*)_private_key, 64);
                      unlocked = true;
                    }
                  }
                }
              }

              if(unlocked){
                MapData &next_step = contract.next();

                if(!service_data.has("identity"))
                  next_step.set("identity", identity.first);

                // sign contract
                if(contract.sign(public_key, private_key)){
                  // verify contract
                  if(contract.verify(true)){
                    // send back contract
                    contract.write(response);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

int submain_server(MainArgs &args)
{
  const unsigned int port = 25519;
  const unsigned int max_connections = 10;
  const unsigned int buffer_size = 4096;
  char buffer[buffer_size];

  // init networking
#ifdef _WIN32
  WSADATA wsa;
  int err = WSAStartup(MAKEWORD(2, 2), &wsa);
  if(err < 0){
    std::cerr << "networking error" << std::endl;
    return 1;
  }
#endif

  // create socket
  SOCKET s = socket(PF_INET, SOCK_STREAM, 0);
  if(s != INVALID_SOCKET){
    // set re-use addr
    int _enable = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&_enable, sizeof(int)) < 0)
      std::cerr << "error while setting socket option" << std::endl;

    // bind on all interfaces

    SOCKADDR_IN sin = {0};
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;

    if(bind(s, (SOCKADDR*)&sin, sizeof(SOCKADDR)) != SOCKET_ERROR){
      // listen connections
      if(listen(s, max_connections) != SOCKET_ERROR){
        // new connection (request)
        SOCKADDR_IN csin = {0};
        SOCKET cs;
        socklen_t sin_size = sizeof(SOCKADDR_IN);

        std::cout << "server listening on port " << port << std::endl;

        while(cs = accept(s, (SOCKADDR*)&csin, &sin_size)){
          if(cs != INVALID_SOCKET){
            // receive/parse request
            int size;
            bool done = false;
            std::string req, header, body;
            std::string method, path;
            long int content_length = -1;
            std::string response_body;
 
            // read/parse from socket
            while(body.size() != content_length && (size = recv(cs, buffer, buffer_size, 0)) > 0){
              if(header.size() == 0){ // find header
                req.append(buffer, size); // fill request

                size_t hpos = req.find("\r\n\r\n");
                if(hpos != std::string::npos){
                  header = req.substr(0,hpos);
                  body = req.substr(hpos+4);
                  req.clear();

                  // parse header
                  std::vector<std::string> lines;
                  split(header, "\r\n", lines);

                  content_length = 0; // init content_length

                  for(size_t i = 0; i < lines.size(); i++){
                    if(i == 0){ // first line
                      std::vector<std::string> args;
                      split(lines[i], " ", args);

                      if(args.size() >= 2){
                        method = args[0];
                        path = args[1];
                      }
                    }
                    else{ // header key/value
                      std::vector<std::string> args;
                      split(lines[i], ": ", args);

                      if(args.size() >= 2 && args[0] == "Content-Length"){
                        std::stringstream ss(args[1]);
                        ss >> content_length;
                      }
                    }
                  }
                }
              }
              else // fill body
                body.append(buffer, size);
            }

            // execute correct path function (empty response body will return an HTTP error)
            if(method == "POST" && body.size() == content_length){
              if(path == "/contract")
                request_contract(args, body, response_body);
            }

            // build response
            std::string response;
            if(response_body.size() > 0){
              response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\nCache-Control: no-cache\r\n\r\n";
              response += response_body;
            }
            else
              response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\nCache-Control: no-cache\r\n\r\n";

            // send response
            send(cs, response.c_str(), response.size(), 0);

            // close socket
            shutdown(cs, SD_BOTH); 
            closesocket(cs);
          }
        }
      }
      else
        std::cerr << "socket listen error" << std::endl;
    }
    else
      std::cerr << "socket bind error" << std::endl;
  }
  else
    std::cerr << "socket creation error" << std::endl;

  // shutdown networking
#ifdef _WIN32
  WSACleanup();
#endif

  return 0;
}
