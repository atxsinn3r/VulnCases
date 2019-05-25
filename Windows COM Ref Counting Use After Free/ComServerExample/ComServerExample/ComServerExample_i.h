

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue Sep 11 17:22:15 2018
 */
/* Compiler settings for ComServerExample.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ComServerExample_i_h__
#define __ComServerExample_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IHelloWorld_FWD_DEFINED__
#define __IHelloWorld_FWD_DEFINED__
typedef interface IHelloWorld IHelloWorld;

#endif 	/* __IHelloWorld_FWD_DEFINED__ */


#ifndef __HelloWorld_FWD_DEFINED__
#define __HelloWorld_FWD_DEFINED__

#ifdef __cplusplus
typedef class HelloWorld HelloWorld;
#else
typedef struct HelloWorld HelloWorld;
#endif /* __cplusplus */

#endif 	/* __HelloWorld_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IHelloWorld_INTERFACE_DEFINED__
#define __IHelloWorld_INTERFACE_DEFINED__

/* interface IHelloWorld */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHelloWorld;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39AF322A-A542-410B-828F-E134B95DD39D")
    IHelloWorld : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PrintSomething( 
            /* [in] */ CHAR *val) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IHelloWorldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHelloWorld * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHelloWorld * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHelloWorld * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHelloWorld * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHelloWorld * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHelloWorld * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHelloWorld * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PrintSomething )( 
            IHelloWorld * This,
            /* [in] */ CHAR *val);
        
        END_INTERFACE
    } IHelloWorldVtbl;

    interface IHelloWorld
    {
        CONST_VTBL struct IHelloWorldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHelloWorld_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHelloWorld_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHelloWorld_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHelloWorld_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHelloWorld_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHelloWorld_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHelloWorld_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IHelloWorld_PrintSomething(This,val)	\
    ( (This)->lpVtbl -> PrintSomething(This,val) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHelloWorld_INTERFACE_DEFINED__ */



#ifndef __ComServerExampleLib_LIBRARY_DEFINED__
#define __ComServerExampleLib_LIBRARY_DEFINED__

/* library ComServerExampleLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_ComServerExampleLib;

EXTERN_C const CLSID CLSID_HelloWorld;

#ifdef __cplusplus

class DECLSPEC_UUID("69217890-0822-49A0-AF26-7F9195CAA59B")
HelloWorld;
#endif
#endif /* __ComServerExampleLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


