

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#include <metal/metal.h>
#include <metal/stddef.h>
#include <metal/string.h>

#include "zowetypes.h"
#include "crossmemory.h"
#include "zis/plugin.h"
#include "zis/server.h"
#include "zis/service.h"

#include "sdumpx-service.h"

typedef struct SamplePluginData_tag {
  uint64_t loadCount;
} SamplePluginData;

static int init(struct ZISContext_tag *context,
                ZISPlugin *plugin,
                ZISPluginAnchor *anchor) {

  SamplePluginData *pluginData = (SamplePluginData *)&anchor->pluginData;
  pluginData->loadCount++;

  return RC_ZIS_PLUGIN_OK;
}

static int term(struct ZISContext_tag *context,
                ZISPlugin *plugin,
                ZISPluginAnchor *anchor) {

  return RC_ZIS_PLUGIN_OK;
}

static int handleCommands(struct ZISContext_tag *context,
                          ZISPlugin *plugin,
                          ZISPluginAnchor *anchor,
                          const CMSModifyCommand *command,
                          CMSModifyCommandStatus *status) {

  if (command->commandVerb == NULL) {
    return RC_ZIS_PLUGIN_OK;
  }

  if (command->argCount != 0) {
    return RC_ZIS_PLUGIN_OK;
  }

  if (!strcmp(command->commandVerb, "D") ||
      !strcmp(command->commandVerb, "DIS") ||
      !strcmp(command->commandVerb, "DISPLAY")) {

    SamplePluginData *pluginData = (SamplePluginData *)&anchor->pluginData;

    cmsPrintf(&context->cmServer->name,
              "ZSS sample plug-in load count = %llu\n", pluginData->loadCount);

    *status = CMS_MODIFY_COMMAND_STATUS_PROCESSED;

  }

  return RC_ZIS_PLUGIN_OK;
}

#define ZSS_SAMPLE_PLUGIN_VERSION         1
#define ZSS_SAMPLE_PLUGIN_SERVICE_COUNT   1

ZISPlugin *getPluginDescriptor(void) {

  ZISPlugin *plugin = zisCreatePlugin((ZISPluginName){ "ZSS-SAMPLE      " },
                                      (ZISPluginNickname){ "ZSMP" },
                                      init, term, handleCommands,
                                      ZSS_SAMPLE_PLUGIN_VERSION,
                                      ZSS_SAMPLE_PLUGIN_SERVICE_COUNT,
                                      ZIS_PLUGIN_FLAG_LPA);
  if (plugin == NULL) {
    return NULL;
  }

  ZISService service1 = zisCreateCurrentPrimaryService(
      ZSS_DUMP_SERVICE_PATH.serviceName,
      NULL, NULL,
      zissvcServeSDUMPX,
      ZIS_DUMP_SERVICE_VERSION
  );

  int addRC = zisPluginAddService(plugin, service1);
  if (addRC != RC_ZIS_PLUGIN_OK) {
    return NULL;
  }

  return plugin;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

