

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
#include "cmutils.h"

#include "sdumpx-service.h"

static int checkParmList(const ZISDumpServiceParmList *parm) {

  if (memcmp(parm->eyecatcher, ZIS_DUMP_SERVICE_PARMLIST_EYECATCHER,
             sizeof(parm->eyecatcher))) {
    return RC_DUMPSVC_BAD_EYECATCHER;
  }

  if (parm->version > ZIS_DUMP_SERVICE_PARMLIST_VERSION) {
    return RC_DUMPSVC_BAD_VERSION;
  }

  return RC_DUMPSVC_OK;
}

static int sdumpx(short asid, const char title[]) {

  int rc = 0;
  char parmList[512] = {0};

  struct {
    char length;
    char title[100];
  } sdumpTitle;

  sdumpTitle.length = 100;
  memcpy(sdumpTitle.title, title, 100);

  /* WARNING: use at your own risk  */
#ifdef ZSS_SAMPLE_PLUGIN_SDUMPX_ENABLED
  __asm(
      ASM_PREFIX

      "         SDUMPX ASID=%[asid]"
      ",BRANCH=NO"
      ",HDRAD=(%[title])"
      ",TYPE=FAILRC"
      ",SDATA=("
      "ALLNUC,ALLPSA,COUPLE,CSA,"
      "GRSQ,IO,LPA,LSQA,NUC,PSA,"
      "RGN,SQA,SUM,SWA,TRT,XESDATA"
      ")"
      ",PLISTVER=3,MF=(E,%[parm])                                              \n"
      "         ST    15,%[rc]                                                 \n"
      : [rc]"=m"(rc)
      : [asid]"m"(asid), [title]"r"(&sdumpTitle), [parm]"m"(parmList)
      : "r0", "r1", "r14", "r15"
  );
#endif

  return rc;
}

int zissvcServeSDUMPX(const CrossMemoryServerGlobalArea *ga,
                      struct ZISServiceAnchor_tag *anchor,
                      struct ZISServiceData_tag *data,
                      void *serviceParmList) {

  if (!serviceParmList) {
    return RC_DUMPSVC_PARM_NULL;
  }

  ZISDumpServiceParmList localParmList;
  cmCopyFromSecondaryWithCallerKey(&localParmList, serviceParmList,
                                   sizeof(localParmList));

  int checkRC = checkParmList(&localParmList);
  if (checkRC != RC_DUMPSVC_OK) {
    return checkRC;
  }

  localParmList.sdumpxRC = sdumpx(localParmList.asid,
                                  localParmList.titleSpacePadded);

  cmCopyToSecondaryWithCallerKey(serviceParmList, &localParmList,
                                 sizeof(localParmList));

  if (localParmList.sdumpxRC != 0) {
    return RC_DUMPSVC_SDUMPX_FAILED;
  }

  return RC_DUMPSVC_OK;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/
