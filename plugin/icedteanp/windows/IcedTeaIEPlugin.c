#include <windows.h>
#include <ole2.h>

#include "IcedTeaIEPlugin.h"
#include "LogMessage.h"

/******************************************************************************
 * WARNING: When modifying the IIcedTea_Web interface ALWAYS keep in mind to  *
 * keep the interface THREAD-SAFE!                                            *
 ******************************************************************************/

HRESULT STDMETHODCALLTYPE IcedTea_Web_QueryInterface(IUnknown* icedTea_Web,
                                                     REFIID iid,
                                                     void** v) {
  if (icedTea_Web == NULL || iid == NULL || v == NULL) {
    if (v != NULL) *v = NULL;
    return E_INVALIDARG;
  }

  LogMessage(0, TEXT("IcedTea_Web_QueryInterface(%p)\n"), icedTea_Web);
  if (!(IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IOleObject) ||
        IsEqualIID(iid, &IID_IIcedTea_Web))) return E_NOINTERFACE;
  if ((*v = (LPVOID)CoTaskMemAlloc(sizeof (IIcedTea_Web))) == NULL)
    return E_OUTOFMEMORY;
  if (((**(IIcedTea_Web**)v).lpVtbl =
       (IIcedTea_WebVtbl*)CoTaskMemAlloc(sizeof (privateIIcedTea_WebVtbl))) == NULL) {
    CoTaskMemFree(*v);
    return E_OUTOFMEMORY;
  }
  (*(**(IIcedTea_Web**)v).lpVtbl).QueryInterface = IcedTea_Web_QueryInterface;
  (*(**(IIcedTea_Web**)v).lpVtbl).AddRef = IcedTea_Web_AddRef;
  (*(**(IIcedTea_Web**)v).lpVtbl).Release = IcedTea_Web_Release;
  IIcedTea_Web_refCount((IIcedTea_Web*)*v) = 0u;
  IIcedTea_Web_AddRef((IUnknown*)*v);
  return S_OK;
}

ULONG STDMETHODCALLTYPE IcedTea_Web_AddRef(IUnknown* icedTea_Web) {
  /* Stores the thread-current reference counter value. */
  ULONG refCount;
  LogMessage(0, TEXT("IcedTea_Web_AddRef(%p)\n"), icedTea_Web);
  /* Make sure the reference counter does not wrap around */
  if ((refCount = IIcedTea_Web_refCount(icedTea_Web)) != MAXDWORD)
    return InterlockedIncrement(&IIcedTea_Web_refCount(icedTea_Web));
  else {
    LogMessage(ERROR_INVALID_STATE,
               TEXT("IIcedTea_Web interface object reference counter limit hit"));
    /* DO NOT increment the reference counter, just return the thread-current
       reference counter value. */
    return refCount;
  }
}

ULONG STDMETHODCALLTYPE IcedTea_Web_Release(IUnknown* icedTea_Web) {
  /* Stores the thread-current reference counter value. */
  ULONG refCount;

  /* No need to make sure the reference counter does not wrap around because
     this method cannot be called with refCount == 0 */
  /* Dispose the IIcedTea_Web object when reference counter reaches 0 */
  LogMessage(0, TEXT("IcedTea_Web_Release(%p)\n"), icedTea_Web);
  if ((refCount = InterlockedDecrement(&IIcedTea_Web_refCount(icedTea_Web))) == 0) {
    LogMessage(0,
               TEXT("IcedTea_Web_Release(%p): disposing IIcedTea_Web object\n"),
               icedTea_Web);
    CoTaskMemFree((*icedTea_Web).lpVtbl);
    (*icedTea_Web).lpVtbl = NULL;
    CoTaskMemFree(icedTea_Web);
  }
  return refCount;
}
