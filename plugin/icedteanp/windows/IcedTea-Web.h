#ifndef _ICEDTEA_WEB_H_
#define _ICEDTEA_WEB_H_

#ifndef RC_INVOKED
#include <windows.h>
#include <ole2.h>
#include <shlobj.h>
#include <objidl.h>

#if !defined __cplusplus && (defined __MINGW32__ || defined __MINGW64__)
typedef BOOL bool;
#define true  TRUE
#define false FALSE
#endif /* __cplusplus && (__MINGW__ || __MINGW64__) */

/**
 * <summary>IcedTea-Web class ID.</summary>
 * <para>
 * This is effectively <c>SHA256("CLSID_IcedTea_Web")[15,31]</c>.
 * </para>
 * Link with <c>NPIcedTea-Web.lib</c>,<br/>
 * or with <c>NP<i>PACKAGE_NAME</i>.lib</c>,
 * <c><i>PLUGIN_MODULE_NAME</i>.lib</c>, or <c><i>PLUGIN_LIB</i></c> if the
 * IcedTea-Web for Windows package has been rebranded.
 * <seealso cref="IID_IIcedTea_Web"/>
 */
extern const CLSID CLSID_IcedTea_Web;

/**
 * <summary>IcedTea-Web interface ID.</summary>
 * <para>
 * This is effectively <c>SHA256("IID_IcedTea_Web")[15,31]</c>.
 * </para>
 * Link with <c>NPIcedTea-Web.lib</c>,<br/>
 * or with <c>NP<i>PACKAGE_NAME</i>.lib</c>,
 * <c><i>PLUGIN_MODULE_NAME</i>.lib</c>, or <c><i>PLUGIN_LIB</i></c> if the
 * IcedTea-Web for Windows package has been rebranded.
 * <seealso cref="CLSID_IcedTea_Web"/>
 */
extern const IID IID_IIcedTea_Web;

/**
 * <summary>IcedTea-Web shell extension interface ID.</summary>
 * <para>
 * This is effectively <c>SHA256("IID_IcedTea_WebShellExtension")[15,31]</c>.
 * </para>
 * Link with <c>NPIcedTea-Web.lib</c>,<br/>
 * or with <c>NP<i>PACKAGE_NAME</i>.lib</c>,
 * <c><i>PLUGIN_MODULE_NAME</i>.lib</c>, or <c><i>PLUGIN_LIB</i></c> if the
 * IcedTea-Web for Windows package has been rebranded.
 * <seealso cref="CLSID_IcedTea_Web"/>
 */
extern const IID IID_IIcedTea_WebShellExtension;

#ifndef __IIcedTea_Web_FORWARD_DECLARATION__
#define __IIcedTea_Web_FORWARD_DECLARATION__
typedef struct IIcedTea_Web IIcedTea_Web;
#endif /* __IIcedTea_Web_FORWARD_DECLARATION__ */

