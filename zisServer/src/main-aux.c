

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
#include "alloc.h"
#include "logging.h"
#include "metalio.h"

#include "aux-guest.h"
#include "aux-host.h"
#include "aux-utils.h"

#include "plugin-utils.h"

#define SAMPLE_GUEST_VERSION 1

#define RC_SMPL_AUX_OK RC_ZISAUX_OK

 /* TODO hack, fix zis/parm.c */
int cmsAddConfigParm(CrossMemoryServer *server,
                     const char *name, const void *value,
                     CrossMemoryServerParmType type) { return 0; }

static int init(struct ZISAUXContext_tag *context,
                void *moduleData, int traceLevel) {

  LOG_INFO("Init started");

  return RC_SMPL_AUX_OK;
}

static int term(struct ZISAUXContext_tag *context,
                void *moduleData, int traceLevel) {

  LOG_INFO("Term started");

  return RC_SMPL_AUX_OK;
}

static int handleCommands(struct ZISAUXContext_tag *context,
                          const ZISAUXCommand *rawCommand,
                          const CMSModifyCommand *parsedCommand,
                          ZISAUXCommandReponse *response,
                          int traceLevel) {

  if (parsedCommand->commandVerb == NULL) {
    return RC_SMPL_AUX_OK;
  }

  if (!strcmp(parsedCommand->commandVerb, "D") ||
      !strcmp(parsedCommand->commandVerb, "DISP") ||
      !strcmp(parsedCommand->commandVerb, "DISPLAY")) {

    LOG_INFO("I've got nothing to display");

    response->status = CMS_MODIFY_COMMAND_STATUS_CONSUMED;

  }

  return RC_SMPL_AUX_OK;
}

static int handleWork(struct ZISAUXContext_tag *context,
                      const ZISAUXRequestPayload *requestPayload,
                      ZISAUXRequestResponse *response,
                      int traceLevel) {

  const char *stringToRevert = requestPayload->data;
  size_t stringLength = strlen(stringToRevert);
  char *responseBuffer = response->data;

  LOG_INFO("Received string \'%s\' to revert", stringToRevert);

  for (int i = stringLength - 1; i >= 0; i--) {
    responseBuffer[stringLength - 1 - i] = stringToRevert[i];
  }

  LOG_INFO("Sending back result string \'%s\'", responseBuffer);

  return RC_SMPL_AUX_OK;
}

ZISAUXModuleDescriptor *getModuleDescriptor(void) {

  ZISAUXModuleDescriptor *descriptor =
      zisAUXMakeModuleDescriptor(init, term, NULL, handleCommands, handleWork,
                                 NULL, SAMPLE_GUEST_VERSION);

  return descriptor;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
