// dllmain.h : Declaration of module class.

class CComServerExampleModule : public ATL::CAtlDllModuleT< CComServerExampleModule >
{
public :
	DECLARE_LIBID(LIBID_ComServerExampleLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMSERVEREXAMPLE, "{AE0D880D-82FE-43F7-A5B2-4404E694D4A7}")
};

extern class CComServerExampleModule _AtlModule;