typedef struct IIcedTea_WebVtbl {
  /* IUnknown methods */
  HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUnknown*, REFIID, void**);

  ULONG (STDMETHODCALLTYPE *AddRef)(IUnknown*);

  ULONG (STDMETHODCALLTYPE *Release)(IUnknown*);

  /* IOleObject methods */
  HRESULT (STDMETHODCALLTYPE *SetClientSite)(IIcedTea_Web*,
                                             IOleClientSite*);

  HRESULT (STDMETHODCALLTYPE *GetClientSite)(IIcedTea_Web*,
                                             IOleClientSite**);

  HRESULT (STDMETHODCALLTYPE *SetHostNames)(IIcedTea_Web*,
                                            LPCOLESTR,
                                            LPCOLESTR);

  HRESULT (STDMETHODCALLTYPE *Close)(IIcedTea_Web*,
                                     DWORD);

  HRESULT (STDMETHODCALLTYPE *SetMoniker)(IIcedTea_Web*,
                                          DWORD,
                                          IMoniker*);

  HRESULT (STDMETHODCALLTYPE *GetMoniker)(IIcedTea_Web*,
                                          DWORD,
                                          DWORD,
                                          IMoniker **);

  HRESULT (STDMETHODCALLTYPE *InitFromData)(IIcedTea_Web*,
                                            IDataObject *,
                                            WINBOOL,
                                            DWORD);

  HRESULT (STDMETHODCALLTYPE *GetClipboardData)(IIcedTea_Web*,
                                                DWORD,
                                                IDataObject**);

  HRESULT (STDMETHODCALLTYPE *DoVerb)(IIcedTea_Web*,
                                      LONG,
                                      LPMSG,
                                      IOleClientSite*,
                                      LONG,
                                      HWND,
                                      LPCRECT);

  HRESULT (STDMETHODCALLTYPE *EnumVerbs)(IIcedTea_Web*,
                                         IEnumOLEVERB**);

  HRESULT (STDMETHODCALLTYPE *Update)(IIcedTea_Web*);

  HRESULT (STDMETHODCALLTYPE *IsUpToDate)(IIcedTea_Web*);

  HRESULT (STDMETHODCALLTYPE *GetUserClassID)(IIcedTea_Web*,
                                              CLSID*);

  HRESULT (STDMETHODCALLTYPE *GetUserType)(IIcedTea_Web*,
                                           DWORD,
                                           LPOLESTR*);

  HRESULT (STDMETHODCALLTYPE *SetExtent)(IIcedTea_Web*,
                                         DWORD,
                                         SIZEL*);

  HRESULT (STDMETHODCALLTYPE *GetExtent)(IIcedTea_Web*,
                                         DWORD,
                                         SIZEL*);

  HRESULT (STDMETHODCALLTYPE *Advise)(IIcedTea_Web*,
                                      IAdviseSink*,
                                      DWORD*);

  HRESULT (STDMETHODCALLTYPE *Unadvise)(IIcedTea_Web*,
                                        DWORD);

  HRESULT (STDMETHODCALLTYPE *EnumAdvise)(IIcedTea_Web*,
                                          IEnumSTATDATA**);

  HRESULT (STDMETHODCALLTYPE *GetMiscStatus)(IIcedTea_Web*,
                                             DWORD,
                                             DWORD*);

  HRESULT (STDMETHODCALLTYPE *SetColorScheme)(IIcedTea_Web*,
                                              LOGPALETTE*);
} IIcedTea_WebVtbl;

typedef struct IIcedTea_Web {
  IIcedTea_WebVtbl* lpVtbl;
} IIcedTea_Web;

// #ifndef __IIcedTea_WebShellExtension_FORWARD_DECLARATION__
// #define __IIcedTea_WebShellExtension_FORWARD_DECLARATION__
// typedef struct IIcedTea_WebShellExtension IIcedTea_WebShellExtension;
// #endif /* __IIcedTea_WebShellExtension_FORWARD_DECLARATION__ */

// typedef struct IIcedTea_WebShellExtensionVtbl {
  // /* IUnknown methods */
  // HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUnknown*, REFIID, void**);

  // ULONG (STDMETHODCALLTYPE *AddRef)(IUnknown*);

  // ULONG (STDMETHODCALLTYPE *Release)(IUnknown*);

  // /* IExtractIcon methods */
  // HRESULT (STDMETHODCALLTYPE *GetIconLocation)(IExtractIcon*,
                                       // UINT,
                                       // PTSTR,
                                       // UINT,
                                       // int*,
                                       // UINT*);
  // HRESULT (STDMETHODCALLTYPE *Extract)(IExtractIcon*,
                               // PCTSTR,
                               // UINT,
                               // HICON*,
                               // HICON*,
                               // UINT);

  // /* IPersistFile methods */
  // HRESULT (STDMETHODCALLTYPE *GetClassID)(IPersistFile*, CLSID*);

  // HRESULT (STDMETHODCALLTYPE *GetCurFile)(IPersistFile*, LPOLESTR*);

  // HRESULT (STDMETHODCALLTYPE *IsDirty)(IPersistFile*);

  // HRESULT (STDMETHODCALLTYPE *Load)(IPersistFile*, LPCOLESTR, DWORD);

  // HRESULT (STDMETHODCALLTYPE *Save)(IPersistFile*, LPCOLESTR, BOOL);

  // HRESULT (STDMETHODCALLTYPE *SaveCompleted)(IPersistFile*, LPCOLESTR);

  // /* IShellExtInit methods */
  // HRESULT (STDMETHODCALLTYPE *Initialize)(IShellExtInit*,
                                          // PCIDLIST_ABSOLUTE,
                                          // IDataObject*,
                                          // HKEY);
