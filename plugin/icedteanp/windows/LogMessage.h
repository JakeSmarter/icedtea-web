/* Include this header if you want log messages for IcedTea-Web. */

#ifndef _LOGMESSAGE_H_
#define _LOGMESSAGE_H_

#include <winnt.h>

/**
 * <summary>
 * Logs a formatted message to the standard output, standard error, and
 * possibly an event logging source.</summary> The event logging source is
 * named after the <c>PACKAGE_NAME</c> pre-processor macro.
 * <para>
 * On Windows XP and later, the
 * function attaches to the current process' or parent process' existing
 * console, if available, and outputs warnings in bright
 * <span style="color:yellow;background-color:black">yellow</span> color, and
 * errors in <span style="color:red;background-color:black">red</span>
 * color on the console. On Windows versions prior to Windows XP, no console is
 * created or attached. However, messages are still directed to <c>stdout</c>
 * and <c>stderr</c> streams appropriately.
 * </para>
 * <param name="error"> Windows system error code (<c>SCODE</c>) as returned
 * like by <c>GetLastError()</c>. Positive values issue an error, negative
 * values issue a warning, and <c>0</c> issues information level messages.
 * </param>
 * <param name="message">A <c>FormatMessage()</c> format string message.
 * </param>
 * <param name="...">A variable count of parameters referenced in the passed
 * <paramref name="message"/>string.</param>
 * <seealso cref="eventSource"/>
 * <seealso cref="AttachConsole()"/>
 */
VOID WINAPI LogMessage(DWORD error, const TCHAR* message, ...);

/**
 * <summary>
 * Global pointer to the process' current user SID defined in Dll.h.
 * </summary>
 * <seealso cref="SetCurrentUserSID()"/>
 * <seealso cref="LogMessage()"/>
 */
extern SID* currentUserSID;

/**
 * <summary>
 * Global event log source handle defined in Dll.h.
 * </summary>
 */
extern HANDLE eventSource;
#endif /* _LOGMESSAGE_H_ */
