#include <windows.h>

#include "IcedTea-Web.h"
#include "LogMessage.h"
#include "config.h"

#include <npapi.h>
#include <npfunctions.h>
#include <jni.h>

typedef struct _pipeList_t {
  struct _pipeList_t* next;
  NPP instance;
  /** This plugin instance's pipe. Although this could also be stored in
   * NPP.pdata it is probably safer to store the pipe handle on the plugin
   * module's heap. If the browser process terminates abruptly the pipe
   * handle would get out of reach in NPP.pdata. */
  HANDLE pipe;
} pipeList_t;

/**
 * Holds a list of instances with pipes. See CreateNamesPipe() and/or
 * CreateFile().
 * TODO: See if we really need a list of pipes for every instance. Maybe one
 * pipe for all instances is enough. */
static pipeList_t* pipeList = NULL;

NPError WINAPI NP_Initialize(NPNetscapeFuncs* aNPNFuncs) {
  /* Set all function pointers to NULL */
  memset(aNPNFuncs + sizeof ((*aNPNFuncs).size) +
           sizeof ((*aNPNFuncs).version),
         '\x00',
         (*aNPNFuncs).size - sizeof ((*aNPNFuncs).size) -
           sizeof ((*aNPNFuncs).version));
  LogMessage(0, TEXT("%s has been called"), __FUNCTION__);
  return NPERR_NO_ERROR;
}

extern NPError WINAPI NP_GetEntryPoints(NPPluginFuncs* pFuncs) {
  LogMessage(0, TEXT("%s has been called"), __FUNCTION__);
  return NPERR_NO_ERROR;
}

NPError WINAPI NP_Shutdown() {
  LogMessage(0, TEXT("%s has been called"), __FUNCTION__);
  return NPERR_NO_ERROR;
}

NPError NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16_t mode,
                int16_t argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved) {
  return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance,
                    NPSavedData** save) {
  return NPERR_NO_ERROR;
}

NPError NPP_GetValue(NPP instance,
                     NPPVariable variable,
                     void* value) {
  switch (variable) {
    case NPPVpluginNameString:
      value = PACKAGE_NAME_STRING;
      break;
    case NPPVpluginDescriptionString:
      value = PACKAGE_DESCRIPTION_STRING;
      break;
    default:
    return NPERR_INVALID_PARAM;
  }
  return NPERR_NO_ERROR;
}

NPError NPP_SetValue(NPP instance,
                     NPNVariable variable,
                     void* value) {
  return NPERR_NO_ERROR;
}