// } IIcedTea_WebShellExtensionVtbl;

// typedef struct IIcedTea_WebShellExtension {
  // const IIcedTea_WebShellExtensionVtbl* lpVtbl;
// } IIcedTea_WebShellExtension;

#ifndef __cplusplus
/* IIcedTea_Web */
/* IUnknown methods */
#define IIcedTea_Web_QueryInterface(This, riid, ppvObject) \
   (*(*This).lpVtbl).QueryInterface(This, riid, ppvObject)
#define IIcedTea_Web_AddRef(This) \
   (*(*This).lpVtbl).AddRef(This)
#define IIcedTea_Web_Release(This) \
   (*(*This).lpVtbl).Release(This)
/* IOleObject methods */
#define IIcedTea_Web_SetClientSite(This, pClientSite) \
   (*(*This).lpVtbl).SetClientSite(This, pClientSite)
#define IIcedTea_Web_GetClientSite(This, ppClientSite) \
   (*(*This).lpVtbl).GetClientSite(This, ppClientSite)
#define IIcedTea_Web_SetHostNames(This, szContainerApp, szContainerObj) \
   (*(*This).lpVtbl).SetHostNames(This, szContainerApp, szContainerObj)
#define IIcedTea_Web_Close(This, dwSaveOption) \
   (*(*This).lpVtbl).Close(This, dwSaveOption)
#define IIcedTea_Web_SetMoniker(This, dwWhichMoniker, pmk) \
   (*(*This).lpVtbl).SetMoniker(This, dwWhichMoniker, pmk)
#define IIcedTea_Web_GetMoniker(This, dwAssign, dwWhichMoniker, ppmk) \
   (*(*This).lpVtbl).GetMoniker(This, dwAssign, dwWhichMoniker, ppmk)
#define IIcedTea_Web_InitFromData(This, pDataObject, fCreation, dwReserved) \
   (*(*This).lpVtbl).InitFromData(This, pDataObject, fCreation, dwReserved)
#define IIcedTea_Web_GetClipboardData(This, dwReserved, ppDataObject) \
   (*(*This).lpVtbl).GetClipboardData(This, dwReserved, ppDataObject)
#define IIcedTea_Web_DoVerb(This,\
                            iVerb,\
                            lpmsg,\
                            pActiveSite,\
                            lindex,\
                            hwndParent,\
                            lprcPosRect) \
   (*(*This).lpVtbl).DoVerb(This,\
                            iVerb,\
                            lpmsg,\
                            pActiveSite,\
                            lindex,\
                            hwndParent,\
                            lprcPosRect)
#define IIcedTea_Web_EnumVerbs(This, ppEnumOleVerb) \
   (*(*This).lpVtbl).EnumVerbs(This, ppEnumOleVerb)
#define IIcedTea_Web_Update(This) \
   (*(*This).lpVtbl).Update(This)
#define IIcedTea_Web_IsUpToDate(This) \
   (*(*This).lpVtbl).IsUpToDate(This)
#define IIcedTea_Web_GetUserClassID(This, pClsid) \
   (*(*This).lpVtbl).GetUserClassID(This, pClsid)
#define IIcedTea_Web_GetUserType(This, dwFormOfType, pszUserType) \
   (*(*This).lpVtbl).GetUserType(This, dwFormOfType, pszUserType)
#define IIcedTea_Web_SetExtent(This, dwDrawAspect, psizel) \
   (*(*This).lpVtbl).SetExtent(This, dwDrawAspect, psizel)
