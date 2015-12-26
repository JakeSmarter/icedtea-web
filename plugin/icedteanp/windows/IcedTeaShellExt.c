/**
 * <summary>
 * Implements Windows Explorer shell extensions like JNLP file application
 * icons, preview handlers, and property sheet handlers.
 * </summary>
 */

#include <ntdef.h>
#include <objidl.h>
#include <shlobj.h>
#include "IcedTea-Web.h"
#include "IcedTeaShellExt.h"
#include "LogMessage.h"

// HRESULT STDMETHODCALLTYPE ShellExtInit_QueryInterface(
  // IShellExtInit* This, REFIID iid, void** object) {
  // if (This == NULL || iid == NULL) {
    // if (object) *object == NULL;
    // return E_INVALIDARG;
  // }
  // if (object == NULL) return E_POINTER;

  // {
    // LPOLESTR iidString = NULL;
    // HRESULT hr;
    // if ((hr = StringFromIID(iid, &iidString)) != S_OK) {
      // *object = NULL;
      // LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hr),
                            // HRESULT_FACILITY(hr),
                            // HRESULT_CODE(hr)),
                 // TEXT("%S(): Could not make string from IID"), __FUNCTION__);
      // return hr;
    // }
    // LogMessage(0u, TEXT("%S(0x%p, 0x%p == \"%s\", 0x%p)"), __FUNCTION__, This, iid, iidString, object);
    // CoTaskMemFree(iidString);
  // }
  // if (IsEqualIID(iid, &IID_IUnknown) ||
      // IsEqualIID(iid, &IID_IShellExtInit)) {
    // (*(*This).lpVtbl).AddRef((IShellExtInit*)(*object = This));
    // return S_OK;
  // }

  // *object = NULL;
  // return E_NOINTERFACE;
// }

// ULONG STDMETHODCALLTYPE ShellExtInit_AddRef(IShellExtInit* This) {
  // LogMessage(0u, TEXT("%S(0x%p): refCount == %u\n"), __FUNCTION__, This, (*(IcedTea_WebShellExtInit*)This).refCount);

  // return This ?
            // (*(IcedTea_WebShellExtInit*)This).refCount == MAXULONG ?
              // MAXULONG :
              // InterlockedIncrement(&(*(IcedTea_WebShellExtInit*)This).refCount) :
            // 0u;
// }

// ULONG STDMETHODCALLTYPE ShellExtInit_Release(IShellExtInit* This) {
  // if (This == NULL) return 0u;

  // LogMessage(0u, TEXT("%S(0x%p): refCount == %u\n"), __FUNCTION__, This, (*(IcedTea_WebShellExtInit*)This).refCount);

  // if ((*(IcedTea_WebShellExtInit*)This).refCount) {
    // ULONG refCountAfterDecrement;
    // if (refCountAfterDecrement = InterlockedDecrement(&(*(IcedTea_WebShellExtInit*)This).refCount))
      // return refCountAfterDecrement;
  // } else return 0u;
  // if ((*(IcedTea_WebShellExtInit*)This).dataObject)
    // (*(*(*(IcedTea_WebShellExtInit*)This).dataObject).lpVtbl).Release((*(IcedTea_WebShellExtInit*)This).dataObject);
  // CoTaskMemFree((*This).lpVtbl);
  // (*This).lpVtbl = NULL;
  // CoTaskMemFree((IcedTea_WebShellExtInit*)This);
  // return 0u;
// }

// HRESULT STDMETHODCALLTYPE ShellExtInit_Initialize(IShellExtInit* This,
                                                   // PCIDLIST_ABSOLUTE itemIdList,
                                                   // IDataObject* dataObject,
                                                   // HKEY key) {
  // if (dataObject == NULL || key == NULL) return E_INVALIDARG;

  // LogMessage(0u, TEXT("%S(0x%p, 0x%p , 0x%p, 0x%08X)"), __FUNCTION__, This, itemIdList, dataObject, key);

  // (*(IcedTea_WebShellExtInit*)This).dataObject = dataObject;
  // (*(*dataObject).lpVtbl).AddRef(dataObject);
  // FORMATETC formatetc = {
// #ifdef UNICODE
    // CF_UNICODETEXT,
// #else
    // CF_TEXT,
