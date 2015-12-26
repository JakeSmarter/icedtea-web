#ifndef _ICEDTEAIEPLUGIN_H_
#define _ICEDTEAIEPLUGIN_H_

#include "IcedTea-Web.h"

typedef struct privateIIcedTea_WebVtbl {
  struct IIcedTea_WebVtbl vtbl;
  /**
   * Per object reference counter.
   */
  volatile ULONG refCount;
} privateIIcedTea_WebVtbl;

#ifndef __cplusplus
/* Private members */
#define IIcedTea_Web_refCount(This) \
   (*(privateIIcedTea_WebVtbl*)(*This).lpVtbl).refCount
#endif /* __cplusplus */

HRESULT STDMETHODCALLTYPE IcedTea_Web_QueryInterface(
  IUnknown*,
  REFIID,
  void**);
ULONG STDMETHODCALLTYPE IcedTea_Web_AddRef(IUnknown*);
ULONG STDMETHODCALLTYPE IcedTea_Web_Release(IUnknown*);
#endif /* _ICEDTEAIEPLUGIN_H_ */
