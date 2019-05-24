/*

by _sinn3r

To compile (make sure the ilk, obj, and exe are removed):
cl /MT /Z7 /GS- vulnerable_service.c /DFOREGROUND_MODE /DYNAMICBASE:NO /link /FIXED /BASE:0x10470000 && editbin vulnerable_service.exe /NXCOMPAT:NO

Compiled with Visual Studio 2003's cl.exe command in Developer Command Prompt.

To completely shut it down, open a command prompt, and then:
1. sc delete "Buffer Overflow"
2. taskkill /F /IM:vulnerable_service.exe

*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ws2tcpip.h>
#include <Winsock2.h>
#include <stdio.h>
#define DEFAULT_RECV_BUFFER_LEN 1024
#define DEFAULT_FNAME_BUFFER_SIZE 512
#define SERVICE_NAME "Buffer Overflow"
#define SERVICE_DESCRIPTION "Buffer Overflow"
#define PORT 4444

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"advapi32.lib")

LPVOID vulnerableExe = NULL;
DWORD vulnerableExeSize = 0;

SOCKET serverSocket;
struct addrinfo* addrResult;

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl);
VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv);
VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID AcceptConnection();
VOID StartVulnerableServer();
VOID ReceiveClientMessages(SOCKET clientSocket, SOCKADDR_IN clientInfo);
VOID ClientRequestHandler(SOCKET clientSocket, SOCKADDR_IN clientInfo, PCSTR message);
VOID ReplyClient(SOCKET clientSocket);
LPVOID ReadVulnerableFile(char* fname);


char* GetCurrentPath() {
  // Since we don't know exactly how much data we will get from GetModuleFileNameA,
  // we pre-allocate the buffer with the default size of 512 bytes. If we get an
  // ERROR_INSUFFICIENT_BUFFER, we will just readjust and try again
  LPVOID fnameBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, DEFAULT_FNAME_BUFFER_SIZE);
  SIZE_T dwSize = HeapSize(GetProcessHeap(), HEAP_NO_SERIALIZE, fnameBuffer);
  DWORD dwResult = 0;
  unsigned int dwStrLen = 0;

  do {
    dwResult = GetModuleFileNameA(NULL, (LPSTR) fnameBuffer, dwSize);
    dwStrLen = strlen((LPSTR) fnameBuffer);
    if (dwResult == ERROR_INSUFFICIENT_BUFFER) {
      // Damn it, the default buffer size wasn't enough, let's readjust it.
      fnameBuffer = HeapReAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, fnameBuffer, dwSize + DEFAULT_FNAME_BUFFER_SIZE);
      dwSize = HeapSize(GetProcessHeap(), HEAP_NO_SERIALIZE, fnameBuffer);
    }
  } while (dwResult != dwStrLen);

  return (char*) fnameBuffer;
}

SC_HANDLE InstallSelf() {
  SC_HANDLE hScm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (!hScm) {
    printf("- Unable to open a handle for SCManager. Agent will not be installed.\n");
    return NULL;
  }

  char* szPath = GetCurrentPath();
  SC_HANDLE hService = CreateService(
      hScm,                       // hSCManager
      SERVICE_NAME,               // lpServiceName
      SERVICE_NAME,               // lpDisplayName
      SERVICE_ALL_ACCESS,
      SERVICE_WIN32_OWN_PROCESS,  // dwDesiredAccess
      SERVICE_AUTO_START,         // dwServiceType
      SERVICE_ERROR_NORMAL,       // dwErrorControl
      szPath,                     // lpBinaryPathName
      NULL,                       // lpLoadOrderGroup
      NULL,                       // lpdwTagId
      NULL,                       // lpDependencies
      NULL,                       // lpServiceStartName
      NULL                        // lpPassword
    );

  if (hService == NULL) {
    printf("- Failed to create the service.\n");
    return NULL;
  }

  SC_ACTION scAction;
  scAction.Type = SC_ACTION_RESTART;
  scAction.Delay = 60000; // 1 minute

  SERVICE_FAILURE_ACTIONS fActions;
  fActions.dwResetPeriod = INFINITE;
  fActions.lpRebootMsg = "Restart application";
  fActions.lpCommand = NULL;
  fActions.cActions = 1;
  fActions.lpsaActions = &scAction;

  ChangeServiceConfig2(hService, SERVICE_CONFIG_FAILURE_ACTIONS, &fActions);

  HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, szPath);
  CloseServiceHandle(hScm);
  return hService;
}


BOOL IsSelfInstalled() {
  BOOL bStatus = FALSE;

  SC_HANDLE hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
  if (hScm == NULL) {
    // If we fail to OpenSCManager, we have to return TRUE so that we don't try to install
    // the service knowing that it will fail.
    printf("- Unable to open a handle for SCManager\n");
    return TRUE;
  }

  SC_HANDLE hService = OpenService(hScm, SERVICE_NAME, SERVICE_QUERY_STATUS);
  if (hService) {
    // Do "sc delete SERVICE_NAME" if you want to delet the service
    printf("- It looks like the agent has already been installed\n");
    bStatus = TRUE;
    CloseServiceHandle(hService);
  }

  CloseServiceHandle(hScm);

  return bStatus;
}

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv){
  gSvcStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, SvcCtrlHandler);
  if (!gSvcStatusHandle){
    printf("- Service failed to start\n");
    return;
  }

  printf("- Service is running\n");

  gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  gSvcStatus.dwServiceSpecificExitCode = 0;
  ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
  SvcInit(dwArgc, lpszArgv);
}

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl){
  switch (dwCtrl){
    case SERVICE_CONTROL_STOP:
      ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
      SetEvent(ghSvcStopEvent);
      ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
      return;
    case SERVICE_CONTROL_INTERROGATE:
      break;
    default:
      break;
  }
}

void SvcInit(DWORD dwArgc, LPTSTR *lpszArgv){
  ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (ghSvcStopEvent == NULL){
    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
    return;
  }

  ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
  StartVulnerableServer();
}

void ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint){
  static DWORD dwCheckPoint = 1;

  gSvcStatus.dwCurrentState = dwCurrentState;
  gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
  gSvcStatus.dwWaitHint = dwWaitHint;

  if (dwCurrentState == SERVICE_START_PENDING)
    gSvcStatus.dwControlsAccepted = 0;
  else
    gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

  if (dwCurrentState == SERVICE_RUNNING ||
      dwCurrentState == SERVICE_STOPPED)
    gSvcStatus.dwCheckPoint = 0;
  else
    gSvcStatus.dwCheckPoint = dwCheckPoint++;

  SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

BOOL StartSelf(SC_HANDLE installed){
  return StartService(installed, 0, NULL);
}

struct addrinfo* InitWinsock() {
  printf("- WSAStartup\n");
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult !=0 ) {
    return NULL;
  }

  printf("- Translate data to an address structure with getaddrinfo\n");
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  struct addrinfo *addrResult = NULL;
  iResult = getaddrinfo(NULL, "4444", &hints, &addrResult);
  if (iResult != 0) {
    printf("- getaddrinfo returns: %d\n", iResult);
    WSACleanup();
    return NULL;
  }

  return addrResult;
}

SOCKET CreateSocket(struct addrinfo* addrResult) {
  printf("- Creating a socket\n");
  SOCKET s = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
  if (s == INVALID_SOCKET) {
    freeaddrinfo(addrResult);
    WSACleanup();
    return -1;
  }

  printf("- Binding a socket\n");
  int iResult = bind(s, addrResult->ai_addr, (int) addrResult->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    freeaddrinfo(addrResult);
    closesocket(s);
    return -1;
  }

  return s;
}

int Listen() {
  printf("- Listening\n");
  int iResult = listen(serverSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    return -1;
  }

  return 1;
}

VOID ReplyClient(SOCKET clientSocket)  {
  send(clientSocket, vulnerableExe, vulnerableExeSize, MSG_DONTROUTE);
}


/*

This is the vulnerable function. A classic stack-based buffer overflow.

*/
VOID ClientRequestHandler(SOCKET clientSocket, SOCKADDR_IN clientInfo, PCSTR message) {
  PCSTR ip = inet_ntoa(clientInfo.sin_addr);
  char buffer[1024];
  ZeroMemory(buffer, sizeof(buffer));
  strcat(buffer, "- Client says: ");
  strcat(buffer, message);
  strcat(buffer, "\n");
  printf(buffer);
  ReplyClient(clientSocket);
}

