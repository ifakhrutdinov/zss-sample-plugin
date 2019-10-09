

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
#include "logging.h"
#include "zis/parm.h"
#include "zis/plugin.h"
#include "zis/server.h"
#include "zis/service.h"

#include "aux-manager.h"

#include "plugin-utils.h"
#include "sdumpx-service.h"

#define RC_SMPL_OK  RC_ZIS_PLUGIN_OK
#define RC_SMPL_ERR 888

/* TODO hack, fix zis/parm.c */
int cmsAddConfigParm(CrossMemoryServer *server,
                     const char *name, const void *value,
                     CrossMemoryServerParmType type) { return 0; }

ZOWE_PRAGMA_PACK

typedef struct SamplePluginData_tag {
  uint64_t loadCount;
  ZISParmSet *parms;
  ZISAUXManager auxMgr;
} SamplePluginData;

ZOWE_PRAGMA_PACK_RESET

const ZISAUXNickname  SMPL_AUX_GUEST_NICKNAME     = {"ZSMP"};

#define AUX_STC_NAME_PARM "ZSMP.AUXNAME"

static int initAUXManager(SamplePluginData *pluginData) {

  ZISAUXSTCName hostName;
  const char *hostSTCString =
      zisGetParmValue(pluginData->parms, AUX_STC_NAME_PARM);
  if (hostSTCString && strlen(hostSTCString) > sizeof(hostName.name)) {
    LOG_DEBUG(AUX_STC_NAME_PARM" not valid");
    return RC_SMPL_ERR;
  }
  memset(&hostName, ' ', sizeof(hostName.name));
  memcpy(hostName.name, hostSTCString, strlen(hostSTCString));

  int cleanRSN = 0;
  int cleanRC = zisauxMgrClean(&pluginData->auxMgr, &cleanRSN);
  if (cleanRC != RC_ZISAUX_OK) {
//    LOG_DEBUG("AUX manager not cleaned, RC = %d, RSN = 0x%08X\n",
//              cleanRC, cleanRSN);
  }

  int auxMgrInitRSN = 0;
  int auxMgrInitRC = zisauxMgrInit(&pluginData->auxMgr, &auxMgrInitRSN);
  if (auxMgrInitRC != RC_ZISAUX_OK) {
    LOG_DEBUG("AUX manager initialization failed, RC = %d, RSN = 0x%08X\n",
              auxMgrInitRC, auxMgrInitRSN);
    return RC_SMPL_ERR;
  }

  if (hostSTCString) {
    zisauxMgrSetHostSTC(&pluginData->auxMgr, hostName);
  }

  return RC_SMPL_OK;
}

static int startAUX(SamplePluginData *pluginData) {

  int startRSN = 0;
  int startRC = zisauxMgrStartGuest(&pluginData->auxMgr,
                                    SMPL_AUX_GUEST_NICKNAME,
                                    (ZISAUXModule){"ZWESSMPA"},
                                    NULL, &startRSN, 0);
  if (startRC != RC_ZISAUX_OK) {
    LOG_DEBUG("AUX guest start up failed, RC = %d, RSN = 0x%08X\n",
              startRC, startRSN);
    return RC_SMPL_ERR;
  }

  return RC_SMPL_OK;
}

static int stopAUX(SamplePluginData *pluginData) {

  int stopRSN = 0;
  int stopRC = zisauxMgrSendTermSignal(&pluginData->auxMgr,
                                       SMPL_AUX_GUEST_NICKNAME,
                                       &stopRSN, 0);
  if (stopRC == RC_ZISAUX_OK) {

    int waitRSN = 0;
    int waitRC = zisauxMgrWaitForTerm(&pluginData->auxMgr,
                                      SMPL_AUX_GUEST_NICKNAME,
                                      &waitRSN, 0);

    if (waitRC != RC_ZISAUX_OK) {
      LOG_DEBUG("AUX guest has not been stopped, wait RC = %d, RSN = 0x%08X\n",
                waitRC, waitRSN);
      return RC_SMPL_ERR;
    }

  } else {
    LOG_DEBUG("AUX guest has been stopped, RC = %d, RSN = 0x%08X\n",
              stopRC, stopRSN);
    return RC_SMPL_ERR;
  }

  return RC_SMPL_OK;
}

