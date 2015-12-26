#include <msxml2.h>
#include "IcedTeaSax.h"

/* IJNLPContentHandler methods */
/* IUnknown methods */
HRESULT STDMETHODCALLTYPE JNLPContentHandler_QueryInterface(
  ISAXContentHandler* This,
  REFIID iid,
  void** object) {
}

ULONG STDMETHODCALLTYPE JNLPContentHandler_AddRef(ISAXContentHandler* This) {
}

ULONG STDMETHODCALLTYPE JNLPContentHandler_Release(ISAXContentHandler* This) {
}

/* ISAXContentHandler methods */
HRESULT STDMETHODCALLTYPE JNLPContentHandler_putDocumentLocator(
  ISAXContentHandler* This,
  ISAXLocator* locator) {
  if (This == NULL || locator == NULL) return E_INVALIDARG;

  if ((*(JNLPContentHandler*)This).locator)
    (*(*(*(JNLPContentHandler*)This).locator).lpVtbl).Release(
      (*(JNLPContentHandler*)This).locator);
  (*(JNLPContentHandler*)This).locator = locator;
  (*(*(*(JNLPContentHandler*)This).locator).lpVtbl).AddRef(
    (*(JNLPContentHandler*)This).locator);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_startDocument(
  ISAXContentHandler* This) {
  if (This == NULL) return E_INVALIDARG;

  return S_OK;
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_endDocument(
  ISAXContentHandler* This) {
  if (This == NULL) return E_INVALIDARG;

  return S_OK;
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_startPrefixMapping(
  ISAXContentHandler* This,
  const wchar_t* prefix,
  int prefixCharCount,
  const wchar_t* uri,
  int uriCharCount) {
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_endPrefixMapping(
  ISAXContentHandler* This,
  const wchar_t* prefix,
  int prefixCharCount) {
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_startElement(
  ISAXContentHandler* This,
  const wchar_t* namespaceURI,
  int namespaceURICharCount,
  const wchar_t* localName,
  int localNameCharCount,
  const wchar_t* qName,
  int qNameCharCount,
  ISAXAttributes* attributes) {
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_endElement(
  ISAXContentHandler* This,
  const wchar_t* namespaceURI,
  int namespaceURICharCount,
  const wchar_t* localName,
  int localNameCharCount,
  const wchar_t* qName,
  int qNameCharCount) {
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_characters(
  ISAXContentHandler* This,
  const wchar_t* chars,
  int charsCount) {
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_ignorableWhitespace(
  ISAXContentHandler* This,
  const wchar_t* whitespaces,
  int whitespacesCharCount) {
  if (This == NULL) return E_INVALIDARG;

  return S_OK;
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_processingInstruction(
  ISAXContentHandler* This,
  const wchar_t* target,
  int targetCharCount,
  const wchar_t* data,
  int dataCharCount) {
}

HRESULT STDMETHODCALLTYPE JNLPContentHandler_skippedEntity(
  ISAXContentHandler* This,
  const wchar_t* entityName,
  int entityNameCharCount) {
}

/* Custom methods */
HRESULT STDMETHODCALLTYPE JNLPContentHandler_GetIconURL(
  ISAXContentHandler* This,
  LPOLESTR* iconURL) {
  if (This == NULL || iconURL == NULL) return E_INVALIDARG;

  return E_NOTIMPL;
}


/* IJNLPErrorHandler methods */
/* IUnknown methods */
HRESULT STDMETHODCALLTYPE JNLPErrorHandler_QueryInterface(
  ISAXErrorHandler*,
  REFIID,
  void**);
ULONG STDMETHODCALLTYPE JNLPErrorHandler_AddRef(ISAXErrorHandler*);
ULONG STDMETHODCALLTYPE JNLPErrorHandler_Release(ISAXErrorHandler*);
/* ISAXErrorHandler methods */
HRESULT STDMETHODCALLTYPE JNLPErrorHandler_error(
  ISAXErrorHandler*,
  ISAXLocator*,
  const wchar_t*,
  HRESULT);
HRESULT STDMETHODCALLTYPE JNLPErrorHandler_fatalError(
  ISAXErrorHandler*,
  ISAXLocator*,
  const wchar_t*,
  HRESULT);
HRESULT STDMETHODCALLTYPE JNLPErrorHandler_ignorableWarning(
  ISAXErrorHandler*,
  ISAXLocator*,
  const wchar_t*,
  HRESULT);