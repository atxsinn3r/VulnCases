// HelloWorld.cpp : Implementation of CHelloWorld

#include "stdafx.h"
#include "HelloWorld.h"
#include <iostream>
using namespace std;


// CHelloWorld



STDMETHODIMP CHelloWorld::PrintSomething(CHAR* val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	cout << "This is your string:" << endl;
	cout << val << endl;

	return S_OK;
}
