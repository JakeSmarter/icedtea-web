#ifndef _DLL_H_
#define _DLL_H_

#include <unknwn.h>

/**
 * <summary>The current user's SID. This SID is used by <see
 * cref="LogMessage()"/> for event logging.</summary>
 * <seealso cref="LogMessage()"/>
 */
SID* currentUserSID = NULL;

/**
 * <summary>This module's global event logging source. It names after the
 * <c>PACKAGE_NAME</c> pre-processor macro's resolved value.</summary>
 */
HANDLE eventSource = NULL;

HRESULT STDMETHODCALLTYPE ClassFactory_QueryInterface(IClassFactory*,
                                                      REFIID,
                                                      void**);
ULONG STDMETHODCALLTYPE ClassFactory_AddRef(IClassFactory*);
ULONG STDMETHODCALLTYPE ClassFactory_Release(IClassFactory*);
HRESULT STDMETHODCALLTYPE ClassFactory_CreateInstance(IClassFactory*,
                                                      IUnknown*,
                                                      REFIID,
                                                      void**);
HRESULT STDMETHODCALLTYPE ClassFactory_LockServer(IClassFactory*, BOOL);
#endif /* _DLL_H_ */