static int init(struct ZISContext_tag *context,
                ZISPlugin *plugin,
                ZISPluginAnchor *anchor) {

  SamplePluginData *pluginData = (SamplePluginData *)&anchor->pluginData;
  pluginData->parms = context->parms;

  int auxInitRC = initAUXManager(pluginData);
  if (auxInitRC != RC_SMPL_OK) {
    return auxInitRC;
  }

  startAUX(pluginData);

  pluginData->loadCount++;

  return RC_ZIS_PLUGIN_OK;
}

static int term(struct ZISContext_tag *context,
                ZISPlugin *plugin,
                ZISPluginAnchor *anchor) {

  SamplePluginData *pluginData = (SamplePluginData *)&anchor->pluginData;

  stopAUX(pluginData);

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

  SamplePluginData *pluginData =
      (SamplePluginData *)&anchor->pluginData.data;

  if (!strcmp(command->commandVerb, "D") ||
      !strcmp(command->commandVerb, "DIS") ||
      !strcmp(command->commandVerb, "DISPLAY")) {

    if (command->argCount == 0) {

      LOG_INFO("ZSS sample plug-in load count = %llu\n", pluginData->loadCount);

      *status = CMS_MODIFY_COMMAND_STATUS_CONSUMED;

    }

  } else if (!strcmp(command->commandVerb, "SET")) {

    if (command->argCount == 2 && !strcmp(command->args[0], "AUX")) {

      int rc, rsn;

      char stringToReverse[1024];
      strcpy(stringToReverse, command->args[1]);

      ZISAUXRequestPayload request;
      rc = zisauxMgrInitRequestPayload(&pluginData->auxMgr, &request,
                                       stringToReverse, sizeof(stringToReverse),
                                       true, &rsn);
      if (rc != RC_ZISAUX_OK) {
        LOG_ERROR("zisauxMgrInitRequestPayload rc = %d, rsn = %08X\n", rc, rsn);
        return RC_ZIS_PLUGIN_OK;
      }

      ZISAUXRequestResponse response;
      rc = zisauxMgrInitRequestResponse(&pluginData->auxMgr, &response,
                                        sizeof(stringToReverse), &rsn);
      if (rc != RC_ZISAUX_OK) {
        LOG_ERROR("zisauxMgrInitRequestResponse rc = %d, rsn = %08X\n", rc, rsn);
        goto request_initialized;
      }

      rc = zisauxMgrSendWork(&pluginData->auxMgr, (ZISAUXNickname){"ZSMP"},
                             &request, &response, &rsn, 0);
      if (rc != RC_ZISAUX_OK) {
        LOG_ERROR("zisauxMgrSendWork rc = %d, rsn = %08X\n", rc, rsn);
        goto response_initialized;
      }

      char result[sizeof(stringToReverse)] = {0};
      rc = zisauxMgrCopyRequestResponseData(result, sizeof(result), true,
                                            &response, &rsn);
      if (rc == RC_ZISAUX_OK) {
        LOG_INFO("Response from AUX: \'%s\'\n", result);
      } else {
        LOG_ERROR("zisauxMgrCopyRequestResponseData rc = %d, rsn = %08X\n", rc, rsn);
      }

      response_initialized:
      rc = zisauxMgrCleanRequestPayload(&pluginData->auxMgr, &request, &rsn);
      if (rc != RC_ZISAUX_OK) {
        LOG_ERROR("zisauxMgrCleanRequestPayload rc = %d, rsn = %08X\n", rc, rsn);
      }

      request_initialized:
      rc = zisauxMgrCleanRequestResponse(&pluginData->auxMgr, &response, &rsn);
      if (rc != RC_ZISAUX_OK) {
        LOG_ERROR("zisauxMgrCleanRequestResponse rc = %d, rsn = %08X\n", rc, rsn);
      }

      *status = CMS_MODIFY_COMMAND_STATUS_CONSUMED;

    }

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

