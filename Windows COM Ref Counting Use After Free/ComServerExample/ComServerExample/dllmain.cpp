// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ComServerExample_i.h"
#include "dllmain.h"

CComServerExampleModule _AtlModule;

class CComServerExampleApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CComServerExampleApp, CWinApp)
END_MESSAGE_MAP()

CComServerExampleApp theApp;

BOOL CComServerExampleApp::InitInstance()
{
	return CWinApp::InitInstance();
}

int CComServerExampleApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
