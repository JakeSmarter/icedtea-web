#ifndef _ICEDTEASAX_H_
#define _ICEDTEASAX_H_

#include <msxml2.h>

/* IJNLPContentHandler methods */
/* IUnknown methods */
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_QueryInterface(
  ISAXContentHandler*,
  REFIID,
  void**);
extern ULONG STDMETHODCALLTYPE JNLPContentHandler_AddRef(ISAXContentHandler*);
extern ULONG STDMETHODCALLTYPE JNLPContentHandler_Release(ISAXContentHandler*);
/* ISAXContentHandler methods */
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_putDocumentLocator(
  ISAXContentHandler*,
  ISAXLocator*);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_startDocument(
  ISAXContentHandler*);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_endDocument(
  ISAXContentHandler*);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_startPrefixMapping(
  ISAXContentHandler*,
  const wchar_t*,
  int,
  const wchar_t*,
  int);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_endPrefixMapping(
  ISAXContentHandler*,
  const wchar_t*,
  int);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_startElement(
  ISAXContentHandler*,
  const wchar_t*,
  int,
  const wchar_t*,
  int,
  const wchar_t*,
  int,
  ISAXAttributes*);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_endElement(
  ISAXContentHandler*,
  const wchar_t*,
  int,
  const wchar_t*,
  int,
  const wchar_t*,
  int);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_characters(
  ISAXContentHandler*,
  const wchar_t*,
  int);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_ignorableWhitespace(
  ISAXContentHandler*,
  const wchar_t*,
  int);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_processingInstruction(
  ISAXContentHandler*,
  const wchar_t*,
  int,
  const wchar_t*,
  int);
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_skippedEntity(
  ISAXContentHandler*,
  const wchar_t*,
  int);
/* Custom methods */
extern HRESULT STDMETHODCALLTYPE JNLPContentHandler_GetIconURL(
  ISAXContentHandler*,
  LPOLESTR*);

typedef struct {
  const ISAXContentHandlerVtbl* lpVtbl;
  ISAXLocator* locator;
  volatile ULONG refCount;
  HANDLE refCountMutex;
  LPOLESTR iconURL;
} JNLPContentHandler;

/* IJNLPErrorHandler methods */
/* IUnknown methods */
extern HRESULT STDMETHODCALLTYPE JNLPErrorHandler_QueryInterface(
  ISAXErrorHandler*,
  REFIID,
  void**);
extern ULONG STDMETHODCALLTYPE JNLPErrorHandler_AddRef(ISAXErrorHandler*);
extern ULONG STDMETHODCALLTYPE JNLPErrorHandler_Release(ISAXErrorHandler*);
/* ISAXErrorHandler methods */
extern HRESULT STDMETHODCALLTYPE JNLPErrorHandler_error(
  ISAXErrorHandler*,
  ISAXLocator*,
  const wchar_t*,
  HRESULT);
extern HRESULT STDMETHODCALLTYPE JNLPErrorHandler_fatalError(
  ISAXErrorHandler*,
  ISAXLocator*,
  const wchar_t*,
  HRESULT);
extern HRESULT STDMETHODCALLTYPE JNLPErrorHandler_ignorableWarning(
  ISAXErrorHandler*,
  ISAXLocator*,
  const wchar_t*,
  HRESULT);

typedef struct {
  const ISAXErrorHandlerVtbl* lpVtbl;
  volatile ULONG refCount;
  HANDLE refCountMutex;
} JNLPErrorHandler;
#endif /* _ICEDTEASAX_H_ */