VOID ReceiveClientMessages(SOCKET clientSocket, SOCKADDR_IN clientInfo) {
  char recvBuffer[DEFAULT_RECV_BUFFER_LEN];
  unsigned int recvBufferLen = DEFAULT_RECV_BUFFER_LEN;
  unsigned int bytesRead = 0;
  do {
    // recv() will complete when the input ends with \r\d
    bytesRead = recv(clientSocket, recvBuffer, recvBufferLen, MSG_PEEK);
    if (bytesRead > 0) {
      // Remember to add a null byte terminator, otherwise we will read out-of-bound.
      memcpy(recvBuffer+bytesRead, "\x00", 1);
      ClientRequestHandler(clientSocket, clientInfo, (PCSTR) recvBuffer);
      bytesRead = 0;
    }
  } while (bytesRead > 0);
}

VOID AcceptConnection() {
  printf("- Ready to accept a connection\r\n");
  SOCKADDR_IN clientInfo;
  int clientInfoLen = sizeof(clientInfo);
  SOCKET clientSocket = accept(serverSocket, (SOCKADDR*) &clientInfo, &clientInfoLen);
  PCSTR ip = inet_ntoa(clientInfo.sin_addr);
  if (clientSocket == INVALID_SOCKET) {
    return;
  }

  printf("- Received a connection, handling the messages\n");
  ReceiveClientMessages(clientSocket, clientInfo);

  printf("- Shutting down connection\n");
  int iResult = shutdown(clientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    closesocket(clientSocket);
  }
}