// #endif /* UNICODE */
    // NULL,
    // DVASPECT_CONTENT,
    // -1,
    // TYMED_HGLOBAL
  // };
  // STGMEDIUM medium;
  // ZeroMemory(&medium, sizeof (STGMEDIUM));
  // {
    // HRESULT hResult;
    // if ((hResult = (*(*dataObject).lpVtbl).GetData(dataObject, &formatetc, &medium))
        // != S_OK) {
      // LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                            // HRESULT_FACILITY(hResult),
                            // HRESULT_CODE(hResult)),
                 // TEXT("%S(0x%p, 0x%p , 0x%p, 0x%08X): Could not get data"),
                 // __FUNCTION__, This, itemIdList, dataObject, key);
      // ReleaseStgMedium(&medium);
      // (*(*dataObject).lpVtbl).Release(dataObject);
      // (*(IcedTea_WebShellExtInit*)This).dataObject = NULL;
      // if (medium)
        // if (medium.pUnkForRelease)
          // (*(*medium.pUnkForRelease).lpVtbl).Release(medium.pUnkForRelease);
        // else
          // GlobalFree(medium.hGlobal);
      // return hResult;
    // }
  // }
  // TCHAR* data;
  // LogMessage(0u, TEXT("%S(0x%p, 0x%p , 0x%p, 0x%08X): dataObject.GetData(0x%p, 0x%p): GlobalLock(medium.hGlobal) == \"%s\""),
                 // __FUNCTION__, This, itemIdList, dataObject, key, &formatetc, &medium, data = GlobalLock(medium.hGlobal));
  // if (GlobalUnlock(medium.hGlobal) != NO_ERROR) {
    // DWORD error;
    // LogMessage(error = GetLastError(),
               // TEXT("%S(0x%p, 0x%p , 0x%p, 0x%08X): Could not unlock globally allocated memory"),
               // __FUNCTION__, This, itemIdList, dataObject, key);
    // ReleaseStgMedium(&medium);
    // (*(*dataObject).lpVtbl).Release(dataObject);
    // (*(IcedTea_WebShellExtInit*)This).dataObject = NULL;
    // return HRESULT_FROM_WIN32(error);
  // }
  // ReleaseStgMedium(&medium);
  // return S_OK;
// }

/* IPersistFile methods */
HRESULT STDMETHODCALLTYPE PersistFile_QueryInterface(
  IPersistFile* This, REFIID iid, void** object) {
  if (This == NULL || iid == NULL) {
    if (object != NULL) *object == NULL;
    return E_INVALIDARG;
  }
  if (object == NULL) return E_POINTER;

  {
    LPOLESTR iidString = NULL;
    HRESULT hr;
    if ((hr = StringFromIID(iid, &iidString)) != S_OK) {
      *object = NULL;
      LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hr),
                            HRESULT_FACILITY(hr),
                            HRESULT_CODE(hr)),
                 TEXT("%S(): Could not make string from IID"), __FUNCTION__);
      return hr;
    }
    LogMessage(0u, TEXT("%S(0x%p, 0x%p == \"%s\", 0x%p)"), __FUNCTION__, This, iid, iidString, object);
    CoTaskMemFree(iidString);
  }
  if (IsEqualIID(iid, &IID_IUnknown) ||
      IsEqualIID(iid, &IID_IPersist) ||
      IsEqualIID(iid, &IID_IPersistFile)) {
    (*(*(IPersistFile*)*object).lpVtbl).AddRef((IPersistFile*)(*object = This));
    return S_OK;
  } 
  *object = NULL;
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE PersistFile_AddRef(IPersistFile* This) {
  LogMessage(0u, TEXT("%S(0x%p): refCount == %u\n"), __FUNCTION__, This, (*(IcedTea_WebPersistFile*)This).refCount);

  return This ?
            (*(IcedTea_WebPersistFile*)This).refCount == MAXULONG ?
              MAXULONG :
              InterlockedIncrement(&(*(IcedTea_WebPersistFile*)This).refCount) :
            0u;
}

ULONG STDMETHODCALLTYPE PersistFile_Release(IPersistFile* This) {
  if (This == NULL) return 0u;

  LogMessage(0u, TEXT("%S(0x%p): refCount == %u\n"), __FUNCTION__, This, (*(IcedTea_WebPersistFile*)This).refCount);

  if ((*(IcedTea_WebPersistFile*)This).refCount) {
    if (InterlockedDecrement(&(*(IcedTea_WebPersistFile*)This).refCount))
      return (*(IcedTea_WebIconHandler*)This).refCount;
  } else return 0u;
  SysFreeString((*(IcedTea_WebPersistFile*)This).fileName);
  (*(IcedTea_WebPersistFile*)This).fileName = NULL;
  (*(IcedTea_WebPersistFile*)This).mode = 0u;
  CoTaskMemFree((*This).lpVtbl);
  (*This).lpVtbl = NULL;
  CoTaskMemFree(This);
  return 0u;
}

