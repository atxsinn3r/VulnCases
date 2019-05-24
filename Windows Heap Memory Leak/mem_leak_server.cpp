#include <WinSock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

/*
0:001> !heap -stat -h

Allocations statistics for
 heap @ 00800000
group-by: TOTSIZE max-display: 20
    size     #blocks     total     ( %) (percent of total busy bytes)
    400 9c7 - 271c00  (97.63) <--- yup
    494 10 - 4940  (0.71)
    1ab4 1 - 1ab4  (0.26)
    1526 1 - 1526  (0.21)
    1000 1 - 1000  (0.16)
    800 2 - 1000  (0.16)
    248 7 - ff8  (0.16)
    d0 b - 8f0  (0.09)
    a8 b - 738  (0.07)
    200 3 - 600  (0.06)
    440 1 - 440  (0.04)
    220 2 - 440  (0.04)
    80 8 - 400  (0.04)
    3bc 1 - 3bc  (0.04)
    35f 1 - 35f  (0.03)
    184 2 - 308  (0.03)
    120 2 - 240  (0.02)
    208 1 - 208  (0.02)
    52 6 - 1ec  (0.02)
    2c 9 - 18c  (0.02)

*/

int main(int args, char** argv) {
  /*
  *  Initialize winsock:
  *  http://msdn.microsoft.com/en-us/library/windows/desktop/ms742213(v=vs.85).aspx
  */
  WSADATA wsaData;
  int wsaErr = WSAStartup(0x101, &wsaData);
  printf("[*] WSAStartup returns: %d\n", wsaErr);
  if (wsaErr != 0) {
    cout << "[x] Could not start WSAStartup. Abort" << endl;
    return -1;
  }

  /*
  *  Initialize the socket
  */
  sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons((u_short) 8080);

  /*
  *  Check and see if we have a valid socket to go on
  */
  SOCKET s;
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == INVALID_SOCKET) {
    cout << "[x] Invalid socket. Abort" << endl;
    return -1;
  }

  /*
  *  Attempt to bind to the socket
  */
  if (bind(s, (sockaddr*) &local, sizeof(local)) != 0) {
    cout << "[x] Unable to bind. Abort" << endl;
    return -1;
  }

  /*
  *  Start listening
  */
  if (listen(s, 10) != 0) {
    cout << "[x] Unable to listen. Abort" << endl;
    return -1;
  }
  cout << "[*] Listening on port 8080..." << endl;

  SOCKET cli;
  sockaddr_in src;
  int srcLen = sizeof(src);

  while (true) {
    cli = accept(s, (struct sockaddr*) &src, &srcLen);
    int r;

    // Keep recving until the client closes the connection
    do {
      char *recvBuf = (char*) malloc(1024);
      memset(recvBuf, 0x00, 1024);
      r = recv(cli, recvBuf, 1023, 0);

      cout << "Client: " << recvBuf << endl;
      cout << "[*] ACK!" << endl;

      // Send the client something
      char *buf = "hello\n";
      send(cli, buf, strlen(buf), 0);
    } while (r > 0);

    /*
    0 = RECV
    1 = SEND
    2 = BOTH
    */
    shutdown(cli, 2);
    closesocket(cli);
  }

  closesocket(s);
  WSACleanup();

  return 0;
}