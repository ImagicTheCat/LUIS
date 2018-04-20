
#include "submains.hpp"

void request_contract(MainArgs &args, const std::string &body, std::string &response)
{
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
              response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
              response += response_body;
            }
            else
              response = "HTTP/1.1 403 Forbidden\r\n\r\n";

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
