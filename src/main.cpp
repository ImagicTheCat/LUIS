
#include <iostream>
#include "network.hpp"

int main(int argc, char **argv)
{
  const unsigned int port = 48151;
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

        while(cs = accept(s, (SOCKADDR*)&csin, &sin_size)){
          std::cout << "accept..." << std::endl;
          if(cs != INVALID_SOCKET){
            // receive request
            int size;
            bool done = false;
            while((size = recv(cs, buffer, buffer_size, 0)) > 0 && !done){
              // TODO read header+data, handle request
              std::cout << std::string(buffer,size);
            }

            std::cout << "end." << std::endl;
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