#define IIcedTea_Web_GetExtent(This, dwDrawAspect, psizel) \
   (*(*This).lpVtbl).GetExtent(This, dwDrawAspect, psizel)
#define IIcedTea_Web_Advise(This, pAdvSink, pdwConnection) \
   (*(*This).lpVtbl).Advise(This, pAdvSink, pdwConnection)
#define IIcedTea_Web_Unadvise(This, dwConnection) \
   (*(*This).lpVtbl).Unadvise(This, dwConnection)
#define IIcedTea_Web_EnumAdvise(This, ppenumAdvise) \
   (*(*This).lpVtbl).EnumAdvise(This, ppenumAdvise)
#define IIcedTea_Web_GetMiscStatus(This, dwAspect, pdwStatus) \
   (*(*This).lpVtbl).GetMiscStatus(This, dwAspect, pdwStatus)
#define IIcedTea_Web_SetColorScheme(This, pLogpal) \
   (*(*This).lpVtbl).SetColorScheme(This, pLogpal)

// /* IIcedTea_WebShellExtension */
// /* IUnknown methods */
// #define IIcedTea_WebShellExtension_QueryInterface(This, riid, ppvObject) \
                 // (*(*This).lpVtbl).QueryInterface(This, riid, ppvObject)
// #define IIcedTea_WebShellExtension_AddRef(This) \
                 // (*(*This).lpVtbl).AddRef(This)
// #define IIcedTea_WebShellExtension_Release(This) \
                 // (*(*This).lpVtbl).Release(This)
// /* IExtractIcon methods */
// #define IIcedTea_WebShellExtension_GetIconLocation(This,\
                                                   // uFlags,\
                                                   // pszIconFile,\
                                                   // cchMax,\
                                                   // piIndex,\
                                                   // pwFlags) \
                 // (*(*This).lpVtbl).GetIconLocation(This,\
                                                   // uFlags,\
                                                   // pszIconFile,\
                                                   // cchMax,\
                                                   // piIndex,\
                                                   // pwFlags)
// #define IIcedTea_WebShellExtension_Extract(This,\
                                           // pszFile,\
                                           // nIconIndex,\
                                           // phiconLarge,\
                                           // phiconSmall,\
                                           // nIconSize) \
                 // (*(*This).lpVtbl).Extract(This,\
                                           // pszFile,\
                                           // nIconIndex,\
                                           // phiconLarge,\
                                           // phiconSmall,\
                                           // nIconSize)
// /* IPersist methods */
// #define IIcedTea_WebShellExtension_GetClassID(This, pClassID) \
                 // (*(*This).lpVtbl).GetClassID(This, pClassID)
// /* IPersistFile methods */
// #define IIcedTea_WebShellExtension_GetCurFile(This, ppszFileName) \
                 // (*(*This).lpVtbl).GetCurFile(This, ppszFileName)
// #define IIcedTea_WebShellExtension_IsDirty(This) \
                 // (*(*This).lpVtbl).IsDirty(This)
// #define IIcedTea_WebShellExtension_Load(This, ppszFileName, dwMode) \
                 // (*(*This).lpVtbl).Load(This, ppszFileName, dwMode)
// #define IIcedTea_WebShellExtension_Save(This, ppszFileName, dwMode) \
                 // (*(*This).lpVtbl).Save(This, ppszFileName, dwMode)
// #define IIcedTea_WebShellExtension_SaveCompleted(This, ppszFileName) \
                 // (*(*This).lpVtbl).SaveCompleted(This, ppszFileName)
// /* IShellExtInit methods */
// #define IIcedTea_WebShellExtension_Initialize(This,\
                                              // pidlFolder,\
                                              // pdtobj,\
                                              // hkeyProgID) \
                 // (*(*This).lpVtbl).Initialize(This,\
                                              // pidlFolder,\
                                              // pdtobj,\
                                              // hkeyProgID)
#endif /* __cplusplus */
#endif /* RC_INVOKED */
#endif /* _ICEDTEA_WEB_H_ */
