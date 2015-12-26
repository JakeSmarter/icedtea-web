#include <windows.h>
// Both tchar.h AND wchar.h are required for Unicode!
#include <tchar.h>
#include <wchar.h>
// tchar.h and wchar.h MUST be included BEFORE stdio.h!
#include <stdio.h>
#include <locale.h>

#include "IcedTea-Web.h"
//#include "Dll.h"
#include "LogMessage.h"

VOID WINAPI LogMessage(DWORD error, const TCHAR* message, ...) {
  /* Set C runtime's locale to current user's defined locale unless already set
   * to something else than "C" by the process (makes standard streams work with
   * non-ASCII characters in messages). More specifically, it sets the C
   * runtime's internal processing codepage. Using the *_l() locale aware
   * functions would be a more robust solution. However, the *_l() functions
   * only set the formatting, not the the internal codepage. */
  if (_tcscmp(_tsetlocale(LC_ALL, NULL), _T("C")) == 0)
    _tsetlocale(LC_ALL, _T(""));

  if (message) {
    va_list arglist = NULL;

    HANDLE processHeap = NULL;
    _TCHAR* expandedMessage = NULL;
#if defined UNICODE || _UNICODE
      size_t len = 0;
#endif /* UNICODE || _UNICODE */

    /* get first argument of variable argument list */
    va_start(arglist, message);
    /* Allocate memory for expanded message */
    if ((expandedMessage =
          (_TCHAR*)HeapAlloc(processHeap = GetProcessHeap(),
                             HEAP_ZERO_MEMORY,
#if defined UNICODE || _UNICODE
                             (len = _vscwprintf(message, arglist)) *
#else
                             _vscprintf(message, arglist) *
#endif /* UNICODE || _UNICODE */
                              sizeof (_TCHAR) + sizeof (_TCHAR))) == NULL)
      return;
    /* Expand message */
    /* _vstprintf is broken because vsprintf and vswprintf take different count
     * of arguments and we do not want non-standard code either */
    if (_vstprintf(expandedMessage,
#if defined UNICODE || _UNICODE
                   len + 1,
#endif /* UNICODE || _UNICODE */
                   message,
                   arglist) < 0) {
      HeapFree(processHeap, 0, expandedMessage);
      va_end(arglist);
      SetLastError(ERROR_INVALID_PARAMETER);
      return;
    }
    va_end(arglist);
    if (error) {
      LPTSTR systemMessage = NULL;
      // const _TCHAR* formatString = _T("[31m%s: %s[39m");
      const _TCHAR* formatString = _T("%s: %s");

      if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        (LONG)error > 0 ? error : -error,
                        0,
                        (LPTSTR)&systemMessage,
                        0x10000,
                        NULL) != 0u && systemMessage != NULL) {

        _TCHAR* tmp = NULL;
        /* Reallocate the size of expanded message */
        if ((tmp =
             (_TCHAR*)HeapReAlloc(processHeap,
                                  HEAP_ZERO_MEMORY,
                                  expandedMessage,
#if defined UNICODE || _UNICODE
                                  (len = _scwprintf(formatString,
                                                    expandedMessage,
                                                    systemMessage)) *
#else
                                  _scprintf(formatString,
                                            expandedMessage,
                                            systemMessage) *
#endif /* UNICODE || UNICODE */
                                 sizeof (_TCHAR) +
                                 sizeof (_TCHAR))) == NULL) {
          LocalFree(systemMessage);
          HeapFree(processHeap, 0, expandedMessage);
          return;
        }
        /* tmp is not NULL so it is safe to overwrite expandedMessage */
        expandedMessage = tmp;
        if (_stprintf(expandedMessage,
#if defined UNICODE || _UNICODE
                      len + 1,
#endif /* UNICODE || UNICODE */
                      formatString,
                      expandedMessage,
                      systemMessage) < 0) {
          LocalFree(systemMessage);
          HeapFree(processHeap, 0, expandedMessage);
          SetLastError(ERROR_INVALID_PARAMETER);
          return;
        }
        LocalFree(systemMessage);
      }
      /* Output expanded error message to console on stderr in color code if
       * possible. Otherwise simply output to stderr */
      if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
        HANDLE stdoutConsoleHandle;

        GetConsoleScreenBufferInfo(stdoutConsoleHandle =
                                   GetStdHandle(STD_OUTPUT_HANDLE),
                                   &consoleScreenBufferInfo);
        /* Set foreground text color on console to:
         *   bright red (error) or
         *   bright yellow (warning) */
        SetConsoleTextAttribute(stdoutConsoleHandle,
                                ((LONG)error > 0 ?
                                  FOREGROUND_RED :
                                  FOREGROUND_RED | FOREGROUND_GREEN) |
                                FOREGROUND_INTENSITY ^
                                consoleScreenBufferInfo.wAttributes);
        WriteConsole(stdoutConsoleHandle,
                     expandedMessage,
#if defined UNICODE || _UNICODE
                     len,
#else
                     strlen(expandedMessage),
#endif /* UNICODE || _UNICODE */
                     NULL,
                     NULL);
        /* Reset colors on console */
        SetConsoleTextAttribute(stdoutConsoleHandle,
                                consoleScreenBufferInfo.wAttributes);
        /* No fflush() required because console flushes immediately */
      } else {
        /* Output the expanded message as an error */
        _fputts(expandedMessage, stderr);
        /* Flush the stream just in case something goes wrong and the error
         * message may not be read */
        fflush(stderr);
      }
      /* Log the expanded message to an eventually registered event logging
       * source */
      if (eventSource != NULL)
        ReportEvent(eventSource,
                    (LONG)error > 0 ?
                      EVENTLOG_ERROR_TYPE : EVENTLOG_WARNING_TYPE,
                    0,
                    (LONG)error > 0 ?
                      0xE0000000u : 0xA0000000u,
                    currentUserSID,
                    1,
                    0,
                    (LPCTSTR*)&expandedMessage,
                    NULL);
    } else {
      /* No error supplied so just output the supplied information message */
      /* Output expanded message to console on stdout if possible, otherwise
       * simply output to stdout */
      if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
                     expandedMessage,
#if defined UNICODE || _UNICODE
                     len,
#else
                     strlen(expandedMessage),
#endif /* UNICODE || _UNICODE */
                     NULL,
                     NULL);
      } else {
        _fputts(expandedMessage, stdout);
      }
      /* Flush the stream just in case something goes wrong an the error
       * message could not be read */
      fflush(stdout);
      /* Log the expanded message to an eventually registered event logging
       * source */
      if (eventSource != NULL)
        ReportEvent(eventSource,
                    EVENTLOG_INFORMATION_TYPE,
                    0,
                    0x60000000u,
                    currentUserSID,
                    1,
                    0,
                    (LPCTSTR*)&expandedMessage,
                    NULL);
    }
    HeapFree(processHeap, 0, expandedMessage);
  } else {
  /* No message supplied so just output the system defined message */
    LPTSTR systemMessage = NULL;

    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      (LONG)error > 0 ? error : -error,
                      0,
                      (LPTSTR)&systemMessage,
                      0x10000,
                      NULL) == 0) return;
    {
       /* Output system message to console on stderr in color code if
        * possible. Otherwise simply output to stderr */
      if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
        HANDLE consoleHandle;

        if (GetConsoleScreenBufferInfo(consoleHandle =
                                       GetStdHandle(error ?
                                                      STD_ERROR_HANDLE :
                                                      STD_OUTPUT_HANDLE),
                                       &consoleScreenBufferInfo) == FALSE) {
          LocalFree(systemMessage);
          return;
        }
        /* Set foreground text color on console to:
         *   bright red (error) or
         *   bright yellow (warning) */
        if (SetConsoleTextAttribute(consoleHandle,
                                    ((LONG)error > 0 ?
                                      FOREGROUND_RED :
                                      FOREGROUND_RED | FOREGROUND_GREEN) |
                                    FOREGROUND_INTENSITY ^
                                    consoleScreenBufferInfo.wAttributes) ==
            FALSE) {
          LocalFree(systemMessage);
          return;
        }
        if (WriteConsole(consoleHandle,
                         systemMessage,
                         _tcslen(systemMessage),
                         NULL,
                         NULL) == FALSE) {
          LocalFree(systemMessage);
          /* Reset colors on console */
          SetConsoleTextAttribute(consoleHandle,
                                  consoleScreenBufferInfo.wAttributes);
          return;
        }
        /* Reset colors on console */
        if (SetConsoleTextAttribute(consoleHandle,
                                    consoleScreenBufferInfo.wAttributes) ==
            FALSE) {
          LocalFree(systemMessage);
          return;
        }
        /* No fflush() required because console flushes immediately */
      } else {
        FILE* stream;
        _fputts(systemMessage, stream = error ? stderr : stdout);
        /* Flush the stream just in case something goes wrong and the error
         * message may not be read */
        fflush(stream);
      }
    }
    if (eventSource != NULL)
      ReportEvent(eventSource,
                  error ?
                    (LONG)error > 0 ?
                      EVENTLOG_ERROR_TYPE : EVENTLOG_WARNING_TYPE :
                    EVENTLOG_INFORMATION_TYPE,
                  0,
                  error ?
                    (LONG)error > 0 ?
                      0xE0000000u : 0xA0000000u :
                    0x60000000u,
                  currentUserSID,
                  1,
                  0,
                  (LPCTSTR*)&systemMessage,
                  NULL);
    LocalFree(systemMessage);
  }
}