#ifndef _ICEDTEASHELLEXT_H_
#define _ICEDTEASHELLEXT_H_

#include <unknwn.h>
#include <objidl.h>
#include <shlobj.h>

/* IPersistFile methods */
extern HRESULT STDMETHODCALLTYPE PersistFile_QueryInterface(IPersistFile*,
                                                            REFIID,
                                                            void**);
extern ULONG STDMETHODCALLTYPE PersistFile_AddRef(IPersistFile*);
extern ULONG STDMETHODCALLTYPE PersistFile_Release(IPersistFile*);
extern HRESULT STDMETHODCALLTYPE PersistFile_GetClassID(IPersistFile*,
                                                        CLSID*);
extern HRESULT STDMETHODCALLTYPE PersistFile_GetCurFile(IPersistFile*,
                                                        LPOLESTR*);
extern HRESULT STDMETHODCALLTYPE PersistFile_IsDirty(IPersistFile*);
extern HRESULT STDMETHODCALLTYPE PersistFile_Load(IPersistFile*,
                                                  LPCOLESTR,
                                                  DWORD);
extern HRESULT STDMETHODCALLTYPE PersistFile_Save(IPersistFile*,
                                                  LPCOLESTR,
                                                  BOOL);
extern HRESULT STDMETHODCALLTYPE PersistFile_SaveCompleted(IPersistFile*,
                                                           LPCOLESTR);

typedef struct {
  IPersistFileVtbl* lpVtbl;
  volatile ULONG refCount;
  HANDLE refCountMutex;
  LPOLESTR fileName;
  DWORD mode;
} IcedTea_WebPersistFile;


/* IExtractIcon methods */
extern HRESULT STDMETHODCALLTYPE ExtractIcon_QueryInterface(IExtractIcon*,
                                                            REFIID,
                                                            void**);
extern ULONG STDMETHODCALLTYPE ExtractIcon_AddRef(IExtractIcon*);
extern ULONG STDMETHODCALLTYPE ExtractIcon_Release(IExtractIcon*);
extern HRESULT STDMETHODCALLTYPE ExtractIcon_GetIconLocation(IExtractIcon*,
                                                             UINT,
                                                             PTSTR,
                                                             UINT,
                                                             int*,
                                                             UINT*);
extern HRESULT STDMETHODCALLTYPE ExtractIcon_Extract(IExtractIcon*,
                                                     PCTSTR,
                                                     UINT,
                                                     HICON*,
                                                     HICON*,
                                                     UINT);

typedef struct {
    struct {
STDMETHOD(QueryInterface)(IExtractIcon*,
                                                            REFIID,
                                                            void**);
STDMETHOD_(ULONG, AddRef)(IExtractIcon*);
STDMETHOD_(ULONG, Release)(IExtractIcon*);
STDMETHOD(GetIconLocation)(IExtractIcon*,
                                                             UINT,
                                                             PTSTR,
                                                             UINT,
                                                             int*,
                                                             UINT*);
STDMETHOD(Extract)(IExtractIcon*,
                                                     PCTSTR,
                                                     UINT,
                                                     HICON*,
                                                     HICON*,
                                                     UINT);
    } *lpVtbl;
  // IExtractIconVtbl* lpVtbl;
  IcedTea_WebPersistFile* persistFile;
  volatile ULONG refCount;
  HANDLE refCountMutex;
} IcedTea_WebIconHandler;


// /* IShellExtInit methods */
// extern HRESULT STDMETHODCALLTYPE ShellExtInit_QueryInterface(IShellExtInit*,
                                                             // REFIID,
                                                             // void**);
// extern ULONG STDMETHODCALLTYPE ShellExtInit_AddRef(IShellExtInit*);
// extern ULONG STDMETHODCALLTYPE ShellExtInit_Release(IShellExtInit*);
// extern HRESULT STDMETHODCALLTYPE ShellExtInit_Initialize(IShellExtInit*,
                                                         // PCIDLIST_ABSOLUTE,
                                                         // IDataObject*,
                                                         // HKEY);

// typedef struct {
  // IShellExtInitVtbl* lpVtbl;
  // IQueryInfoVtbl* lpVtblQueryInfo;
  // volatile ULONG refCount;
  // IDataObject* dataObject;
  // HANDLE dataObjectMutex;
// } IcedTea_WebPropertyHandler;
#endif /* _ICEDTEASHELLEXT_H_ */