HRESULT STDMETHODCALLTYPE PersistFile_GetClassID(
  IPersistFile* This,
  CLSID* clsid) {
  if (This == NULL || clsid == NULL) return E_FAIL;

  LogMessage(0u, TEXT("%S(0x%p, 0x%p)"), __FUNCTION__, This, clsid);

  *clsid = CLSID_IcedTea_Web;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE PersistFile_GetCurFile(
  IPersistFile* This,
  LPOLESTR* fileName) {
  if (This == NULL || fileName == NULL) {
    if (fileName) *fileName = NULL;
    return E_FAIL;
  }

  LogMessage(0u, TEXT("%S(0x%p, 0x%p == \"%s\")"), __FUNCTION__, This, fileName, *fileName);

  return (*(IcedTea_WebPersistFile*)This).fileName &&
          (*(IcedTea_WebPersistFile*)This).fileName[0] ?
    // FIXME: Replace SysReAllocString() possibly with SysAllocString()?
    // However, LPOLESTR strings have special NULL/empty string semantics; NULL
    // LPOLESTR pointers are equivalent to empty strings. See
    // https://msdn.microsoft.com/en-us/library/ms221105%28v=vs.85%29.aspx
    SysReAllocString(fileName, (*(IcedTea_WebPersistFile*)This).fileName) ?
    // (*fileName = SysAllocString((*(IcedTea_WebPersistFile*)This).fileName)) ?
      S_OK :
      E_OUTOFMEMORY :
    SysReAllocString(fileName, OLESTR("*.jnlp")) ?
    // (*fileName = SysAllocString(OLESTR("*.jnlp"))) ?
      S_FALSE :
      E_OUTOFMEMORY;
}

HRESULT STDMETHODCALLTYPE PersistFile_IsDirty(
  IPersistFile* This) {
  if (This == NULL) return E_INVALIDARG;

  return S_FALSE;
}

HRESULT STDMETHODCALLTYPE PersistFile_Load(
  IPersistFile* This,
  LPCOLESTR fileName,
  DWORD mode) {
  if (This == NULL) return E_INVALIDARG;

  LogMessage(0u, TEXT("%S(0x%p, 0x%p == \"%s\", 0x%08X)"), __FUNCTION__, This, fileName, fileName, mode);

  if (SysReAllocString(&(*(IcedTea_WebPersistFile*)This).fileName, fileName) ==
      FALSE)
    return E_OUTOFMEMORY;
  (*(IcedTea_WebPersistFile*)This).mode = mode;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE PersistFile_Save(
  IPersistFile* This,
  LPCOLESTR fileName,
  BOOL remember) {
  if (This == NULL) return E_INVALIDARG;

  return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE PersistFile_SaveCompleted(
  IPersistFile* This,
  LPCOLESTR fileName) {
  if (This == NULL) return E_INVALIDARG;

  return S_OK;
}

/* IExtractIcon methods */
HRESULT STDMETHODCALLTYPE ExtractIcon_QueryInterface(
  IExtractIcon* This, REFIID iid, void** object) {
  if (This == NULL || iid == NULL) {
    if (object) *object == NULL;
    return E_INVALIDARG;
  }
  if (object == NULL) return E_POINTER;

  {
    LPOLESTR iidString = NULL;
    HRESULT hr;
    if ((hr = StringFromIID(iid, &iidString)) != S_OK) {
      *object = NULL;
      LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hr),
                            HRESULT_FACILITY(hr),
                            HRESULT_CODE(hr)),
                 TEXT("%S(): Could not make string from IID"), __FUNCTION__);
      return hr;
    }
    LogMessage(0u, TEXT("%S(0x%p, 0x%p == \"%s\", 0x%p)"), __FUNCTION__, This, iid, iidString, object);
    CoTaskMemFree(iidString);
  }
  if (IsEqualIID(iid, &IID_IUnknown) ||
      IsEqualIID(iid, &IID_IExtractIcon) ||
    (*(*This).lpVtbl).AddRef((IExtractIcon*)(*object = This));
    return S_OK;
  }
  if (IsEqualIID(iid, &IID_IPersist) ||
      IsEqualIID(iid, &IID_IPersistFile)) {
    *object = (*(IcedTea_WebIconHandler*)This).persistFile;
  }
  // TODO: Add IRunnableTask
  // IsEqualIID(iid, &IID_IRunnableTask)

  *object = NULL;
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ExtractIcon_AddRef(IExtractIcon* This) {
  LogMessage(0u, TEXT("%S(0x%p): refCount == %u\n"), __FUNCTION__, This, (*(IcedTea_WebIconHandler*)This).refCount);

  return This ?
            (*(IcedTea_WebIconHandler*)This).refCount == MAXULONG ?
              MAXULONG :
              InterlockedIncrement(&(*(IcedTea_WebIconHandler*)This).refCount) :
            0u;
}

ULONG STDMETHODCALLTYPE ExtractIcon_Release(IExtractIcon* This) {
  if (This == NULL) return 0u;

  LogMessage(0u, TEXT("%S(0x%p): refCount == %u\n"), __FUNCTION__, This, (*(IcedTea_WebIconHandler*)This).refCount);

  if ((*(IcedTea_WebIconHandler*)This).refCount) {
    if (InterlockedDecrement(&(*(IcedTea_WebIconHandler*)This).refCount))
      return (*(IcedTea_WebIconHandler*)This).refCount;
  } else return 0u;
  (*(*(*(IcedTea_WebIconHandler*)This).persistFile).lpVtbl).Release(
    (IPersistFile*)(*(IcedTea_WebIconHandler*)This).persistFile);
  (*(IcedTea_WebIconHandler*)This).persistFile = NULL;
  CoTaskMemFree((*This).lpVtbl);
  (*This).lpVtbl = NULL;
  CoTaskMemFree((IcedTea_WebIconHandler*)This);
  return 0u;
}

HRESULT STDMETHODCALLTYPE ExtractIcon_Extract(
  IExtractIcon* This,
  PCTSTR pszFile,
  UINT nIconIndex,
  HICON *phiconLarge,
  HICON *phiconSmall,
  UINT nIconSize) {
  if (This == NULL || pszFile == NULL) return E_INVALIDARG;

  LogMessage(0u,
             TEXT("%S(0x%p, 0x%p == \"%s\", %u, 0x%p, 0x%p, %u)"),
             __FUNCTION__,
             This,
             pszFile,
             pszFile,
             nIconIndex,
             phiconLarge,
             phiconSmall,
             nIconSize);

  // if (phiconLarge != NULL) *phiconLarge = NULL;
  // if (phiconSmall != NULL) *phiconSmall = NULL;
  return S_FALSE;
}

HRESULT STDMETHODCALLTYPE ExtractIcon_GetIconLocation(
  IExtractIcon* This,
  UINT uFlags,
  PTSTR pszIconFile,
  UINT cchMax,
  int *piIndex,
  UINT *pwFlags) {
  if (This == NULL || pszIconFile == NULL || piIndex == NULL ||
      pwFlags == NULL) return E_INVALIDARG;

  LogMessage(0u,
             TEXT("%S(0x%p, 0x%08X == %s%s%s%s%s%s, 0x%p == \"%s\", %u, 0x%p, 0x%p)"),
             __FUNCTION__,
             This,
             uFlags,
             uFlags & GIL_ASYNC ? TEXT("GIL_ASYNC") : TEXT(""),
             uFlags & GIL_DEFAULTICON ? uFlags & GIL_ASYNC ? TEXT(" | GIL_DEFAULTICON") : TEXT("GIL_DEFAULTICON") : TEXT(""),
             uFlags & GIL_FORSHELL ? uFlags & (GIL_ASYNC | GIL_DEFAULTICON) ? TEXT(" | GIL_FORSHELL") : TEXT("GIL_FORSHELL") : TEXT(""),
             uFlags & GIL_FORSHORTCUT ? uFlags & (GIL_ASYNC | GIL_DEFAULTICON | GIL_FORSHELL) ? TEXT(" | GIL_FORSHORTCUT") : TEXT("GIL_FORSHORTCUT") : TEXT(""),
             uFlags & GIL_OPENICON ? uFlags & (GIL_ASYNC | GIL_DEFAULTICON | GIL_FORSHELL | GIL_FORSHORTCUT) ? TEXT(" | GIL_OPENICON") : TEXT("GIL_OPENICON") : TEXT(""),
             uFlags & GIL_CHECKSHIELD ? uFlags & (GIL_ASYNC | GIL_DEFAULTICON | GIL_FORSHELL | GIL_FORSHORTCUT | GIL_OPENICON) ? TEXT(" | GIL_CHECKSHIELD") : TEXT("GIL_CHECKSHIELD") : TEXT(""),
             pszIconFile,
             pszIconFile,
             cchMax,
             piIndex,
             pwFlags);

  // if (piIndex != NULL) *piIndex = 0;
  // if (pwFlags != NULL) *pwFlags = 0u;
  // return E_NOTIMPL;
  if (uFlags & GIL_ASYNC) {
    pszIconFile[0] = TEXT('\x00');
    *piIndex = 0;
    *pwFlags = 0u;
    return E_NOTIMPL;
  }
  pszIconFile[0] = TEXT('\x00');
  *piIndex = 0;
  *pwFlags = 0u;
  return S_FALSE;
}