#ifndef PLUGIN_API_HANDLE_H
#define PLUGIN_API_HANDLE_H

#include "PluginAPI.h"

VMAPI getHandleAPI(void);

// Expose the internal registry function for the main engine to use
int api_registerType(const char *typeName);

#endif
