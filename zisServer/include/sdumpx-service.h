

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifndef ZISSERVER_INCLUDE_SDUMPX_SERVICE_H_
#define ZISSERVER_INCLUDE_SDUMPX_SERVICE_H_

#ifdef METTLE
#include <metal/metal.h>
#include <metal/stdint.h>
#else
#include <stdint.h>
#endif

#include "zis/service.h"

typedef struct ZISDumpServiceParmList_tag {
  char eyecatcher[8];
  int16_t version;
#define ZIS_DUMP_SERVICE_PARMLIST_EYECATCHER "ZISDMPSP"
#define ZIS_DUMP_SERVICE_PARMLIST_VERSION    1
  int16_t asid;
  char titleSpacePadded[100];
  int32_t sdumpxRC;
} ZISDumpServiceParmList;

static const ZISServicePath ZSS_DUMP_SERVICE_PATH = {
    .pluginName  = "ZSS-SAMPLE      ",
    .serviceName = "SDUMPX          ",
};

#define ZIS_DUMP_SERVICE_VERSION 1

int zissvcServeSDUMPX(const CrossMemoryServerGlobalArea *ga,
                      struct ZISServiceAnchor_tag *anchor,
                      struct ZISServiceData_tag *data,
                      void *serviceParmList);

#define RC_DUMPSVC_OK               (RC_ZIS_SRVC_OK)
#define RC_DUMPSVC_PARM_NULL        (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_DUMPSVC_BAD_EYECATCHER   (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_DUMPSVC_BAD_VERSION      (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_DUMPSVC_SDUMPX_FAILED    (ZIS_MAX_GEN_SRVC_RC + 4)

#endif /* ZISSERVER_INCLUDE_SDUMPX_SERVICE_H_ */


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