LPVOID ReadVulnerableFile(char* fname) {
  HANDLE hFile = CreateFileA(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (!hFile) {
    return NULL;
  }
  DWORD dwFileSize = GetFileSize(hFile, NULL);
  vulnerableExeSize = dwFileSize;
  printf("- %s loaded (%d bytes)\n", fname, dwFileSize);
  LPVOID buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
  DWORD dwBytesRead;
  if (!ReadFile(hFile, buffer, dwFileSize, &dwBytesRead, NULL)) {
    HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, buffer);
    CloseHandle(hFile);
    return NULL;
  }

  CloseHandle(hFile);
  return buffer;
}

VOID StartVulnerableServer() {
  char* exePath = GetCurrentPath();
  vulnerableExe = ReadVulnerableFile(exePath);

  struct addrinfo* addrResult = InitWinsock();
  if (!addrResult) {
    printf("- Failed to init Winsock.\n");
    return;
  }

  serverSocket = CreateSocket(addrResult);
  int iResult = Listen();
  if (!iResult) {
    return;
  }

  while (TRUE) {
    AcceptConnection();
  }

  HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, vulnerableExe);
}


#ifdef FOREGROUND_MODE
int main(int argc, char** argv) {
  printf("- In foreground mode\n");
  StartVulnerableServer();
  return 0;
}
#else
int main(int argc, char** argv[])
{
  if (!IsSelfInstalled()) {
    printf("- Service not registered on Windows. One will be created.\n");

    SC_HANDLE installed = InstallSelf();
    if (!installed){
      printf("- Failed to create service\n");
      return -1;
    }

    BOOL stat = StartSelf(installed);
    if (!stat){
      printf("- Failed to start service\n");
      return -1;
    }

    CloseServiceHandle(installed);
    return 0;
  }

  printf("- Running the process as a service\n");
  SERVICE_TABLE_ENTRY DispatchTable[] = {
    {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) SvcMain },
    {NULL, NULL},
  };

  if (!StartServiceCtrlDispatcher( DispatchTable )){
    return -1;
  }

  return 0;
}
#endif