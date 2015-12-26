#include <windows.h>
#include <ntdef.h>
#include <shlwapi.h>
#include <shlobj.h>
// Both tchar.h AND wchar.h are required for Unicode!
#include <tchar.h>
#include <wchar.h>
// tchar.h and wchar.h MUST be included BEFORE stdio.h!
#include <stdio.h>

#include "IcedTea-Web.h"
#include "IcedTea-Web.rc"
#include "config.h"
#include "Dll.h"
#include "IcedTeaIEPlugin.h"
#include "IcedTeaShellExt.h"
#include "LogMessage.h"

/**
 * <summary>
 * Registers the <see cref="CLSID_IcedTea_Web"/> class and <see
 * cref="IID_IIcedTea_Web"/> interface ID by writing the a appropriate keys,
 * values, and data into the registry.</summary>
 * <para>
 * The following registry keys and values are created:<br/>
 * <dl>
 * <dt><c>HKEY_CLASSES_ROOT\\CLSID\\{<i><see cref="CLSID_IcedTea_Web"/></i>}</c></dt>
 * <dd><c>(Default) = IcedTea-Web</c></dd>
 * <dl><dt><c>Control</c></dt>
 * <dt><c>DefaultIcon</c></dt>
 * <dd><c>(Default) = &quot;<i>Path to NPIcedTea-Web module's location</i>&quot;,-<i>ID_ICON</i></c></dd>
 * <dt><c>InprocHandler32</c></dt>
 * <dd><c>(Default) = OLE32.DLL</c></dd>
 * <dt><c>InprocServer32</c></dt>
 * <dd><c>(Default) = <i>Path to NPIcedTea-Web module's location</i></c><br/>
 * <c>ThreadingModel = Both</c></dd>
 * <dt><c>Interface</c></dt>
 * <dd><c>{<i>IID_IUnknown</i>} = IUnknown</c><br/>
 * <c>{<i>IID_IOleObject</i>} = IOleObject</c><br/>
 * <c>{<i><see cref="CLSID_IcedTea_Web"/></i>} = IIcedTea_Web</c></dd>
 * <dt><c>Version</c></dt>
 * <dd><c>(Default) = <i>x.x.x.x version of IcedTea-Web module</i></c></dd>
 * </dl></para>
 * <para><dl>
 * <dt><c>HKEY_CLASSES_ROOT\\.jnlp</c></dt>
 * <dd><c>(Default) = <i>PACKAGE_VENDOR</i>.<i>PACKAGE_NAME</i></c><br/>
 * <c>Content Type = application/x-java-jnlp-file</c><br/>
 * <c>PerceivedType = text</c></dd>
 * </dl></para>
 * <para><dl>
 * <dt><c>HKEY_CLASSES_ROOT\\<i>PACKAGE_VENDOR</i>.<i>PACKAGE_NAME</i></c></dt>
 * <dd><c>(Default) = Java&trade; Network Launch Protocol</c></dd>
 * <dl><dt><c>CurVer</c></dt>
 * <dd><c>(Default) = <i>PACKAGE_VENDOR</i>.<i>PACKAGE_NAME</i>.<i>PACKAGE_VERSION</i></c></dd>
 * </dl></dl></para>
 * <para><dl>
 * <dt><c>HKEY_CLASSES_ROOT\\<i>PACKAGE_VENDOR</i>.<i>PACKAGE_NAME</i>.<i>PACKAGE_VERSION</i></c></dt>
 * <dd><c>(Default) = Java&trade; Network Launch Protocol</c><br/>
 * <c>FriendlyTypeName = \@&quot;<i>Path to NPIcedTea-Web module's location</i>&quot;,-<i>IDS_FRIENDLYTYPENAME</i></c><br/>
 * <c>NeverShowExt</c></dd>
 * <dl><dt><c>CLSID</c></dt>
 * <dd><c>(Default) = {<i>This PACKAGE_VERSION's <see cref="CLSID_IcedTea_Web"/></i>}</c></dd>
 * <dt><c>DefaultIcon</c></dt>
 * <dd><c>(Default) = &quot;<i>Path to NPIcedTea-Web module's location</i>&quot;,-<i>ID_ICON</i></c></dd>
 * <dt><c>Insertable</c></dt>
 * </dl></dl></para>
 * <para><b>NOTE:</b> Currently, only system-wide registration is supported
 * (requires an account in the <c>BUILTIN\\Administrators</c> security group or
 * UAC). Registration by a user or for user only is to be added.</para>
 * <para>TODO: Move registering and unregistering the event log source to <see
 * cref="DllInstall()"/>.</para>
 * <seealso cref="https://msdn.microsoft.com/en-us/library/ms683954%28v=vs.85%29.aspx">Registering COM Applications</seealso>
 * <seealso cref="https://msdn.microsoft.com/en-us/library/ms694355%28v=vs.85%29.aspx">Registry Entries</seealso>
 * <seealso cref="UnregisterCOMClass()"/>
 * <seealso cref="RegisterEventLogSource()"/>
 */
static HRESULT WINAPI RegisterCOMClass(void);

/**
 * <summary>
 * A helper function to call {@link RegisterCOMClass(void)} in a worker thread.
 * </summary>
 * <returns><see cref="RegisterCOMClass()"/>'s return value</returns>
 * <seealso cref="RegisterCOMClass()"/>
 * <seealso cref="DllRegisterServer()"/>
 */
static DWORD WINAPI RegisterCOMClassThread(LPVOID);

/**
 * <summary>
 * Registers the IcedTea-Web event log source with the event logging service.
 * </summary>
 * <seealso cref="http://msdn.microsoft.com/en-us/library/aa363661%28v=vs.85%29.aspx">Event sources</seealso>
 * <seealso cref="UnregisterEventLogSource()"/>
 * <seealso cref="eventSource"/>
 */
static HRESULT WINAPI RegisterEventLogSource(void);

/**
 * <summary>
 * Registers the IcedTea-Web plug-in with Mozilla's Firefox by writing the
 * appropriate keys, values, and data into the registry.
 * </summary>
 * The following registry keys and values are created:
 * <dl>
 * <dt><c>HKEY_LOCAL_MACHINE\\SOFTWARE\\MozillaPlugins\\\@<i>PACKAGE_VENDOR</i>/<i>PACKAGE_NAME</i>,version=<i>PACKAGE_VERSION</i></c></dt>
 * <dd><c>Description = <i>PACKAGE_VERSION</i></c><br/>
 * <c>Path = <i>Path to NPIcedTea-Web module's location</i></c><br/>
 * <c>ProductName = <i>PACKAGE_NAME</i></c><br/>
 * <c>Vendor = <i>PACKAGE_VENDOR</i></c><br/>
 * <c>Version = <i>PACKAGE_VERSION</i></c></dd></dl>
 * <dl><dt>NOTE:</dt>
 * <dd>Currently, only system-wide registration is supported (requires an
 * account in the <c>BUILTIN\\Administrators</c> security group or UAC). User
 * and user only registration is to be added.</dd></dl>
 * <seealso cref="https://developer.mozilla.org/en-US/Add-ons/Plugins/Gecko_Plugin_API_Reference/Plug-in_Basics#How_Gecko_Finds_Plug-ins">How Gecko finds plug-ins</seealso>
 * <seealso cref="https://developer.mozilla.org/en-US/Add-ons/Plugins/Gecko_Plugin_API_Reference/Plug-in_Development_Overview#Windows_Installation_Using_the_Registry">Windows
 * Installation Using the Registry</seealso>
 * <seealso cref="UnregisterNP()"/>
 */
static HRESULT WINAPI RegisterNP(void);

/**
 * <summary>
 * Unregisters the previously registered <see cref="CLSID_IcedTea_Web"/> class
 * and <see cref="IID_IIcedTea_Web"/> interface ID by deleting the a
 * appropriate keys and values from the registry.
 * </summary>
 * It also removes the previously created event log source registered with the
 * event log by <see cref="RegisterCOMClass()"/>.
 * <para>TODO: Move registering and unregistering the event log source to <see
 * cref="DllInstall()"/>.</para>
 * <seealso cref="RegisterCOMClass()"/>
 */
static HRESULT WINAPI UnregisterCOMClass(void);

/**
 * <summary>
 * A helper function to call <see cref="UnregisterCOMClass()"/> in a worker
 * thread.
 * </summary>
 * <returns><see cref="UnregisterCOMClass()"/>'s return value.</returns>
 * <seealso cref="UnregisterCOMClass()"/>
 * <seealso cref="DllUnregisterServer()"/>
 */
static DWORD WINAPI UnregisterCOMClassThread(LPVOID);

/**
 * <summary>
 * Unregisters the IcedTea-Web event log source from the event logging service.
 * </summary>
 * <seealso cref="RegisterEventLogSource()"/>
 */
static HRESULT WINAPI UnregisterEventLogSource(void);

/**
 * <summary>
 * Unregisters the IcedTea-Web plug-in with Mozilla's Firefox by deleting the
 * appropriate keys and values from the registry.
 * </summary>
 * <seealso cref="RegisterNP()"/>
 */
static HRESULT WINAPI UnregisterNP(void);

/**
 * <summary>
 * Attempts to set the global <see cref="currentUserSID"/> variable to a
 * pointer to the current user's SID.
 * </summary>
 * <para>
 * <dl><dt>WARNING:</dt>
 * <dd><b>DO NOT</b> call this function! It is a helper for <see
 * cref="LogMessage()"/> and <b>must not</b> be called by
 * external functions.</dd></dl></para>
 * <seealso cref="currentUserSID"/>
 * <seealso cref="LogMessage()"/>
 */
static VOID WINAPI SetCurrentUserSID(void);

/**
 * <summary>
 * Callback function handling Windows event messages for the
 * <i>DllInstall Syntax</i> dialog, defined in IcedTea-Web.rc.
 * </summary>
 * <returns><c>TRUE</c> if the message denoted in the <paramref
 * name="message"/> parameter has been handled by the function, otherwise
 * <c>FALSE</c>.</returns>
 * <param name="dialog">Handle to the dialog window for which a <paramref
 * name="message"/> is to be handled.</param>
 * <param name="message">The message to handle.</param>
 * <param name="wParam">The <c>WPARAM</c> type parameter of the <paramref
 * name="message"/> to handle. Its value depends on the <paramref
 * name="message"/>.</param>
 * <param name="lParam">The <c>LPARAM</c> type parameter of the <paramref
 * name="message"/> to handle. Its value depends on the <paramref
 * name="message"/>.</param>
 * <seealso cref="DllInstall()"/>
 */
static INT_PTR CALLBACK SyntaxDialogProc(HWND dialog,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam);

/**
 * <summary>
 * Gets a pointer to <i>this</i> module's absolute file name string.
 * </summary>
 * The memory pointed to by the return value <b>must</b> be released with
 * <see cref="CoTaskMemFree()"/>. Call <see cref="GetLastError()"/> for
 * extended error information.
 * <para>
 * <dl><dt>WARNING:</dt>
 * <dd><b>DO NOT</b> call this function! It is a helper for <see
 * cref="RegisterEventLogSource()"/> and <see cref="RegisterCOMClass()"/> and
 * <b>must not</b> be called by external functions.</dd></dl></para>
 * <returns>A pointer to <i>this</i> module's absolute file name string, or
 * <c>NULL</c> in case of an error.</returns>
 * <seealso cref="RegisterEventLogSource()"/>
 * <seealso cref="RegisterCOMClass()"/>
 */
static TCHAR* WINAPI GetThisModuleFileName(void);

/**
 * <summary>
 * IcedTea-Web's global <see cref="IClassFactory"/> reference counter.
 * </summary>
 * IcedTea-Web creates and manages only <b>one</b> class object for all of its
 * OLE browser plug-in, OLE container, and shell extension handler objects.
 * <seealso cref="IClassFactory"/>
 * <seealso cref="IIcedTea_Web"/>
 * <seealso cref="IIcedTea_WebShellExtension"/>
 */
static volatile ULONG iClassFactoryRefCount = 0u;
static IClassFactory* iClassFactory = NULL;

/* TODO: Add code to return S_OK after the last interface object has been
 * released */
HRESULT WINAPI DllCanUnloadNow() {
  LogMessage(0u, TEXT("%S(): iClassFactoryRefCount == %u"), __FUNCTION__, iClassFactoryRefCount);
  return iClassFactoryRefCount ? S_FALSE : S_OK;
}

HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID iid, LPVOID* v) {
  if (clsid == NULL || iid == NULL || v == NULL) {
    if (v != NULL) *v = NULL;
    return E_INVALIDARG;
  }
  if (!IsEqualCLSID(clsid, &CLSID_IcedTea_Web)) {
    *v = NULL;
    return CLASS_E_CLASSNOTAVAILABLE;
  }

  {
    LPOLESTR clsidString = NULL,
             iidString = NULL;
    {
      HRESULT hResult;
      if ((hResult = StringFromCLSID(clsid, &clsidString)) != S_OK) {
        *v = NULL;
        return hResult;
      }
      if ((hResult = StringFromIID(iid, &iidString)) != S_OK) {
        CoTaskMemFree(clsidString);
        *v = NULL;
        return hResult;
      }
    }
    LogMessage(0, TEXT("%S(0x%p == \"%s\", 0x%p == \"%s\", 0x%p)"), __FUNCTION__, clsid, clsidString, iid, iidString, v);
    CoTaskMemFree(clsidString);
    CoTaskMemFree(iidString);
  }

  if (IsEqualIID(iid, &IID_IUnknown) || IsEqualIID(iid, &IID_IClassFactory)) {
    if (iClassFactory) {
      *v = iClassFactory;
    } else {
      if ((*v = (LPVOID)CoTaskMemAlloc(sizeof (IClassFactory))) ==
          NULL)
        return E_OUTOFMEMORY;
      if (((*(IClassFactory*)*v).lpVtbl =
          (IClassFactoryVtbl*)CoTaskMemAlloc(
            sizeof (IClassFactoryVtbl))) == NULL) {
        CoTaskMemFree(*v);
        *v = NULL;
        return E_OUTOFMEMORY;
      }
      (*(*(iClassFactory = (IClassFactory*)*v)).lpVtbl).QueryInterface =
      ClassFactory_QueryInterface;
      (*(*(IClassFactory*)*v).lpVtbl).AddRef =
      ClassFactory_AddRef;
      (*(*(IClassFactory*)*v).lpVtbl).Release =
      ClassFactory_Release;
      (*(*(IClassFactory*)*v).lpVtbl).CreateInstance =
      ClassFactory_CreateInstance;
      (*(*(IClassFactory*)*v).lpVtbl).LockServer =
      ClassFactory_LockServer;
    }
    (*(*(IClassFactory*)*v).lpVtbl).AddRef(*v);
  } else {
    *v = NULL;
    return E_NOINTERFACE;
  }

  // if (!(IsEqualIID(iid, &IID_IUnknown) ||
        // IsEqualIID(iid, &IID_IOleObject) ||
        // IsEqualIID(iid, &IID_IIcedTea_Web))) return E_NOINTERFACE;
  // if ((*v = (LPVOID)CoTaskMemAlloc(sizeof (IIcedTea_Web))) == NULL)
    // return E_OUTOFMEMORY;
  // if (((**(IIcedTea_Web**)v).lpVtbl =
       // (IIcedTea_WebVtbl*)CoTaskMemAlloc(sizeof (privateIIcedTea_WebVtbl))) == NULL) {
    // CoTaskMemFree(*v);
    // *v = NULL;
    // return E_OUTOFMEMORY;
  // }
  // (*(**(IIcedTea_Web**)v).lpVtbl).QueryInterface = IcedTea_Web_QueryInterface;
  // (*(**(IIcedTea_Web**)v).lpVtbl).AddRef = IcedTea_Web_AddRef;
  // (*(**(IIcedTea_Web**)v).lpVtbl).Release = IcedTea_Web_Release;
  // IIcedTea_Web_refCount(*(IIcedTea_Web**)v) = 0u;
  // IIcedTea_Web_AddRef(*(IIcedTea_Web**)v);
  return S_OK;
}

HRESULT CALLBACK DllGetVersion(DLLVERSIONINFO2 *dllVerionInfo2)
{
  (*dllVerionInfo2).info1.dwMajorVersion = PACKAGE_MAJOR_VERSION;
  (*dllVerionInfo2).info1.dwMinorVersion = PACKAGE_MINOR_VERSION;
  (*dllVerionInfo2).info1.dwBuildNumber = PACKAGE_BUILD_VERSION;
  (*dllVerionInfo2).info1.dwPlatformID = DLLVER_PLATFORM_NT;
  if ((*dllVerionInfo2).info1.cbSize < sizeof (DLLVERSIONINFO2)) return S_OK;
  (*dllVerionInfo2).dwFlags = 0u;
  (*dllVerionInfo2).ullVersion = MAKEDLLVERULL(PACKAGE_MAJOR_VERSION,
                                               PACKAGE_MINOR_VERSION,
                                               PACKAGE_PATCH_VERSION,
                                               PACKAGE_BUILD_VERSION);
  return S_OK;
}

HRESULT DllInstall(BOOL install, PCWSTR cmdLine) {
  if (cmdLine != NULL) {
    HMODULE module = NULL;

    if ((module = GetModuleHandle(TEXT(PLUGIN_MODULE_NAME_STRING))) == NULL) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not get handle to module \"%s\""),
                 TEXT(PLUGIN_MODULE_NAME_STRING));
      return HRESULT_FROM_WIN32(lastError);
    }
    if (DialogBox((HINSTANCE)module,
                  MAKEINTRESOURCE(IDD_SYNTAX),
                  GetDesktopWindow(),
                  SyntaxDialogProc) <= 0) {
        DWORD lastError;

        LogMessage(lastError = GetLastError(),
                   TEXT("Could not load dialog resource from module \"%s\""),
                   TEXT(PLUGIN_MODULE_NAME_STRING));
        return HRESULT_FROM_WIN32(lastError);
    }
    LogMessage(ERROR_BAD_ARGUMENTS,
               TEXT("/I:\"%s\""),
               cmdLine);
    return HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS);
  }
  return S_OK;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
  if (hInstance == NULL) {
    LogMessage(ERROR_INVALID_HANDLE, TEXT("Could not load or unload DLL"));
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }
  if (!((reason == DLL_PROCESS_ATTACH) ^
        (reason == DLL_PROCESS_DETACH) ^
        (reason == DLL_THREAD_ATTACH) ^
        (reason == DLL_THREAD_DETACH))) {
    LogMessage(ERROR_BAD_ARGUMENTS, TEXT("Could not load or unload DLL"));
    SetLastError(ERROR_BAD_ARGUMENTS);
    return FALSE;
  }
  switch (reason) {
    case DLL_PROCESS_ATTACH:
      /* Try to register an event logging source, regardless whether it has
         yet been registered with the event logging service in the registry.
         This strategy allows for logging events into the event log even before
         an event source has been registered with the event logging service in
         the registry. */
      if ((eventSource =
           RegisterEventSource(NULL, TEXT(PACKAGE_NAME_STRING))) == NULL) {
        DWORD lastError;

        /* Issue a warning */
        LogMessage(-(lastError = GetLastError()),
                   TEXT("Could not register event logging source \"%s\""),
                   TEXT(PACKAGE_NAME_STRING));
        SetLastError(lastError);
        /* Do not return with an error here to give the process a chance to
           run anyway and do error reporting via standard streams */
      }
      /* Try setting the process' current user SID */
      SetCurrentUserSID();
      break;
    case DLL_PROCESS_DETACH:
      /* Try to deregister an event logging source possibly previously
         registered on DLL_PROCESS_ATTACH */
      if (eventSource != NULL) {
        if (DeregisterEventSource(eventSource) == FALSE) {
          DWORD lastError;

          /* Issue a warning */
          LogMessage(-(lastError = GetLastError()),
                     TEXT("Could not deregister event logging source \"%s\""),
                     TEXT(PACKAGE_NAME_STRING));
          SetLastError(lastError);
          /* Do not return with an error here. Give the process a chance to exit
            cleanly and let the OS reclaim living resources */
        } else {
          eventSource = NULL;
        }
      }
      HeapFree(GetProcessHeap(), 0, currentUserSID);
  }
  return TRUE;
}

HRESULT WINAPI DllRegisterServer() {
  HRESULT hResult = S_OK;

  if ((hResult = RegisterEventLogSource()) == S_OK) {
    HANDLE registerCOMClassThread = NULL;

    if ((registerCOMClassThread =
         CreateThread(NULL,
                      0x10000,
                      (LPTHREAD_START_ROUTINE)RegisterCOMClassThread,
                      NULL,
                      0,
                      NULL)) == NULL) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not create RegisterCOMClass thread"));
      return HRESULT_FROM_WIN32(lastError);
    }
    if ((hResult = RegisterNP()) != S_OK) {
      while (WaitForSingleObjectEx(registerCOMClassThread,
                                    INFINITE,
                                    TRUE) == WAIT_IO_COMPLETION);
      CloseHandle(registerCOMClassThread);
      return hResult;
    }
    while ((hResult = WaitForSingleObjectEx(registerCOMClassThread,
                                             INFINITE,
                                             TRUE)) == WAIT_IO_COMPLETION);
    if (hResult) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not wait for thread"));
      CloseHandle(registerCOMClassThread);
      return HRESULT_FROM_WIN32(lastError);
    }
    if (GetExitCodeThread(registerCOMClassThread,
                          (LPDWORD)&hResult) == FALSE) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not get RegisterCOMClass thread's return value"));
      CloseHandle(registerCOMClassThread);
      return HRESULT_FROM_WIN32(lastError);
    }
    if (hResult != S_OK) {
      CloseHandle(registerCOMClassThread);
      return hResult;
    }
    if (CloseHandle(registerCOMClassThread) == FALSE) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not close RegisterCOMClass thread"));
      return HRESULT_FROM_WIN32(lastError);
    }
    LogMessage(ERROR_SUCCESS,
               TEXT(PACKAGE_NAME_STRING " has been registered successfully."));
  }
  return hResult;
}

HRESULT WINAPI DllUnregisterServer() {
  HRESULT hResult = S_OK;
  HANDLE unregisterCOMClassThread = NULL;

  /* Create and start the UnregisterCOMClassThread() thread function */
  if ((unregisterCOMClassThread =
         CreateThread(NULL,
                      0x10000,
                      (LPTHREAD_START_ROUTINE)UnregisterCOMClassThread,
                      NULL,
                      0,
                      NULL)) == NULL) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not create UnregisterCOMClass thread"));
      return HRESULT_FROM_WIN32(lastError);
    }
    /* Concurrently, unregister with Mozilla compatible browsers */
    if ((hResult = UnregisterNP()) != S_OK) {
      /* Wait for UnregisterCOMClassThread() to finish while allowing
         asynchronous (overlapped) I/O operations to complete */
      while (WaitForSingleObjectEx(unregisterCOMClassThread, INFINITE, TRUE) ==
             WAIT_IO_COMPLETION);
      /* UnregisterCOMClassThread() has definitely finished so it is safe to
         close the handle */
      CloseHandle(unregisterCOMClassThread);
      return hResult;
    }
    /* Wait for UnregisterCOMClassThread() to finish while allowing
       asynchronous (overlapped) I/O operations to complete */
    while ((hResult = WaitForSingleObjectEx(unregisterCOMClassThread,
                                            INFINITE,
                                            TRUE)) == WAIT_IO_COMPLETION);
    /* From here on, UnregisterCOMClassThread() has definitely finished, hence
       it is safe to close the unregisterCOMClassThread handle. */
    /* Check if waiting for UnregisterCOMClassThread() to finish has caused an
       error */
    if (hResult) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not wait for UnregisterCOMClass thread"));
      CloseHandle(unregisterCOMClassThread);
      return HRESULT_FROM_WIN32(lastError);
    }
    /* Get the UnregisterCOMClassThread()'s HRESULT return value, perhaps
       indicating an error */
    if (GetExitCodeThread(unregisterCOMClassThread,
                          (LPDWORD)&hResult) == FALSE) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not get UnregisterCOMClass thread's return value"));
      CloseHandle(unregisterCOMClassThread);
      return HRESULT_FROM_WIN32(lastError);
    }
    /* Check UnregisterCOMClassThread()'s return value for error */
    if (hResult != S_OK) {
      CloseHandle(unregisterCOMClassThread);
      return hResult;
    }
    if (CloseHandle(unregisterCOMClassThread) == FALSE) {
      DWORD lastError;

      LogMessage(lastError = GetLastError(),
                 TEXT("Could not close RegisterCOMClass thread"));
      return HRESULT_FROM_WIN32(lastError);
    }
    /* Finally, unregister the event log source */
    if ((hResult = UnregisterEventLogSource()) != S_OK) return hResult;
    LogMessage(ERROR_SUCCESS,
               TEXT(PACKAGE_NAME_STRING " has been successfully unregistered."));
  return hResult;
}

#if !defined UNICODE || !defined _UNICODE
#define LPOLESTR2MBCS(lpOleStr) \
  { \
    char* mbGUIDString = NULL; \
    size_t len; \
    if ( \
         ( \
           mbGUIDString = (char*)CoTaskMemAlloc( \
             len = wcstombs( \
               NULL, \
               lpOleStr, \
               0 \
             ) + 1 \
           ) \
         ) == NULL \
      ) { \
      CoTaskMemFree(lpOleStr); \
      LogMessage(ERROR_OUTOFMEMORY, \
                 TEXT("Could not allocate memory for ANSI GUID string")); \
      return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY); \
    } \
    wcstombs(mbGUIDString, lpOleStr, len); \
    CoTaskMemFree(lpOleStr); \
    lpOleStr = (LPOLESTR)mbGUIDString; \
  }
#endif /* !UNICODE || !_UNICODE */
static HRESULT WINAPI RegisterCOMClass(void) {
  HRESULT hResult = S_OK;
  LONG lResult = 0LL;
  LPOLESTR GUIDString = NULL;
  TCHAR* moduleFileName = NULL;
  size_t len = 0;

  if ((moduleFileName = GetThisModuleFileName()) == NULL) {
    return HRESULT_FROM_WIN32(GetLastError());
  }
  {
    HKEY CLSIDKey = NULL;
    TCHAR* icedTeaWebCLSIDKeyString = NULL;

    /* Get CLSID_IcedTea_Web as string */
    if ((hResult = StringFromCLSID(&CLSID_IcedTea_Web, &GUIDString)) != S_OK) {
      LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                            HRESULT_FACILITY(hResult),
                            HRESULT_CODE(hResult)),
                 TEXT("Could not create string from CLSID"));
      return hResult;
    }
