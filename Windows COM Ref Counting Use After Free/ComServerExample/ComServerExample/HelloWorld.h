// HelloWorld.h : Declaration of the CHelloWorld

#pragma once
#include "resource.h"       // main symbols



#include "ComServerExample_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CHelloWorld

class ATL_NO_VTABLE CHelloWorld :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHelloWorld, &CLSID_HelloWorld>,
	public IDispatchImpl<IHelloWorld, &IID_IHelloWorld, &LIBID_ComServerExampleLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CHelloWorld()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_HELLOWORLD)


BEGIN_COM_MAP(CHelloWorld)
	COM_INTERFACE_ENTRY(IHelloWorld)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



	STDMETHOD(PrintSomething)(CHAR* val);
};

OBJECT_ENTRY_AUTO(__uuidof(HelloWorld), CHelloWorld)