#if !defined UNICODE || !defined _UNICODE
LPOLESTR2MBCS(GUIDString)
#endif /* !UNICODE || !_UNICODE */
    /* Allocate memory for CLSID_IcedTea_Web registry key string */
    if ((icedTeaWebCLSIDKeyString =
#if defined UNICODE || defined _UNICODE
         (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("CLSID\\%s"),
                                                  (TCHAR*)GUIDString)) *
#else
         (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("CLSID\\%s"),
                                           (TCHAR*)GUIDString) *
#endif /* UNICODE || _UNICODE */
                                sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
      LogMessage(ERROR_OUTOFMEMORY,
                 TEXT("Could not allocate memory for registry key string"));
      CoTaskMemFree(GUIDString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
    }
    /* Format "CLSID\{CLSID_IcedTea_Web}" string */
    _stprintf(icedTeaWebCLSIDKeyString,
#if defined UNICODE || defined _UNICODE
              len + 1,
#endif /* UNICODE || _UNICODE */
              TEXT("CLSID\\%s"),
              (TCHAR*)GUIDString);
    /* Free GUIDString ({CLSID_IcedTea_Web}) because it is no longer needed */
    CoTaskMemFree(GUIDString);
    /* Create HKCR\CLSID\{CLSID_IcedTea_Web} registry subkey for writing */
    if ((lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
                                  icedTeaWebCLSIDKeyString,
                                  0,
                                  NULL,
                                  REG_OPTION_NON_VOLATILE,
                                  KEY_WRITE,
                                  NULL,
                                  &CLSIDKey,
                                  NULL)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not create registry key \"HKCR\\%s\""),
                 icedTeaWebCLSIDKeyString);
      CoTaskMemFree(icedTeaWebCLSIDKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Set HKCR\CLSID\{CLSID_IcedTea_Web} key's default value to PACKAGE_NAME */
    if ((lResult = RegSetValueEx(CLSIDKey,
                                 NULL,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)TEXT(PACKAGE_NAME_STRING),
                                 _tcslen(TEXT(PACKAGE_NAME_STRING)) *
                                 sizeof(TCHAR) +
                                 sizeof(TCHAR))) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                 icedTeaWebCLSIDKeyString,
                 TEXT(PACKAGE_NAME_STRING));
      RegCloseKey(CLSIDKey);
      CoTaskMemFree(icedTeaWebCLSIDKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      HKEY controlKey = NULL;
      TCHAR* controlKeyString = NULL;

      /* Allocate memory for CLSID\CLSID_IcedTea_Web\Control registry key string */
      if ((controlKeyString =
#if defined UNICODE || defined _UNICODE
           (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("%s\\%s"),
                                                    icedTeaWebCLSIDKeyString,
                                                    TEXT("Control"))) *
#else
           (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("%s\\%s"),
                                             icedTeaWebCLSIDKeyString,
                                             TEXT("Control")) *
#endif /* UNICODE || _UNICODE */
                                   sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      /* Format "CLSID\{CLSID_IcedTea_Web}\Control" string */
      _stprintf(controlKeyString,
#if defined UNICODE || defined _UNICODE
                len + 1,
#endif /* UNICODE || _UNICODE */
                TEXT("%s\\%s"),
                icedTeaWebCLSIDKeyString,
                TEXT("Control"));
      /* Create HKCR\CLSID\{CLSID_IcedTea_Web}\Control registry subkey to
       * declare this class an ActiveX Control */
      if ((lResult = RegCreateKeyEx(CLSIDKey,
                                    TEXT("Control"),
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &controlKey,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   controlKeyString);
        CoTaskMemFree(controlKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Close HKCR\CLSID\{CLSID_IcedTea_Web}\Control registry subkey */
      if ((lResult = RegCloseKey(controlKey)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   controlKeyString);
        CoTaskMemFree(controlKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free controlKeyString string because it is no longer needed */
      CoTaskMemFree(controlKeyString);
    }
    /* HKCR\CLSID\{CLSID_IcedTea_Web}\DefaultIcon */
    {
      HKEY clsidDefaultIconKey = NULL;
      const TCHAR* defaultIconKeyString = TEXT("DefaultIcon");
      TCHAR* defaultData = NULL;

      /* Create HKCR\CLSID\{CLSID_IcedTea_Web}\DefaultIcon registry subkey for writing */
      if ((lResult = RegCreateKeyEx(CLSIDKey,
                                    defaultIconKeyString,
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &clsidDefaultIconKey,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   defaultIconKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Allocate memory for "\"moduleFileName\",-ID_ICON" data string */
      if ((defaultData =
#if defined UNICODE || defined _UNICODE
           (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("\"%s\",-%u"),
                                                   moduleFileName,
                                                   ID_ICON)) *
#else
           (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("\"%s\",-%u"),
                                             moduleFileName,
                                             ID_ICON) *
#endif /* UNICODE || _UNICODE */
                                  sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        RegCloseKey(clsidDefaultIconKey);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      /* Format "\"moduleFileName\",-IDS_ICON" string */
       _stprintf(defaultData,
#if defined UNICODE || defined _UNICODE
                 len + 1,
#endif /* UNICODE || _UNICODE */
                 TEXT("\"%s\",-%u"),
                 moduleFileName,
                 ID_ICON);
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\DefaultIcon key's default value
         to "\"moduleFileName\",-ID_ICON" */
      if ((lResult = RegSetValueEx(clsidDefaultIconKey,
                                   NULL,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)defaultData,
                                   _tcslen(defaultData) * sizeof (TCHAR) +
                                   sizeof (TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                     TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                     defaultIconKeyString,
                     defaultData);
        CoTaskMemFree(defaultData);
        RegCloseKey(clsidDefaultIconKey);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free memory pointed to by defaultData because it is no longer needed */
      CoTaskMemFree(defaultData);
      /* Close HKCR\IcedTea.IcedTea-Web\DefaultIcon registry subkey */
      if ((lResult = RegCloseKey(clsidDefaultIconKey)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   defaultIconKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
    }
    {
      HKEY inprocServer32Key = NULL;
      TCHAR* inprocServer32KeyString = NULL;
      const TCHAR* threadingModelValue = TEXT("ThreadingModel"),
                 * threadingModelData = TEXT("Both");

      /* Allocate memory for CLSID\CLSID_IcedTea_Web\InprocServer32 registry key string */
      if ((inprocServer32KeyString =
#if defined UNICODE || defined _UNICODE
           (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("%s\\%s"),
                                                   icedTeaWebCLSIDKeyString,
                                                   TEXT("InprocServer32"))) *
#else
           (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("%s\\%s"),
                                  icedTeaWebCLSIDKeyString,
                                  TEXT("InprocServer32")) *
#endif /* UNICODE || _UNICODE */
                                  sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                     TEXT("Could not allocate memory for registry key string"));
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      /* Format "CLSID\{CLSID_IcedTea_Web}\InprocServer32" string */
      _stprintf(inprocServer32KeyString,
#if defined UNICODE || defined _UNICODE
                len + 1,
#endif /* UNICODE || _UNICODE */
                TEXT("%s\\%s"),
                icedTeaWebCLSIDKeyString,
                TEXT("InprocServer32"));
      /* Create HKCR\CLSID\{CLSID_IcedTea_Web}\InprocServer32 registry subkey for writing */
      if ((lResult = RegCreateKeyEx(CLSIDKey,
                                    TEXT("InprocServer32"),
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &inprocServer32Key,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                     TEXT("Could not create registry key \"HKCR\\%s\""),
                     inprocServer32KeyString);
        CoTaskMemFree(inprocServer32KeyString);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\InprocServer32 key's default value to
       * this module file's location */
      if ((lResult = RegSetValueEx(inprocServer32Key,
                                   NULL,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)moduleFileName,
                                   _tcslen(moduleFileName) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                   inprocServer32KeyString,
                   moduleFileName);
        RegCloseKey(inprocServer32Key);
        CoTaskMemFree(inprocServer32KeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\InprocServer32 key's "ThreadingModel"
       * value to "Both" indicate multiple IcedTea-Web plug-in instances */
      if ((lResult = RegSetValueEx(inprocServer32Key,
                                   threadingModelValue,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)threadingModelData,
                                   _tcslen(threadingModelData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\" to \"%s\""),
                   threadingModelValue,
                   inprocServer32KeyString,
                   threadingModelData);
        RegCloseKey(inprocServer32Key);
        CoTaskMemFree(inprocServer32KeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Close HKCR\CLSID\{CLSID_IcedTea_Web}\InprocServer32 registry subkey */
      if ((lResult = RegCloseKey(inprocServer32Key)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   inprocServer32KeyString);
        CoTaskMemFree(inprocServer32KeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free memory pointed to by inprocServer32KeyString because it is no longer needed */
      CoTaskMemFree(inprocServer32KeyString);
    }
    {
      HKEY inprocHandler32Key = NULL;
      TCHAR* inprocHandler32KeyString = NULL;
      const TCHAR* defaultData = TEXT("OLE32.DLL");

      /* Allocate memory for CLSID\CLSID_IcedTea_Web\InprocHandler32 registry key string */
      if ((inprocHandler32KeyString =
#if defined UNICODE || defined _UNICODE
           (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("%s\\%s"),
                                                    icedTeaWebCLSIDKeyString,
                                                    TEXT("InprocHandler32"))) *
#else
           (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("%s\\%s"),
                                             icedTeaWebCLSIDKeyString,
                                             TEXT("InprocHandler32")) *
#endif /* UNICODE || _UNICODE */
                                  sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_SEVERITY(ERROR_OUTOFMEMORY);
      }
      /* Format "CLSID\{CLSID_IcedTea_Web}\InprocHandler32" string */
      _stprintf(inprocHandler32KeyString,
#if defined UNICODE || defined _UNICODE
                len + 1,
#endif /* UNICODE || _UNICODE */
                TEXT("%s\\%s"),
                icedTeaWebCLSIDKeyString,
                TEXT("InprocHandler32"));
      /* Create HKCR\CLSID\{CLSID_IcedTea_Web}\InprocHandler32 registry subkey for writing */
      if ((lResult = RegCreateKeyEx(CLSIDKey,
                                    TEXT("InprocHandler32"),
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &inprocHandler32Key,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   inprocHandler32KeyString);
        CoTaskMemFree(inprocHandler32KeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\InprocHandler32 key's default value to
       * "OLE32.DLL" to use the default COM handler */
      if ((lResult = RegSetValueEx(inprocHandler32Key,
                                   NULL,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)defaultData,
                                   _tcslen(defaultData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                   inprocHandler32KeyString,
                   defaultData);
        RegCloseKey(inprocHandler32Key);
        CoTaskMemFree(inprocHandler32KeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Close HKCR\CLSID\{CLSID_IcedTea_Web}\InprocHandler32 registry subkey */
      if ((lResult = RegCloseKey(inprocHandler32Key)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   inprocHandler32KeyString);
        CoTaskMemFree(inprocHandler32KeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free the memory for inprocHandler32KeyString string because it is no longer needed */
      CoTaskMemFree(inprocHandler32KeyString);
    }
    {
      HKEY interfaceKey = NULL;
      TCHAR* interfaceKeyString = NULL;
      const TCHAR* iUnknownData = TEXT("IUnknown"),
                 * iOleObjectData = TEXT("IOleObject"),
                 * iIcedTeaWebData = TEXT("IIcedTea_Web");

      /* Allocate memory for CLSID\CLSID_IcedTea_Web\Interface registry key string */
      if ((interfaceKeyString =
#if defined UNICODE || defined _UNICODE
           (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("%s\\%s"),
                                                    icedTeaWebCLSIDKeyString,
                                                    TEXT("Interface"))) *
#else
           (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("%s\\%s"),
                                  icedTeaWebCLSIDKeyString,
                                  TEXT("Interface")) *
#endif /* UNICODE || _UNICODE */
                                  sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      /* Format "CLSID\{CLSID_IcedTea_Web}\Interface" string */
      _stprintf(interfaceKeyString,
#if defined UNICODE || defined _UNICODE
                len + 1,
#endif /* UNICODE || _UNICODE */
                TEXT("%s\\%s"),
                icedTeaWebCLSIDKeyString,
                TEXT("Interface"));
      /* Create HKCR\CLSID\{CLSID_IcedTea_Web}\Interface registry subkey for writing */
      if ((lResult = RegCreateKeyEx(CLSIDKey,
                                    TEXT("Interface"),
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &interfaceKey,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   interfaceKeyString);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Convert IID_IUnknown to string */
      if ((hResult = StringFromIID(&IID_IUnknown, &GUIDString)) != S_OK) {
        LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                              HRESULT_FACILITY(hResult),
                              HRESULT_CODE(hResult)),
                   TEXT("Could not create string from IID"));
        RegCloseKey(interfaceKey);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return hResult;
      }
#if !defined UNICODE || !defined _UNICODE
LPOLESTR2MBCS(GUIDString)
#endif /* !UNICODE || !_UNICODE */
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\Interface key's "{IID_IUnknown}"
       * value to "IUnknown" */
      if ((lResult = RegSetValueEx(interfaceKey,
                                   (TCHAR*)GUIDString,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)iUnknownData,
                                   _tcslen(iUnknownData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\" to \"%s\""),
                   (TCHAR*)GUIDString,
                   interfaceKeyString,
                   iUnknownData);
        CoTaskMemFree(GUIDString);
        RegCloseKey(interfaceKey);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free the memory for "{IID_IUnknown}" string because it is no longer needed */
      CoTaskMemFree(GUIDString);
      /* Convert IID_IOleObject to string */
      if ((hResult = StringFromIID(&IID_IOleObject, &GUIDString)) != S_OK) {
        LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                              HRESULT_FACILITY(hResult),
                              HRESULT_CODE(hResult)),
                   TEXT("Could not create string from IID"));
        RegCloseKey(interfaceKey);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return hResult;
      }
#if !defined UNICODE || !defined _UNICODE
LPOLESTR2MBCS(GUIDString)
#endif /* !UNICODE || !_UNICODE */
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\Interface key's "{IID_IOleObject}"
       * value to "IUnknown" */
      if ((lResult = RegSetValueEx(interfaceKey,
                                   (TCHAR*)GUIDString,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)iOleObjectData,
                                   _tcslen(iOleObjectData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\" to \"%s\""),
                   (TCHAR*)GUIDString,
                   interfaceKeyString,
                   iOleObjectData);
        CoTaskMemFree(GUIDString);
        RegCloseKey(interfaceKey);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free the memory for "{IID_IOleObject}" string because it is no longer needed */
      CoTaskMemFree(GUIDString);
      /* Convert IID_IIcedTea_Web to string */
      if ((hResult = StringFromIID(&IID_IIcedTea_Web, &GUIDString)) != S_OK) {
        LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                              HRESULT_FACILITY(hResult),
                              HRESULT_CODE(hResult)),
                   TEXT("Could not create string from IID"));
        RegCloseKey(interfaceKey);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return hResult;
      }
#if !defined UNICODE || !defined _UNICODE
LPOLESTR2MBCS(GUIDString)
#endif /* !UNICODE || !_UNICODE */
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\Interface key's "{IID_IIcedTea_Web}"
       * value to "IIcedTea_Web" */
      if ((lResult = RegSetValueEx(interfaceKey,
                                   (TCHAR*)GUIDString,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)iIcedTeaWebData,
                                   _tcslen(iIcedTeaWebData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\" to \"%s\""),
                   (TCHAR*)GUIDString,
                   interfaceKeyString,
                   iIcedTeaWebData);
        CoTaskMemFree(GUIDString);
        RegCloseKey(interfaceKey);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free the memory for "{IID_IIcedTea_Web}" string because it is no longer needed */
      CoTaskMemFree(GUIDString);
      /* Close HKCR\CLSID\{CLSID_IcedTea_Web}\Interface registry subkey */
      if ((lResult = RegCloseKey(interfaceKey)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   interfaceKeyString);
        CoTaskMemFree(interfaceKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free the memory for interfaceKeyString string because it is no longer needed */
      CoTaskMemFree(interfaceKeyString);
    }
    {
      HKEY versionKey = NULL;
      TCHAR* versionKeyString = NULL;
      const TCHAR* defaultData = TEXT(PACKAGE_VERSION_STRING);

      /* Allocate memory for CLSID\CLSID_IcedTea_Web\Version registry key string */
      if ((versionKeyString =
#if defined UNICODE || defined _UNICODE
           (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("%s\\%s"),
                                                   icedTeaWebCLSIDKeyString,
                                                   TEXT("Version"))) *
#else
           (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("%s\\%s"),
                                  icedTeaWebCLSIDKeyString,
                                  TEXT("Version")) *
#endif /* UNICODE || _UNICODE */
                                  sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      /* Format "CLSID\{CLSID_IcedTea_Web}\Version" string */
      _stprintf(versionKeyString,
#if defined UNICODE || defined _UNICODE
                len + 1,
#endif /* UNICODE || _UNICODE */
                TEXT("%s\\%s"),
                icedTeaWebCLSIDKeyString,
                TEXT("Version"));
      /* Create HKCR\CLSID\{CLSID_IcedTea_Web}\Version registry subkey for writing */
      if ((lResult = RegCreateKeyEx(CLSIDKey,
                                    TEXT("Version"),
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &versionKey,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   versionKeyString);
        CoTaskMemFree(versionKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Set HKCR\CLSID\{CLSID_IcedTea_Web}\Version key's default value to
       * PACKAGE_VERSION.
       * TODO: Make the version string be read out from the module file version
       * information instead of the macro definition PACKAGE_VERSION */
      if ((lResult = RegSetValueEx(versionKey,
                                   NULL,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)defaultData,
                                   _tcslen(defaultData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                   versionKeyString,
                   defaultData);
        RegCloseKey(versionKey);
        CoTaskMemFree(versionKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Close HKCR\CLSID\{CLSID_IcedTea_Web}\Version registry subkey */
      if ((lResult = RegCloseKey(versionKey)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   versionKeyString);
        CoTaskMemFree(versionKeyString);
        RegCloseKey(CLSIDKey);
        CoTaskMemFree(icedTeaWebCLSIDKeyString);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free the memory for versionKeyString string because it is no longer needed */
      CoTaskMemFree(versionKeyString);
    }
    /* Close HKCR\CLSID\{CLSID_IcedTea_Web} registry subkey */
    if ((lResult = RegCloseKey(CLSIDKey)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not close registry key \"HKCR\\%s\""),
                 icedTeaWebCLSIDKeyString);
      CoTaskMemFree(icedTeaWebCLSIDKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Free icedTeaWebCLSIDKeyString string because it is no longer needed */
    CoTaskMemFree(icedTeaWebCLSIDKeyString);
  }
  {
    HKEY dotJNLPKey = NULL;
    const TCHAR* dotJNLPKeyString = TEXT(".jnlp"),
               * progIDData = TEXT(PACKAGE_VENDOR_STRING "."
                                   PACKAGE_NAME_STRING),
               * contentTypeValue = TEXT("Content Type"),
               * contentTypeData = TEXT("application/x-java-jnlp-file"),
               * perceivedTypeValue = TEXT("PerceivedType"),
               * perceivedTypeData = TEXT("text");

    /* Create HKCR\.jnlp registry subkey for writing */
    if ((lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
                                  dotJNLPKeyString,
                                  0,
                                  NULL,
                                  REG_OPTION_NON_VOLATILE,
                                  KEY_WRITE,
                                  NULL,
                                  &dotJNLPKey,
                                  NULL)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not create registry key \"HKCR\\%s\""),
                 dotJNLPKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Set HKCR\.jnlp key's default value to "IcedTea.IcedTea-Web" */
    if ((lResult = RegSetValueEx(dotJNLPKey,
                                 NULL,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)progIDData,
                                 _tcslen(progIDData) * sizeof(TCHAR) +
                                 sizeof(TCHAR))) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                 dotJNLPKeyString,
                 progIDData);
      RegCloseKey(dotJNLPKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Set HKCR\.jnlp key's value "Content Type" to "application/x-java-jnlp-file" */
    if ((lResult = RegSetValueEx(dotJNLPKey,
                                 contentTypeValue,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)contentTypeData,
                                 _tcslen(contentTypeData) * sizeof(TCHAR) +
                                 sizeof(TCHAR))) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\" to \"%s\""),
                 contentTypeValue,
                 dotJNLPKeyString,
                 contentTypeData);
      RegCloseKey(dotJNLPKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Set HKCR\.jnlp key's value "PerceivedType" to "text" */
    if ((lResult = RegSetValueEx(dotJNLPKey,
                                 perceivedTypeValue,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)perceivedTypeData,
                                 _tcslen(perceivedTypeData) * sizeof(TCHAR) +
                                 sizeof(TCHAR))) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\" to \"%s\""),
                 perceivedTypeValue,
                 dotJNLPKeyString,
                 perceivedTypeData);
      RegCloseKey(dotJNLPKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Close HKCR\.jnlp registry subkey */
    if ((lResult = RegCloseKey(dotJNLPKey)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not close registry key \"HKCR\\%s\""),
                 dotJNLPKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Refresh the shell after registering the file association */
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
  }
  /* Register version independent ProgID */
  {
    HKEY progIDKey = NULL;
    const TCHAR* progIDKeyString = TEXT(PACKAGE_VENDOR_STRING "."
                                        PACKAGE_NAME_STRING),
               * defaultData = TEXT("Java™ Network Launch Protocol");

    /* Create HKCR\IcedTea.IcedTea-Web (version independent ProgID) registry
     * subkey for writing */
    if ((lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
                                  progIDKeyString,
                                  0,
                                  NULL,
                                  REG_OPTION_NON_VOLATILE,
                                  KEY_WRITE,
                                  NULL,
                                  &progIDKey,
                                  NULL)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not create registry key \"HKCR\\%s\""),
                 progIDKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Set HKCR\IcedTea.IcedTea-Web key's default value to "Java\u2122 Network
     * Launch Protocol" */
    if ((lResult = RegSetValueEx(progIDKey,
                                 NULL,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)defaultData,
                                 _tcslen(defaultData) * sizeof(TCHAR) +
                                 sizeof(TCHAR))) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                 progIDKeyString,
                 defaultData);
      RegCloseKey(progIDKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      HKEY progIDCurVerKey = NULL;
      const TCHAR* curVerKeyString = TEXT("CurVer"),
                 * defaultData = TEXT(PACKAGE_VENDOR_STRING "."
                                      PACKAGE_NAME_STRING "."
                                      PACKAGE_VERSION_STRING),
                 * formatString = TEXT("%s\\%s");
      TCHAR* fullKeyString;

      /* Allocate memory for "PACKAGE_VENDOR.PACKAGE_NAME\CurVer" registry key
         string */
      if ((fullKeyString =
#if defined UNICODE || defined _UNICODE
           CoTaskMemAlloc((len = _sctprintf(formatString,
                                            progIDKeyString,
                                            progIDCurVerKey)) *
#else
           CoTaskMemAlloc(_sctprintf(formatString,
                                     progIDKeyString,
                                     progIDCurVerKey) *
#endif /* defined UNICODE || defined _UNICODE */
                          sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
      }
      _stprintf(fullKeyString,
#if defined UNICODE || defined _UNICODE
                len + 1,
#endif /* defined UNICODE || defined _UNICODE */
                formatString,
                progIDKeyString,
                progIDCurVerKey);
      /* Create HKCR\IcedTea.IcedTea-Web\CurVer registry subkey for writing */
      if ((lResult = RegCreateKeyEx(progIDKey,
                                    curVerKeyString,
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &progIDCurVerKey,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   fullKeyString);
        CoTaskMemFree(fullKeyString);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Set HKCR\IcedTea.IcedTea-Web\CurVer key's default value to
       * "PACKAGE_VENDOR.PACKAGE_NAME.PACKAGE_VERSION" */
      if ((lResult = RegSetValueEx(progIDCurVerKey,
                                   NULL,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)defaultData,
                                   _tcslen(defaultData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                   fullKeyString,
                   defaultData);
        CoTaskMemFree(fullKeyString);
        RegCloseKey(progIDCurVerKey);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Close HKCR\IcedTea.IcedTea-Web\CurVer registry subkey */
      if ((lResult = RegCloseKey(progIDCurVerKey)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   fullKeyString);
        CoTaskMemFree(fullKeyString);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
    }
    /* Close HKCR\IcedTea.IcedTea-Web registry subkey */
    if ((lResult = RegCloseKey(progIDKey)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   progIDKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
  }
  /* Register version dependent ProgID */
  {
    HKEY progIDKey = NULL;
    const TCHAR* progIDKeyString = TEXT(PACKAGE_VENDOR_STRING "."
                                        PACKAGE_NAME_STRING "."
                                        PACKAGE_VERSION_STRING),
               * defaultData = TEXT("Java\u2122 Network Launch Protocol"),
               * friendlyTypeNameValue = TEXT("FriendlyTypeName"),
               * neverShowExtValue = TEXT("NeverShowExt");
    TCHAR*       friendlyTypeNameData = NULL;
    /* Create HKCR\IcedTea.IcedTea-Web.PACKAGE_VERSION registry subkey for writing */
    if ((lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
                                  progIDKeyString,
                                  0,
                                  NULL,
                                  REG_OPTION_NON_VOLATILE,
                                  KEY_WRITE,
                                  NULL,
                                  &progIDKey,
                                  NULL)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not create registry key \"HKCR\\%s\""),
                 progIDKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Set HKCR\IcedTea.IcedTea-Web.PACKAGE_VERSION key's default value to
     * "Java\u2122 Network Launch Protocol" */
    if ((lResult = RegSetValueEx(progIDKey,
                                 NULL,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)defaultData,
                                 _tcslen(defaultData) * sizeof(TCHAR) +
                                 sizeof(TCHAR))) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                 progIDKeyString,
                 defaultData);
      RegCloseKey(progIDKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Allocate memory for "@\"moduleFileName\",-IDS_FRIENDLYTYPENAME" data string */
    if ((friendlyTypeNameData =
#if defined UNICODE || defined _UNICODE
         (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("@\"%s\",-%u"),
                                moduleFileName,
                                IDS_FRIENDLYTYPENAME)) *
#else
         (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("@\"%s\",-%u"),
                                           moduleFileName,
                                           IDS_FRIENDLYTYPENAME) *
#endif /* UNICODE || _UNICODE */
                                sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
      LogMessage(ERROR_OUTOFMEMORY,
                 TEXT("Could not allocate memory for registry key string"));
      RegCloseKey(progIDKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
    }
    /* Format "@\"moduleFileName\",-IDS_FRIENDLYTYPENAME" string */
     _stprintf(friendlyTypeNameData,
#if defined UNICODE || defined _UNICODE
                    len + 1,
#endif /* UNICODE || _UNICODE */
               TEXT("@\"%s\",-%u"),
               moduleFileName,
               IDS_FRIENDLYTYPENAME);
    /* Set HKCR\IcedTea.IcedTea-Web key's value "FriendlyTypeName" to
       "@\"moduleFileName\",-IDS_FRIENDLYTYPENAME" */
    if ((lResult = RegSetValueEx(progIDKey,
                                 friendlyTypeNameValue,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)friendlyTypeNameData,
                                 _tcslen(friendlyTypeNameData) * sizeof(TCHAR) +
                                 sizeof(TCHAR))) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\" to \"%s\""),
                 friendlyTypeNameValue,
                 progIDKeyString,
                 friendlyTypeNameData);
      CoTaskMemFree(friendlyTypeNameData);
      RegCloseKey(progIDKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Free memory pointed to by friendlyTypeNameData because it is no longer needed */
    CoTaskMemFree(friendlyTypeNameData);
    /* Set HKCR\IcedTea.IcedTea-Web key's value "NeverShowExt" */
    if ((lResult = RegSetValueEx(progIDKey,
                                 neverShowExtValue,
                                 0,
                                 REG_SZ,
                                 (const BYTE*)NULL,
                                 0)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not set value \"%s\" in registry key \"HKCR\\%s\""),
                 neverShowExtValue,
                 progIDKeyString);
      RegCloseKey(progIDKey);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      HKEY progIDCLSIDKey = NULL;
      const TCHAR* clsidKeyString = TEXT("CLSID");

      /* Create HKCR\IcedTea.IcedTea-Web.PACKAGE_VERSION\CLSID registry subkey for writing */
      if ((lResult = RegCreateKeyEx(progIDKey,
                                    clsidKeyString,
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &progIDCLSIDKey,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   clsidKeyString);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Get CLSID_IcedTea_Web as string */
      if ((hResult = StringFromCLSID(&CLSID_IcedTea_Web, &GUIDString)) != S_OK) {
        LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                              HRESULT_FACILITY(hResult),
                              HRESULT_CODE(hResult)),
                   TEXT("Could not create string from CLSID"));
        RegCloseKey(progIDCLSIDKey);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return hResult;
      }
#if !defined UNICODE || !defined _UNICODE
      LPOLESTR2MBCS(GUIDString)
#endif
      /* Set HKCR\IcedTea.IcedTea-Web.PACKAGE_VERSION\CLSID key's default value
       * to "{CLSID_IcedTea_Web}" */
      if ((lResult = RegSetValueEx(progIDCLSIDKey,
                                   NULL,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)GUIDString,
                                   _tcslen((TCHAR*)GUIDString) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                   clsidKeyString,
                   (TCHAR*)GUIDString);
        CoTaskMemFree(GUIDString);
        RegCloseKey(progIDCLSIDKey);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free memory pointed to by GUIDString because it is no longer needed */
      CoTaskMemFree(GUIDString);
      /* Close HKCR\IcedTea.IcedTea-Web\CLSID registry subkey */
      if ((lResult = RegCloseKey(progIDCLSIDKey)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   clsidKeyString);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
    }
    {
      HKEY progIDDefaultIconKey = NULL;
      const TCHAR* defaultIconKeyString = TEXT("DefaultIcon");
      TCHAR* defaultData = NULL;

      /* Create HKCR\IcedTea.IcedTea-Web\DefaultIcon registry subkey for writing */
      if ((lResult = RegCreateKeyEx(progIDKey,
                                    defaultIconKeyString,
                                    0,
                                    NULL,
                                    REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL,
                                    &progIDDefaultIconKey,
                                    NULL)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not create registry key \"HKCR\\%s\""),
                   defaultIconKeyString);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Allocate memory for "\"moduleFileName\",-ID_ICON" data string */
      if ((defaultData =
#if defined UNICODE || defined _UNICODE
           (TCHAR*)CoTaskMemAlloc((len = _sctprintf(TEXT("\"%s\",-%u"),
                                                    moduleFileName,
                                                    ID_ICON)) *
#else
           (TCHAR*)CoTaskMemAlloc(_sctprintf(TEXT("\"%s\",-%u"),
                                             moduleFileName,
                                             ID_ICON) *
#endif /* UNICODE || _UNICODE */
                                  sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        RegCloseKey(progIDDefaultIconKey);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      /* Format "\"moduleFileName\",-IDS_ICON" string */
       _stprintf(defaultData,
#if defined UNICODE || defined _UNICODE
                 len + 1,
#endif /* UNICODE || _UNICODE */
                 TEXT("\"%s\",-%u"),
                 moduleFileName,
                 ID_ICON);
      /* Set HKCR\IcedTea.IcedTea-Web\DefaultIcon key's default value
         to "\"moduleFileName\",-ID_ICON" */
      if ((lResult = RegSetValueEx(progIDDefaultIconKey,
                                   NULL,
                                   0,
                                   REG_SZ,
                                   (const BYTE*)defaultData,
                                   _tcslen(defaultData) * sizeof(TCHAR) +
                                   sizeof(TCHAR))) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not set default value in registry key \"HKCR\\%s\" to \"%s\""),
                   defaultIconKeyString,
                   defaultData);
        CoTaskMemFree(defaultData);
        RegCloseKey(progIDDefaultIconKey);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
      /* Free memory pointed to by defaultData because it is no longer needed */
      CoTaskMemFree(defaultData);
      /* Close HKCR\IcedTea.IcedTea-Web\DefaultIcon registry subkey */
      if ((lResult = RegCloseKey(progIDDefaultIconKey)) != ERROR_SUCCESS) {
        LogMessage(lResult,
                   TEXT("Could not close registry key \"HKCR\\%s\""),
                   defaultIconKeyString);
        RegCloseKey(progIDKey);
        CoTaskMemFree(moduleFileName);
        return HRESULT_FROM_WIN32(lResult);
      }
    }
    /* Close HKCR\IcedTea.IcedTea-Web.PACKAGE_VERSION registry subkey */
    if ((lResult = RegCloseKey(progIDKey)) != ERROR_SUCCESS) {
      LogMessage(lResult,
                 TEXT("Could not close registry key \"HKCR\\%s\""),
                 progIDKeyString);
      CoTaskMemFree(moduleFileName);
      return HRESULT_FROM_WIN32(lResult);
    }
  }
  /* Free memory pointed to by moduleFileName because it is no longer needed */
  CoTaskMemFree(moduleFileName);
  return S_OK;
}

static DWORD WINAPI RegisterCOMClassThread(LPVOID arg) {
  return (DWORD)RegisterCOMClass();
}

static HRESULT WINAPI RegisterEventLogSource() {
  LONG lResult = 0;
  HKEY eventSourceKey = NULL;
  const TCHAR* eventSourceKeyString = TEXT("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\" PACKAGE_NAME_STRING),
             * typesSupportedValue = TEXT("TypesSupported"),
             * eventMessageFileValue = TEXT("EventMessageFile");
  TCHAR* eventMessageFileData = NULL;
  const DWORD typesSupportedData = EVENTLOG_ERROR_TYPE |
                                   EVENTLOG_WARNING_TYPE |
                                   EVENTLOG_INFORMATION_TYPE;

  /* Create HKLM\SYSTEM\CurrentControlSet\Services\Eventlog\Application\PACKAGE_NAME
     registry subkey for writing */
  if ((lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                                eventSourceKeyString,
                                0,
                                NULL,
                                REG_OPTION_NON_VOLATILE,
                                KEY_WRITE,
                                NULL,
                                &eventSourceKey,
                                NULL)) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not create registry key \"HKLM\\%s\""),
               eventSourceKeyString);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Set value "TypesSupported" in
     HKLM\SYSTEM\CurrentControlSet\Services\Eventlog\Application\PACKAGE_NAME
     registry key to 0x00000007 */
  if ((lResult = RegSetValueEx(eventSourceKey,
                               typesSupportedValue,
                               0,
                               REG_DWORD,
                               (const BYTE*)&typesSupportedData,
                               sizeof (DWORD))) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not set value \"%s\" in registry key \"HKLM\\%s\" to \"0x%08X\""),
               typesSupportedValue,
               eventSourceKeyString,
               typesSupportedData);
    RegCloseKey(eventSourceKey);
    return HRESULT_FROM_WIN32(lResult);
  }
  if ((eventMessageFileData = GetThisModuleFileName()) == NULL) {
    DWORD lastError = GetLastError();
    RegCloseKey(eventSourceKey);
    return HRESULT_FROM_WIN32(lastError);
  }
  /* Set value "EventMessageFile" in
     HKLM\SYSTEM\CurrentControlSet\Services\Eventlog\Application\PACKAGE_NAME
     registry key to this module's absolute location */
  if ((lResult = RegSetValueEx(eventSourceKey,
                               eventMessageFileValue,
                               0,
                               REG_SZ,
                               (const BYTE*)eventMessageFileData,
                               _tcslen(eventMessageFileData) *
                                 sizeof (TCHAR) +
                                 sizeof (TCHAR))) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not set value \"%s\" in registry key \"HKLM\\%s\" to \"%s\""),
               eventMessageFileValue,
               eventSourceKeyString,
               NULL);
    CoTaskMemFree(eventMessageFileData);
    RegCloseKey(eventSourceKey);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Free memory pointed to by eventMessageFileData because it is no longer needed */
  CoTaskMemFree(eventMessageFileData);
  /* Close HKLM\SYSTEM\CurrentControlSet\Services\Eventlog\Application\PACKAGE_NAME
     registry key */
  if ((lResult = RegCloseKey(eventSourceKey)) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not close registry key \"HKLM\\%s\""),
               eventSourceKeyString);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Deregister the preliminary registered event logging source */
  if (eventSource != NULL) {
    if (DeregisterEventSource(eventSource) == FALSE) {
        DWORD lastError;

        LogMessage(lastError = GetLastError(),
                   TEXT("Could not deregister event logging source \"%s\""),
                   TEXT(PACKAGE_NAME_STRING));
        return HRESULT_FROM_WIN32(lastError);
    }
    eventSource = NULL;
  }
  /* Reregister event source after it has been registered with the event logging
     service in the registry (this time it is for real) ;-) */
  if ((eventSource = RegisterEventSource(NULL, TEXT(PACKAGE_NAME_STRING))) ==
      NULL) {
    DWORD lastError;

    LogMessage(lastError = GetLastError(),
               TEXT("Could not register event logging source \"%s\""),
               TEXT(PACKAGE_NAME_STRING));
    return HRESULT_FROM_WIN32(lastError);
  }
  return S_OK;
}

static HRESULT WINAPI RegisterNP() {
/* TODO: Add user only registration */
  LONG lResult = ERROR_SUCCESS;
  HKEY icedTeaWebKey = NULL;
  const TCHAR* icedTeaWebKeyString = TEXT("SOFTWARE\\MozillaPlugins\\@"
                                          PACKAGE_VENDOR_STRING "/"
                                          PACKAGE_NAME_STRING ",version="
                                          PACKAGE_VERSION_STRING),
             * descriptionValue = TEXT("Description"),
             * descriptionData = TEXT(PACKAGE_DESCRIPTION_STRING),
             * pathValue = TEXT("Path"),
             * productNameValue = TEXT("ProductName"),
             * productNameData = TEXT(PACKAGE_NAME_STRING),
             * vendorValue = TEXT("Vendor"),
             * vendorData = TEXT(PACKAGE_VENDOR_STRING),
             * versionValue = TEXT("Version"),
             * versionData = TEXT(PACKAGE_VERSION_STRING);
  TCHAR* pathData = NULL;

  /* Create HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME registry key */
  if ((lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                                icedTeaWebKeyString,
                                0,
                                NULL,
                                REG_OPTION_NON_VOLATILE,
                                KEY_WRITE,
                                NULL,
                                &icedTeaWebKey,
                                NULL)) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not create registry key \"HKLM\\%s\""),
               icedTeaWebKeyString);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Set HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME\Description
     value to PACKAGE_DESCRIPTION */
  if ((lResult = RegSetValueEx(icedTeaWebKey,
                               descriptionValue,
                               0,
                               REG_SZ,
                               (const BYTE*)descriptionData,
                               _tcslen(descriptionData) *
                                 sizeof (TCHAR) +
                                 sizeof (TCHAR))) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not set value \"%s\" in registry key \"HKLM\\%s\" to \"%s\""),
               descriptionValue,
               icedTeaWebKeyString,
               descriptionData);
    RegCloseKey(icedTeaWebKey);
    return HRESULT_FROM_WIN32(lResult);
  }
  if ((pathData = GetThisModuleFileName()) == NULL)
    return HRESULT_FROM_WIN32(GetLastError());
  /* Set HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME\Path value to
     this module's file name location */
  if ((lResult = RegSetValueEx(icedTeaWebKey,
                               pathValue,
                               0,
                               REG_SZ,
                               (const BYTE*)pathData,
                               _tcslen(pathData) *
                                 sizeof (TCHAR) +
                                 sizeof (TCHAR))) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not set value \"%s\" in registry key \"HKLM\\%s\" to \"%s\""),
               pathValue,
               icedTeaWebKeyString,
               pathData);
    CoTaskMemFree(pathData);
    RegCloseKey(icedTeaWebKey);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Free the memory pointed to by pathData because it is no longer needed */
  CoTaskMemFree(pathData);
  /* Set HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME\ProductName
     value to PACKAGE_NAME */
  if ((lResult = RegSetValueEx(icedTeaWebKey,
                               productNameValue,
                               0,
                               REG_SZ,
                               (const BYTE*)productNameData,
                               _tcslen(productNameData) *
                                 sizeof (TCHAR) +
                                 sizeof (TCHAR))) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not set value \"%s\" in registry key \"HKLM\\%s\" to \"%s\""),
               productNameValue,
               icedTeaWebKeyString,
               productNameData);
    RegCloseKey(icedTeaWebKey);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Set HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME\Vendor value
     to PACKAGE_VENDOR */
  if ((lResult = RegSetValueEx(icedTeaWebKey,
                               vendorValue,
                               0,
                               REG_SZ,
                               (const BYTE*)vendorData,
                               _tcslen(vendorData) *
                                 sizeof (TCHAR) +
                                 sizeof (TCHAR))) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not set value \"%s\" in registry key \"HKLM\\%s\" to \"%s\""),
               vendorValue,
               icedTeaWebKeyString,
               vendorData);
    RegCloseKey(icedTeaWebKey);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Set HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME\Version value
     to PACKAGE_VENDOR */
  if ((lResult = RegSetValueEx(icedTeaWebKey,
                               versionValue,
                               0,
                               REG_SZ,
                               (const BYTE*)versionData,
                               _tcslen(versionData) *
                                 sizeof (TCHAR) +
                                 sizeof (TCHAR))) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not set value \"%s\" in registry key \"HKLM\\%s\" to \"%s\""),
               versionValue,
               icedTeaWebKeyString,
               versionData);
    RegCloseKey(icedTeaWebKey);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Close HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME registry
     key */
  if ((lResult = RegCloseKey(icedTeaWebKey)) != ERROR_SUCCESS) {
    LogMessage(lResult,
               TEXT("Could not close registry key \"HKLM\\%s\""),
               icedTeaWebKeyString);
    return HRESULT_FROM_WIN32(lResult);
  }
  return S_OK;
}

static HRESULT WINAPI UnregisterCOMClass() {
/* TODO: Delete only actually created registry keys and values, in order to
   spare any registry sub-keys and values possibly created by the user.*/
  LONG lResult;

  /* Delete HKCR\.jnlp registry key */
  {
    const TCHAR* dotjnlpKeyString = TEXT(".jnlp");

    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                dotjnlpKeyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {
      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 dotjnlpKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Refresh the shell after unregistering the file association */
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
  }
  /* TODO: Modify to delete only this version's ProgID registry key when other
     version ProgIDs still exist. And, redirect the version independent ProgID
     to the latest registered IcedTea-Web class and ProgID version.
   /* Delete ProgID registry keys. */
  {
    const TCHAR
               * versionProgIDKeyString =
                   TEXT(PACKAGE_VENDOR_STRING "." PACKAGE_NAME_STRING "."
                        PACKAGE_VERSION_STRING),
               * versionProgIDCLSIDKeyString =
                   TEXT(PACKAGE_VENDOR_STRING "." PACKAGE_NAME_STRING "."
                        PACKAGE_VERSION_STRING "\\CLSID"),
               * versionProgIDDefaultIconKeyString =
                 TEXT(PACKAGE_VENDOR_STRING "." PACKAGE_NAME_STRING "."
                      PACKAGE_VERSION_STRING "\\DefaultIcon"),
               * progIDKeyString = TEXT(PACKAGE_VENDOR_STRING "."
                                        PACKAGE_NAME_STRING),
               * progIDCurVerKeyString = TEXT(PACKAGE_VENDOR_STRING "."
                                              PACKAGE_NAME_STRING "\\CurVer");

    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                versionProgIDCLSIDKeyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {
      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 versionProgIDCLSIDKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                versionProgIDDefaultIconKeyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {
      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 versionProgIDDefaultIconKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                versionProgIDKeyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {
      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 versionProgIDKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                progIDCurVerKeyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {
      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 progIDCurVerKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                progIDKeyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {
      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 progIDKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
  }
  /* Delete the HKCR\CLSID\{CLSID_IcedTea_Web} registry key and its sub-keys */
  {
    TCHAR* clsidKeyString = NULL, * keyString = NULL;
    const TCHAR* controlKeyString = TEXT("Control"),
               * defaultIconKeyString = TEXT("DefaultIcon"),
               * inprocHandler32KeyString = TEXT("InprocHandler32"),
               * inprocServer32KeyString = TEXT("InprocServer32"),
               * interfaceKeyString = TEXT("Interface"),
               * versionKeyString = TEXT("Version");
    HANDLE processHeap = NULL;
#if defined UNICODE || defined _UNICODE
    size_t len;
#endif /* UNICODE || _UNICODE */

    {
      HRESULT hResult;
      LPOLESTR GUIDString = NULL;

      /* Get CLSID_IcedTea_Web as string */
      if ((hResult = StringFromCLSID(&CLSID_IcedTea_Web,
                                     &GUIDString)) != S_OK) {
        LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                              HRESULT_FACILITY(hResult),
                              HRESULT_CODE(hResult)),
                   TEXT("Could not create string from CLSID"));
        return hResult;
      }
#if !defined UNICODE || !defined _UNICODE
      LPOLESTR2MBCS(GUIDString)
#endif /* UNICODE || _UNICODE */
      {
        /* Allocate memory for CLSID_IcedTea_Web registry key string */
        if ((clsidKeyString =
             HeapAlloc(processHeap = GetProcessHeap(),
                       HEAP_ZERO_MEMORY,
#if defined UNICODE || defined _UNICODE
                       (len = _sctprintf(TEXT("CLSID\\%s"),
                       (TCHAR*)GUIDString)) *
#else
                       _sctprintf(TEXT("CLSID\\%s"),
                       (TCHAR*)GUIDString) *
#endif /* UNICODE || _UNICODE */
                       sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
          LogMessage(ERROR_OUTOFMEMORY,
                     TEXT("Could not allocate memory for registry key string"));
          CoTaskMemFree(GUIDString);
          return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
        }
        /* Format "CLSID\{CLSID_IcedTea_Web}" string */
        _stprintf(clsidKeyString,
#if defined UNICODE || defined _UNICODE
                  len + 1,
#endif /* UNICODE || _UNICODE */
                  TEXT("CLSID\\%s"),
                  (TCHAR*)GUIDString);
      }
      /* Free GUIDString ({CLSID_IcedTea_Web}) because it is no longer needed */
      CoTaskMemFree(GUIDString);
    }
    /* Allocate memory for HKCR\CLSID\{CLSID_IcedTea_Web}\Version registry key
       string */
    if ((keyString = HeapAlloc(processHeap,
                               HEAP_ZERO_MEMORY,
#if defined UNICODE || defined _UNICODE
                               (len = _sctprintf(TEXT("%s\\%s"),
                                                 clsidKeyString,
                                                 versionKeyString)) *
#else
                               _sctprintf(TEXT("%s\\%s"),
                                          clsidKeyString,
                                          versionKeyString) *
#endif /* UNICODE || _UNICODE */
                               sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
      LogMessage(ERROR_OUTOFMEMORY,
                 TEXT("Could not allocate memory for registry key string"));
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
    }
    /* Make HKCR\CLSID\{CLSID_IcedTea_Web}\Version registry key string */
    _stprintf(keyString,
#if defined UNICODE || defined _UNICODE
              len + 1,
#endif /* UNICODE || _UNICODE */
              TEXT("%s\\%s"),
              clsidKeyString,
              versionKeyString);
    /* Delete HKCR\CLSID\{CLSID_IcedTea_Web}\Version registry key string */
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                keyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {

      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 keyString);
      HeapFree(processHeap, 0, keyString);
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      TCHAR* tmpKeyString = NULL;

      /* Allocate memory for HKCR\CLSID\{CLSID_IcedTea_Web}\Interface registry
         key string */
      if ((tmpKeyString =
           HeapReAlloc(processHeap,
                       HEAP_ZERO_MEMORY,
                       keyString,
#if defined UNICODE || defined _UNICODE
                       (len = _sctprintf(TEXT("%s\\%s"),
                                         clsidKeyString,
                                         interfaceKeyString)) *
#else
                       _sctprintf(TEXT("%s\\%s"),
                                  clsidKeyString,
                                  interfaceKeyString) *
#endif /* UNICODE || _UNICODE */
                       sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        HeapFree(processHeap, 0, keyString);
        HeapFree(processHeap, 0, clsidKeyString);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      keyString = tmpKeyString;
    }
    /* Make HKCR\CLSID\{CLSID_IcedTea_Web}\Interface registry key string */
    _stprintf(keyString,
#if defined UNICODE || defined _UNICODE
              len + 1,
#endif /* UNICODE || _UNICODE */
              TEXT("%s\\%s"),
              clsidKeyString,
              interfaceKeyString);
    /* Delete HKCR\CLSID\{CLSID_IcedTea_Web}\Interface registry key string */
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                keyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {

      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 keyString);
      HeapFree(processHeap, 0, keyString);
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      TCHAR* tmpKeyString = NULL;

      /* Allocate memory for HKCR\CLSID\{CLSID_IcedTea_Web}\InprocServer32
         registry key string */
      if ((tmpKeyString =
           HeapReAlloc(processHeap,
                       HEAP_ZERO_MEMORY,
                       keyString,
#if defined UNICODE || defined _UNICODE
                       (len = _sctprintf(TEXT("%s\\%s"),
                                         clsidKeyString,
                                         inprocServer32KeyString)) *
#else
                       _sctprintf(TEXT("%s\\%s"),
                                  clsidKeyString,
                                  inprocServer32KeyString) *
#endif /* UNICODE || _UNICODE */
                       sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        HeapFree(processHeap, 0, keyString);
        HeapFree(processHeap, 0, clsidKeyString);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      keyString = tmpKeyString;
    }
    /* Make HKCR\CLSID\{CLSID_IcedTea_Web}\InprocServer32 registry key string */
    _stprintf(keyString,
#if defined UNICODE || defined _UNICODE
              len + 1,
#endif /* UNICODE || _UNICODE */
              TEXT("%s\\%s"),
              clsidKeyString,
              inprocServer32KeyString);
    /* Delete HKCR\CLSID\{CLSID_IcedTea_Web}\InprocServer32 registry key
       string */
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                keyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {

      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 keyString);
      HeapFree(processHeap, 0, keyString);
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      TCHAR* tmpKeyString = NULL;

      /* Allocate memory for HKCR\CLSID\{CLSID_IcedTea_Web}\InprocHandler32
         registry key string */
      if ((tmpKeyString =
           HeapReAlloc(processHeap,
                       HEAP_ZERO_MEMORY,
                       keyString,
#if defined UNICODE || defined _UNICODE
                       (len = _sctprintf(TEXT("%s\\%s"),
                                         clsidKeyString,
                                         inprocHandler32KeyString)) *
#else
                       _sctprintf(TEXT("%s\\%s"),
                                  clsidKeyString,
                                  inprocHandler32KeyString) *
#endif /* UNICODE || _UNICODE */
                       sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        HeapFree(processHeap, 0, keyString);
        HeapFree(processHeap, 0, clsidKeyString);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      keyString = tmpKeyString;
    }
    /* Make HKCR\CLSID\{CLSID_IcedTea_Web}\InprocHandler32 registry key string */
    _stprintf(keyString,
#if defined UNICODE || defined _UNICODE
              len + 1,
#endif /* UNICODE || _UNICODE */
              TEXT("%s\\%s"),
              clsidKeyString,
              inprocHandler32KeyString);
    /* Delete HKCR\CLSID\{CLSID_IcedTea_Web}\InprocHandler32 registry key
       string */
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                keyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {

      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 keyString);
      HeapFree(processHeap, 0, keyString);
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      TCHAR* tmpKeyString = NULL;

      /* Allocate memory for HKCR\CLSID\{CLSID_IcedTea_Web}\DefaultIcon registry
         key string */
      if ((tmpKeyString =
           HeapReAlloc(processHeap,
                       HEAP_ZERO_MEMORY,
                       keyString,
#if defined UNICODE || defined _UNICODE
                       (len = _sctprintf(TEXT("%s\\%s"),
                                         clsidKeyString,
                                         defaultIconKeyString)) *
#else
                       _sctprintf(TEXT("%s\\%s"),
                                  clsidKeyString,
                                  defaultIconKeyString) *
#endif /* UNICODE || _UNICODE */
                       sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        HeapFree(processHeap, 0, keyString);
        HeapFree(processHeap, 0, clsidKeyString);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      keyString = tmpKeyString;
    }
    /* Make HKCR\CLSID\{CLSID_IcedTea_Web}\DefaultIcon registry key string */
    _stprintf(keyString,
#if defined UNICODE || defined _UNICODE
              len + 1,
#endif /* UNICODE || _UNICODE */
              TEXT("%s\\%s"),
              clsidKeyString,
              defaultIconKeyString);
    /* Delete HKCR\CLSID\{CLSID_IcedTea_Web}\DefaultIcon registry key string */
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                keyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {

      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 keyString);
      HeapFree(processHeap, 0, keyString);
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    {
      TCHAR* tmpKeyString = NULL;

      /* Allocate memory for HKCR\CLSID\{CLSID_IcedTea_Web}\Control
         registry key string */
      if ((tmpKeyString =
           HeapReAlloc(processHeap,
                       HEAP_ZERO_MEMORY,
                       keyString,
#if defined UNICODE || defined _UNICODE
                       (len = _sctprintf(TEXT("%s\\%s"),
                                         clsidKeyString,
                                         controlKeyString)) *
#else
                       _sctprintf(TEXT("%s\\%s"),
                                  clsidKeyString,
                                  controlKeyString) *
#endif /* UNICODE || _UNICODE */
                       sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
        LogMessage(ERROR_OUTOFMEMORY,
                   TEXT("Could not allocate memory for registry key string"));
        HeapFree(processHeap, 0, keyString);
        HeapFree(processHeap, 0, clsidKeyString);
        return HRESULT_FROM_WIN32(ERROR_OUTOFMEMORY);
      }
      keyString = tmpKeyString;
    }
    /* Make HKCR\CLSID\{CLSID_IcedTea_Web}\Control registry key string */
    _stprintf(keyString,
#if defined UNICODE || defined _UNICODE
              len + 1,
#endif /* UNICODE || _UNICODE */
              TEXT("%s\\%s"),
              clsidKeyString,
              controlKeyString);
    /* Delete HKCR\CLSID\{CLSID_IcedTea_Web}\Control registry key string */
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                keyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {

      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 keyString);
      HeapFree(processHeap, 0, keyString);
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    /* Free the memory pointed to by keyString because it is no longer needed */
    HeapFree(processHeap, 0, keyString);
    /* Delete HKCR\CLSID\{CLSID_IcedTea_Web} registry key string */
    if ((lResult = RegDeleteKey(HKEY_CLASSES_ROOT,
                                clsidKeyString)) != ERROR_SUCCESS &&
        lResult != ERROR_FILE_NOT_FOUND) {
      LogMessage(lResult,
                 TEXT("Could not delete registry key \"HKCR\\%s\""),
                 clsidKeyString);
      HeapFree(processHeap, 0, clsidKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
    HeapFree(processHeap, 0, clsidKeyString);
  }
  return S_OK;
}

static DWORD WINAPI UnregisterCOMClassThread(LPVOID arg) {
  return (DWORD)UnregisterCOMClass();
}

static HRESULT WINAPI UnregisterEventLogSource() {
/* TODO: Delete only actually created registry keys and values, in order to
   spare any registry sub-keys and values possibly created by the user.*/
/* TODO: Modify to redirect the event log source to the latest registered
   IcedTea-Web class version. */
  LONG lResult;
  const TCHAR* eventSourceKeyString =
    TEXT("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\"
         PACKAGE_NAME_STRING);

  /* Deregister the event log source before unregistering it with the event
     logging service */
  if (eventSource != NULL)
    if (DeregisterEventSource(eventSource) == FALSE)
      LogMessage(-GetLastError(),
                 TEXT("Could not deregister event log source \"%s\""),
                 PACKAGE_NAME_STRING);
    else eventSource = NULL;
  /* As it should be safe to Delete
   SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\PACKAGE_NAME
   registry key */
  if ((lResult = RegDeleteKey(HKEY_LOCAL_MACHINE, eventSourceKeyString)) !=
      ERROR_SUCCESS && lResult != ERROR_FILE_NOT_FOUND) {
    LogMessage(lResult,
               TEXT("Could not delete registry key \"HKLM\\%s\""),
               eventSourceKeyString);
    return HRESULT_FROM_WIN32(lResult);
  }
  /* Reregister the event log source without the event logging service */
  if (eventSource == NULL)
    if ((eventSource = RegisterEventSource(NULL,
                                           TEXT(PACKAGE_NAME_STRING))) ==
        FALSE)
      LogMessage(-GetLastError(),
                 TEXT("Could not register event log source \"%s\""),
                 PACKAGE_NAME_STRING);
  return S_OK;
}

static HRESULT WINAPI UnregisterNP() {
/* TODO: Delete only actually created registry keys and values, in order to
   spare any registry sub-keys and values possibly created by the user.*/
  LONG lResult;
  const TCHAR* icedTeaWebKeyString = TEXT("SOFTWARE\\MozillaPlugins\\@"
                                          PACKAGE_VENDOR_STRING "/"
                                          PACKAGE_NAME_STRING ",version="
                                          PACKAGE_VERSION_STRING);

/*  HKEY icedTeaWebKey = NULL;

  if ((lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                              icedTeaWebKeyString,
                              0,
                              KEY_WRITE,
                              &icedTeaWebKey)) != ERROR_SUCCESS &&
      lResult != ERROR_FILE_NOT_FOUND) {
    LogMessage(lResult,
               TEXT("Could not open registry key \"HKLM\\%s\" for deleting"),
               icedTeaWebKeyString);
    return HRESULT_FROM_WIN32(lResult);
  } else {
    if ((lResult = RegDeleteValue(icedTeaWebKey,
                                  TEXT("Version"))) != ERROR_SUCCESS) {
      LogMessage(-lResult,
                 TEXT("Could not delete value \"%s\" in registry key \"HKLM\\%s\""),
                 TEXT("Version"),
                 icedTeaWebKeyString);
      RegCloseKey(icedTeaWebKey);
      return HRESULT_FROM_WIN32(lResult);
    }
    if ((lResult = RegCloseKey(icedTeaWebKey)) != ERROR_SUCCESS) {
      LogMessage(-lResult,
                 TEXT("Could not close registry key \"HKLM\\%s\""),
                 icedTeaWebKeyString);
      return HRESULT_FROM_WIN32(lResult);
    }
  }*/
  /* Delete HKLM\SOFTWARE\MozillaPlugins\@PACKAGE_VENDOR/PACKAGE_NAME,version=P
     ACKAGE_VERSION registry key string */
  if ((lResult = RegDeleteKey(HKEY_LOCAL_MACHINE,
                              icedTeaWebKeyString)) != ERROR_SUCCESS &&
      lResult != ERROR_FILE_NOT_FOUND) {
    LogMessage(lResult,
               TEXT("Could not delete registry key \"HKLM\\%s\""),
               icedTeaWebKeyString);
    return HRESULT_FROM_WIN32(lResult);
  }
  return S_OK;
}

static VOID WINAPI SetCurrentUserSID() {
  HANDLE currentProcess = NULL,
         currentProcessToken = NULL,
         processHeap = NULL;
  TOKEN_USER* currentUserToken = NULL;

  if (OpenProcessToken(currentProcess = GetCurrentProcess(),
                       TOKEN_QUERY,
                       &currentProcessToken) == FALSE) {
    LogMessage(GetLastError(),
               TEXT("Could not open process token to get current user's SID"));
    CloseHandle(currentProcess);
    return;
  }
  {
    DWORD returnLength = 0;

    /* Get the size for TOKEN_USER structure first */
    GetTokenInformation(currentProcessToken,
                        TokenUser,
                        NULL,
                        0,
                        &returnLength);
    /* Allocate enough memory for TOKEN_USER structure */
    if ((currentUserToken =
         (TOKEN_USER*)HeapAlloc(processHeap = GetProcessHeap(),
                                HEAP_ZERO_MEMORY,
                                returnLength)) == NULL) {
      LogMessage(GetLastError(), /* HeapAlloc does indeed call SetLastError() */
                 TEXT("Could not allocate memory for user token information"));
      CloseHandle(currentProcess);
      CloseHandle(currentProcessToken);
      return;
    }
    /* Get the actual TOKEN_USER structure */
    if (GetTokenInformation(currentProcessToken,
                            TokenUser,
                            currentUserToken,
                            returnLength,
                            &returnLength) == FALSE) {
      LogMessage(GetLastError(), TEXT("Could not get user token information"));
      HeapFree(processHeap, 0, currentUserToken);
      CloseHandle(currentProcess);
      CloseHandle(currentProcessToken);
      return;
    }
  }
  /* Close unneeded handles */
  CloseHandle(currentProcess);
  CloseHandle(currentProcessToken);
  {
    DWORD len;

    /* Allocate memory to copy only the user SID out of the TOKEN_USER structure */
    if ((currentUserSID =
         (SID*)HeapAlloc(processHeap,
                         HEAP_ZERO_MEMORY,
                         len = GetLengthSid((*currentUserToken).User.Sid))) ==
         NULL) {
      LogMessage(GetLastError(), /* HeapAlloc does indeed call SetLastError() */
                 TEXT("Could not allocate memory for user SID to copy"));
      HeapFree(processHeap, 0, currentUserToken);
      return;
    }
    /* Copy the SID */
    if (CopySid(len, currentUserSID, (*currentUserToken).User.Sid) == FALSE) {
        LogMessage(GetLastError(), TEXT("Could not copy current user SID"));
        HeapFree(processHeap, 0, currentUserSID);
        currentUserSID = NULL;
    }
  }
  /* Release TOKEN_USER memory structure */
  HeapFree(processHeap, 0, currentUserToken);
}

static INT_PTR CALLBACK SyntaxDialogProc(HWND dialog,
                                         UINT message,
                                         WPARAM wParam,
                                         LPARAM lParam) {
  if (dialog == NULL) return FALSE;
  switch (message) {
    case WM_COMMAND:
      if (wParam == MAKEWPARAM(IDC_SYNTAX_OK_BUTTON, BN_CLICKED)) {
        PostMessage(dialog, WM_CLOSE, 0, 0);
        return TRUE;
      }
    break;
    case WM_CLOSE: {
        HFONT messageFont = NULL;

        if ((messageFont = (HFONT)SendMessage(dialog,
                                              WM_GETFONT,
                                              0,
                                              0)) != NULL) {
          DeleteObject(messageFont);
        }
      }
      EndDialog(dialog, 1);
      return TRUE;

    case WM_INITDIALOG: {
      HANDLE processHeap = NULL;
      {
         NONCLIENTMETRICS* nonClientMetrics = NULL;

        /* Allocate memory for non-client metrics structure */
        if ((nonClientMetrics =
             HeapAlloc(processHeap = GetProcessHeap(),
                       HEAP_ZERO_MEMORY,
                       sizeof (NONCLIENTMETRICS))) == NULL) {
          LogMessage(-ERROR_NOT_ENOUGH_MEMORY,
                     TEXT("Could not allocate memory for non-client metrics"));
        } else {
          /* Get non-client metrics containing the user defined font face for
           message boxes. Respect user's look and feel. */
          (*nonClientMetrics).cbSize = sizeof (NONCLIENTMETRICS);
          if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
                                   sizeof (NONCLIENTMETRICS),
                                   (PVOID)nonClientMetrics,
                                   0u) == FALSE) {
            LogMessage(-GetLastError(),
                       TEXT("Could not get non-client metrics"));
            HeapFree(processHeap, 0, nonClientMetrics);
          } else {
            /* Set syntax dialog and all of its children font faces to user
               defined message box font face */
            HFONT messageFont = NULL;

            if ((messageFont =
                 CreateFontIndirect(&(*nonClientMetrics).lfMessageFont)) ==
                 NULL) {
              LogMessage(-GetLastError(),
                         TEXT("Could not create \"%s\" font face"),
                         (*nonClientMetrics).lfMessageFont.lfFaceName);
              HeapFree(processHeap, 0, nonClientMetrics);
            } else {
              TEXTMETRIC textMetric;
              HeapFree(processHeap, 0, nonClientMetrics);
              HDC dialogDC = NULL;

              SelectObject(dialogDC = GetDC(dialog), messageFont);
              /* WM_SETFONT should actually be sent here to all child windows,
                 but in lack of a convenient function we send it to every child
                 one-by-one. EnumChildWindows() is to much hassle for this. */
              SendMessage(dialog,
                          WM_SETFONT,
                          (WPARAM)messageFont,
                          TRUE);
              SendDlgItemMessage(dialog,
                                 IDC_SYNTAX_EDIT,
                                 WM_SETFONT,
                                 (WPARAM)messageFont,
                                 TRUE);
              SendDlgItemMessage(dialog,
                                 IDC_SYNTAX_OK_BUTTON,
                                 WM_SETFONT,
                                 (WPARAM)messageFont,
                                 TRUE);
              /* Resize the dialog to accommodate for the font's width */
              if (GetTextMetrics(dialogDC,
                                 &textMetric) == FALSE) {
                LogMessage(-GetLastError(), TEXT("Could not get text metrics"));
              } else {
                HWND editControl, okButton;
                WINDOWPLACEMENT windowPlacement = {
                  sizeof (WINDOWPLACEMENT)
                };
                RECT dialogRect;
                LONG width;

                GetWindowPlacement(editControl = GetDlgItem(dialog,
                                                            IDC_SYNTAX_EDIT),
                                   &windowPlacement);
                windowPlacement.rcNormalPosition.right =
                  textMetric.tmAveCharWidth * 80 +
                  windowPlacement.rcNormalPosition.left;
                SetWindowPlacement(editControl, &windowPlacement);
                width = windowPlacement.rcNormalPosition.left +
                  windowPlacement.rcNormalPosition.right;
                GetWindowRect(dialog, &dialogRect);
                SetWindowPos(dialog,
                             (HWND)0,
                             0,
                             0,
                             3 * windowPlacement.rcNormalPosition.left +
                             windowPlacement.rcNormalPosition.right,
                             dialogRect.bottom - dialogRect.top,
                             SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
                GetClientRect(dialog, &dialogRect);
                GetWindowPlacement(okButton = GetDlgItem(dialog,
                                                         IDC_SYNTAX_OK_BUTTON),
                                   &windowPlacement);
                windowPlacement.rcNormalPosition.left =
                  (dialogRect.right - dialogRect.left -
                  (width =
                    windowPlacement.rcNormalPosition.right -
                    windowPlacement.rcNormalPosition.left)) >> 1;
                windowPlacement.rcNormalPosition.right =
                  windowPlacement.rcNormalPosition.left + width;
                SetWindowPlacement(okButton, &windowPlacement);
              }
              ReleaseDC(dialog, dialogDC);
            }
          }
        }
      }
      {
        HICON icon = NULL;

        if ((icon = LoadIcon(GetModuleHandle(TEXT(PLUGIN_MODULE_NAME_STRING)),
                             MAKEINTRESOURCE(ID_ICON))) == NULL) {
          LogMessage(-GetLastError(),
                     TEXT("Could not load icon from module \"%s\""),
                     TEXT(PLUGIN_MODULE_NAME_STRING));
        } else {
          SendMessage(dialog, WM_SETICON, 0, (LPARAM)icon);
          SendMessage(dialog, WM_SETICON, 1, (LPARAM)icon);
        }
      }
      /* Center the dialog on the calling window's monitor */
      {
        HMONITOR monitor = MonitorFromWindow(GetForegroundWindow(),
                                             MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX* monitorInfo = NULL;

        if ((monitorInfo =
             (MONITORINFOEX*)HeapAlloc(processHeap,
                                       HEAP_ZERO_MEMORY,
                                       sizeof (MONITORINFOEX))) == NULL) {
          LogMessage(-ERROR_OUTOFMEMORY,
                     TEXT("Could not allocate memory for monitor info"));
        } else {
          (*monitorInfo).cbSize = (DWORD)sizeof (MONITORINFOEX);
          if (GetMonitorInfo(monitor, (LPMONITORINFO)monitorInfo) == FALSE) {
            LogMessage(-GetLastError(), TEXT("Could not get monitor info"));
          } else {
            RECT dialogRect;

            if (GetWindowRect(dialog, &dialogRect) == FALSE) {
              LogMessage(-GetLastError(),
                         TEXT("Could not get dialog's rectangle"));
            } else {
              if (SetWindowPos(dialog,
                               (HWND)0,
                               ((((*monitorInfo).rcMonitor.right -
                                  (*monitorInfo).rcMonitor.left) -
                                  (dialogRect.right - dialogRect.left)) >> 1) +
                                  (*monitorInfo).rcMonitor.left,
                               ((((*monitorInfo).rcMonitor.bottom -
                                  (*monitorInfo).rcMonitor.top) -
                                  (dialogRect.bottom - dialogRect.top)) >> 1) +
                                  (*monitorInfo).rcMonitor.top,
                               0,
                               0,
                               SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER) ==
                  FALSE) {
                LogMessage(-GetLastError(),
                           TEXT("Could not center dialog on monitor \"%s\""),
                           (*monitorInfo).szDevice);
              }
            }
          }
          HeapFree(processHeap, 0, monitorInfo);
        }
      }
      {
        LPTSTR syntaxHelpText = NULL;
        int len;

        if ((syntaxHelpText =
             (LPTSTR)HeapAlloc(processHeap,
                               HEAP_ZERO_MEMORY,
                               (len = GetWindowTextLength(GetDlgItem(dialog,
                                                                     IDC_SYNTAX_EDIT))) *
                               sizeof (TCHAR) + sizeof (TCHAR))) == NULL) {
          LogMessage(-ERROR_NOT_ENOUGH_MEMORY,
                     TEXT("Could not allocate memory"));
        } else {
          /* Get the syntax help text from the edit control */
          GetDlgItemText(dialog, IDC_SYNTAX_EDIT, syntaxHelpText, len + 1);
          /* Print syntax help text */
          LogMessage(ERROR_SUCCESS, (TCHAR*)syntaxHelpText);
          HeapFree(processHeap, 0, syntaxHelpText);
        }
      }
    }
    /* TODO: Add default focus to OK button */
    return TRUE;

    case WM_KEYDOWN:
      if (wParam == VK_ESCAPE) {
        PostMessage(dialog, WM_CLOSE, 0, 0);
        return TRUE;
      }
  }
  return FALSE;
}

static TCHAR* WINAPI GetThisModuleFileName() {
  HMODULE module = NULL;
  const TCHAR* internalModuleName = TEXT(PLUGIN_MODULE_NAME_STRING);
  TCHAR* moduleFileName = NULL;

  /* Get the handle to this module (not the possibly calling regsvr32.exe) */
  if ((module = GetModuleHandle(internalModuleName)) == NULL) {
    DWORD lastError;

    LogMessage(lastError = GetLastError(),
               TEXT("Could not get handle for module \"%s\""),
               internalModuleName);
    SetLastError(lastError);
    return NULL;
  }
#if defined UNICODE || defined _UNICODE
#define ITW_TEMP_MAX_PATH MAX_PATH
#undef MAX_PATH
#define MAX_PATH 32767
#endif /* UNICODE || UNICODE */
  /* Allocate memory module file name string */
  if ((moduleFileName =
       (TCHAR*)CoTaskMemAlloc(MAX_PATH * sizeof (TCHAR) + sizeof (TCHAR))) ==
      NULL) {
    const DWORD lastError = ERROR_OUTOFMEMORY;

    LogMessage(lastError,
               TEXT("Could not allocate memory for module file name string"));
    SetLastError(lastError);
    return NULL;
  }
  /* Get this module's file name string */
  if (GetModuleFileName(module,
                        moduleFileName,
                        MAX_PATH) == 0) {
    DWORD lastError = GetLastError();

    CoTaskMemFree(moduleFileName);
    LogMessage(lastError, TEXT("Could not get module file name"));
    SetLastError(lastError);
    return NULL;
  }
#ifdef ITW_TEMP_MAX_PATH
#undef MAX_PATH
#define MAX_PATH ITW_TEMP_MAX_PATH
#undef ITW_TEMP_MAX_PATH
#endif /* ITW_TEMP_MAX_PATH */
  return moduleFileName;
}

HRESULT STDMETHODCALLTYPE ClassFactory_QueryInterface(IClassFactory* This,
                                                      REFIID iid,
                                                      void** v) {
  if (This == NULL || iid == NULL) {
    if (v != NULL) *v = NULL;
    return E_INVALIDARG;
  }
  if (v == NULL) return E_POINTER;

  {
    LPOLESTR iidString = NULL;
    HRESULT hr;
    if ((hr = StringFromIID(iid, &iidString)) != S_OK) {
      *v = NULL;
      LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hr),
                            HRESULT_FACILITY(hr),
                            HRESULT_CODE(hr)),
                 TEXT("%S(): Could not make string from IID"), __FUNCTION__);
      return hr;
    }
    LogMessage(0u, TEXT("%S(0x%p, 0x%p == \"%s\", 0x%p)"), __FUNCTION__, This, iid, iidString, v);
    CoTaskMemFree(iidString);
  }

  if (IsEqualIID(iid, &IID_IUnknown) || IsEqualIID(iid, &IID_IClassFactory)) {
    *v == This;
    (*(*This).lpVtbl).AddRef(This);
    return S_OK;
  }

  *v == NULL;
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ClassFactory_AddRef(IClassFactory* This) {
  LogMessage(0, TEXT("%S(0x%p): iClassFactoryRefCount == %u"), __FUNCTION__, This, iClassFactoryRefCount);
  return This ?
            iClassFactoryRefCount == MAXULONG ?
              MAXULONG :
              InterlockedIncrement(&iClassFactoryRefCount) :
            0u;
}

ULONG STDMETHODCALLTYPE ClassFactory_Release(IClassFactory* This) {
  if (This == NULL) return 0u;

  LogMessage(0u, TEXT("%S(0x%p): iClassFactoryRefCount == %u"), __FUNCTION__, This, iClassFactoryRefCount);

  if (iClassFactoryRefCount) {
    ULONG iClassFactoryRefCountAfterDecrement;
    if (iClassFactoryRefCountAfterDecrement =
        InterlockedDecrement(&iClassFactoryRefCount))
      return iClassFactoryRefCount;
  } else return 0u;
  CoTaskMemFree((*This).lpVtbl);
  (*This).lpVtbl = NULL;
  CoTaskMemFree(This);
  iClassFactory = NULL;
  return iClassFactoryRefCount = 0u;
}

HRESULT STDMETHODCALLTYPE ClassFactory_CreateInstance(IClassFactory* This,
                                                      IUnknown* iUnknown,
                                                      REFIID iid,
                                                      void** v) {
  if (This == NULL || iid == NULL || v == NULL) {
    if (v) *v = NULL;
    return E_INVALIDARG;
  }
  if (iUnknown) {
    *v = NULL;
    LogMessage(MAKE_SCODE(HRESULT_SEVERITY(CLASS_E_NOAGGREGATION),
                          HRESULT_FACILITY(CLASS_E_NOAGGREGATION),
                          HRESULT_CODE(CLASS_E_NOAGGREGATION)),
               TEXT("%S(): %s does not support interface aggregation"), __FUNCTION__, TEXT(PACKAGE_NAME_STRING));
    return CLASS_E_NOAGGREGATION;
  }

  {
    LPOLESTR iidString = NULL;
    {
      HRESULT hResult;
      if ((hResult = StringFromIID(iid, &iidString)) != S_OK) {
        LogMessage(MAKE_SCODE(HRESULT_SEVERITY(hResult),
                              HRESULT_FACILITY(hResult),
                              HRESULT_CODE(hResult)),
                   TEXT("%S(): Could not make string from IID"), __FUNCTION__);
        *v = NULL;
        return hResult;
      }
    }
    LogMessage(0u, TEXT("%S(0x%p, 0x%p, 0x%p == \"%s\", 0x%p)"),
             __FUNCTION__,
             This,
             iUnknown,
             iid,
             iidString,
             v);
    CoTaskMemFree(iidString);
  }

  // if (IsEqualIID(iid, &IID_IUnknown)) {
    // (*(*This).lpVtbl).AddRef(*v = This);
  // } else
    if (IsEqualIID(iid, &IID_IExtractIcon)) {
      if ((*v = (LPVOID)CoTaskMemAlloc(sizeof (IcedTea_WebIconHandler))) ==
          NULL)
        return E_OUTOFMEMORY;
      if (((*(IExtractIcon*)*v).lpVtbl =
          (IExtractIconVtbl*)CoTaskMemAlloc(
            sizeof (IExtractIconVtbl))) == NULL) {
        CoTaskMemFree(*v);
        *v = NULL;
        return E_OUTOFMEMORY;
      }
      if (((*(IcedTea_WebIconHandler*)*v).persistFile =
          (IcedTea_WebPersistFile*)CoTaskMemAlloc(
            sizeof (IcedTea_WebPersistFile))) == NULL) {
        CoTaskMemFree((*(IcedTea_WebIconHandler*)*v).lpVtbl);
        CoTaskMemFree(*v);
        *v = NULL;
        return E_OUTOFMEMORY;
      }
      if (((*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl =
          (IPersistFileVtbl*)CoTaskMemAlloc(
            sizeof (IPersistFileVtbl))) == NULL) {
        CoTaskMemFree((*(IcedTea_WebIconHandler*)*v).persistFile);
        CoTaskMemFree((*(IcedTea_WebIconHandler*)*v).lpVtbl);
        CoTaskMemFree(*v);
        *v = NULL;
        return E_OUTOFMEMORY;
      }
      /* IExtractIcon */
      (*(*(IcedTea_WebIconHandler*)*v).lpVtbl).QueryInterface =
      ExtractIcon_QueryInterface;
      (*(*(IcedTea_WebIconHandler*)*v).lpVtbl).AddRef =
      ExtractIcon_AddRef;
      (*(*(IcedTea_WebIconHandler*)*v).lpVtbl).Release =
      ExtractIcon_Release;
      (*(*(IcedTea_WebIconHandler*)*v).lpVtbl).GetIconLocation =
      ExtractIcon_GetIconLocation;
      (*(*(IcedTea_WebIconHandler*)*v).lpVtbl).Extract =
      ExtractIcon_Extract;
      (*(IcedTea_WebIconHandler*)*v).refCount = 0u;
      (*(*(IcedTea_WebIconHandler*)*v).lpVtbl).AddRef((IExtractIcon*)*v);

      /* IPersistFile */
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).QueryInterface =
      PersistFile_QueryInterface;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).AddRef =
      PersistFile_AddRef;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).Release =
      PersistFile_Release;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).GetClassID =
      PersistFile_GetClassID;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).GetCurFile =
      PersistFile_GetCurFile;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).IsDirty =
      PersistFile_IsDirty;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).Load =
      PersistFile_Load;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).Save =
      PersistFile_Save;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).SaveCompleted =
      PersistFile_SaveCompleted;
      (*(*(IcedTea_WebIconHandler*)*v).persistFile).fileName =
      SysAllocString(NULL);
      (*(*(IcedTea_WebIconHandler*)*v).persistFile).mode = 0u;
      (*(*(IcedTea_WebIconHandler*)*v).persistFile).refCount = 0u;
      (*(*(*(IcedTea_WebIconHandler*)*v).persistFile).lpVtbl).AddRef(
        (IPersistFile*)(*(IcedTea_WebIconHandler*)*v).persistFile);
    // } else if (IsEqualIID(iid, &IID_IShellExtInit)) {
      // if ((*v = (IcedTea_WebPropertyHandler*)CoTaskMemAlloc(
        // sizeof (IcedTea_WebPropertyHandler))) == NULL)
        // return E_OUTOFMEMORY;
    // if (((*(IShellExtInit*)*v).lpVtbl = (IShellExtInitVtbl*)CoTaskMemAlloc(
      // sizeof (IShellExtInitVtbl))) == NULL) {
        // CoTaskMemFree(*v);
        // *v = NULL;
        // return E_OUTOFMEMORY;
    // }
    // (*(*(IShellExtInit*)*v).lpVtbl).QueryInterface =
    // ShellExtInit_QueryInterface;
    // (*(*(IShellExtInit*)*v).lpVtbl).AddRef =
    // ShellExtInit_AddRef;
    // (*(*(IShellExtInit*)*v).lpVtbl).Release =
    // ShellExtInit_Release;
    // (*(*(IShellExtInit*)*v).lpVtbl).Initialize =
    // ShellExtInit_Initialize;
    // (*(IcedTea_WebPropertyHandler*)*v).lpVtblQueryInfo = NULL;
    // (*(IcedTea_WebPropertyHandler*)*v).refCount = 0u;
    // (*(IcedTea_WebPropertyHandler*)*v).dataObject = NULL;
    // (*(IcedTea_WebPropertyHandler*)*v).dataObjectMutex = NULL;
    // (*(*(IShellExtInit*)*v).lpVtbl).AddRef((IShellExtInit*)*v);
    } else {
      *v = NULL;
      return E_NOINTERFACE;
    }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassFactory_LockServer(IClassFactory* This,
                                                   BOOL lock) {
  if (This == NULL) return E_INVALIDARG;

  LogMessage(0u, TEXT("%S(0x%p, %s): iClassFactoryRefCount == %u"), __FUNCTION__, This, lock ? TEXT("TRUE") : TEXT("FALSE"), iClassFactoryRefCount);
  return CoLockObjectExternal((IUnknown*)This, lock, FALSE);
}