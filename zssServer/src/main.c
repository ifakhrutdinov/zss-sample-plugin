

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <errno.h>

#include "zowetypes.h"
#include "alloc.h"
#include "utils.h"
#include "collections.h"
#include "bpxnet.h"
#include "socketmgmt.h"
#include "le.h"
#include "json.h"
#include "httpserver.h"
#include "dataservice.h"
#include "serviceUtils.h"
#include "utils.h"
#include "zos.h"

#include "zis/client.h"
#include "zis/service.h"

#include "sdumpx-service.h"

static void printASCBs(jsonPrinter *printer) {
  jsonStartArray(printer, "addressSpaces");
  {
    // walk ASCB chain
    CVT *cvt = getCVT();
    ASVT *asvt = (ASVT*)cvt->cvtasvt;
    ASCB *currASCB = (ASCB*)asvt->asvtenty;
    while (currASCB) {

      // print ASCB
      jsonStartObject(printer, NULL);
      jsonAddUnterminatedString(printer, "jobName", getASCBJobname(currASCB), 8);
      char asidString[8];
      sprintf(asidString, "%4.4hx", currASCB->ascbasid);
      jsonAddString(printer, "asid", asidString);
      jsonEndObject(printer);

      currASCB = currASCB->ascbfwdp;
    }
  }
  jsonEndArray(printer);
}

static int serveAddressSpaces(HttpService *service, HttpResponse *response) {

  HttpRequest *request = response->request;
  if (strcmp(request->method, methodGET)) {
    respondWithError(response, HTTP_STATUS_BAD_REQUEST, "Unsupported request");
    return 0;
  }

  jsonPrinter *printer = respondWithJsonPrinter(response);
  setResponseStatus(response, HTTP_STATUS_OK, "Ok");
  setDefaultJSONRESTHeaders(response);
  writeHeader(response);

  jsonStart(printer);
  printASCBs(printer);
  jsonEnd(printer);

  finishResponse(response);

  return 0;
}

static char *getURLSegment(HttpRequest *request, int number) {

  StringListElt *currSegment = firstStringListElt(request->parsedFile);

  char *segment = NULL;
  for (int i = 0; i < number; i++) {
    if (currSegment == NULL) {
      return NULL;
    }
    segment = currSegment->string;
    currSegment = currSegment->next;
  }

  return segment;
}

static char *getFirstServicePathSegment(HttpRequest *request) {
  // skip ZLUX/plugins/org.zowe.zsssample/services/sample/{service}
  return getURLSegment(request, 7);
}

static short getASIDParm(HttpRequest *request) {

  char *startASIDString = getFirstServicePathSegment(request);
  if (startASIDString == NULL) {
    return -1;
  }

  char *endASIDString = NULL;
  errno = 0;
  long asid = strtol(startASIDString, &endASIDString, 16);

  if (startASIDString == endASIDString || errno != 0) {
    return -1;
  }

  if (asid < SHRT_MIN || SHRT_MAX < asid) {
    return -1;
  }

  return asid;
}

static char *getTitleParm(HttpRequest *request) {

  if (!request->contentBody) {
    return NULL;
  }

  char *nativeBody = copyStringToNative(request->slh, request->contentBody,
                                        strlen(request->contentBody));
  if (!nativeBody) {
    return NULL;
  }

  Json *body = jsonParseString(request->slh, nativeBody, NULL, 0);
  if (!body) {
    return NULL;
  }

  JsonObject *bodyObject = jsonAsObject(body);
  if (!bodyObject) {
    return NULL;
  }

  char *title = jsonObjectGetString(bodyObject, "title");

  return title;
}

static void respondWithSDUMPXData(HttpService *service, HttpResponse *response,
                                  short asid, const char *title) {

  ZISServiceStatus status = {0};

  CrossMemoryServerName *cms =
      getConfiguredProperty(service->server, HTTP_SERVER_PRIVILEGED_SERVER_PROPERTY);

  ZISDumpServiceParmList zisParmList = {
      .eyecatcher = ZIS_DUMP_SERVICE_PARMLIST_EYECATCHER,
      .version = ZIS_DUMP_SERVICE_PARMLIST_VERSION,
      .asid = asid,
      .sdumpxRC = 0,
  };

  memset(&zisParmList.titleSpacePadded, ' ',
         sizeof(zisParmList.titleSpacePadded));
  memcpy(&zisParmList.titleSpacePadded, title,
         min(strlen(title), sizeof(zisParmList.titleSpacePadded)));

  int zisRC = zisCallService(cms, &ZSS_DUMP_SERVICE_PATH, &zisParmList, &status);

  int httpStatus = HTTP_STATUS_OK;
  char *statusMessage = "Ok";
  if (zisRC != RC_ZIS_OK) {
    httpStatus = HTTP_STATUS_INTERNAL_SERVER_ERROR;
    statusMessage = "ZIS Error";
  }

  jsonPrinter *printer = respondWithJsonPrinter(response);
  setResponseStatus(response, httpStatus, statusMessage);
  setDefaultJSONRESTHeaders(response);
  writeHeader(response);

  jsonStart(printer);
  char asidString[8];
  sprintf(asidString, "%4.4hx", asid);
  jsonAddString(printer, "asid", asidString);
  jsonAddString(printer, "title", title ? (char *)title : "");
  jsonAddInt(printer, "zisRC", zisRC);
  jsonAddInt(printer, "cmsRC", status.cmsRC);
  jsonAddInt(printer, "serviceRC", status.serviceRC);
  jsonAddInt(printer, "sdumpxRC", zisParmList.sdumpxRC);
  jsonEnd(printer);

  finishResponse(response);

}

static int serveSDUMPX(HttpService *service, HttpResponse *response) {

  HttpRequest *request = response->request;
  if (strcmp(request->method, methodPOST)) {
    respondWithError(response, HTTP_STATUS_BAD_REQUEST, "Unsupported request");
    return 0;
  }

  short asid = getASIDParm(request);
  if (asid == -1) {
    respondWithError(response, HTTP_STATUS_BAD_REQUEST, "Bad ASID");
    return 0;
  }

  char *title = getTitleParm(request);

  respondWithSDUMPXData(service, response, asid, title);

  return 0;
}

static char *getServiceEndpoint(HttpRequest *request) {
  // skip ZLUX/plugins/org.zowe.zsssample/services/sample
  return getURLSegment(request, 6);
}

static int serveSampleData(HttpService *service, HttpResponse *response) {

  HttpRequest *request = response->request;

  char *serviceName = getServiceEndpoint(request);
  if (!strcmp(serviceName, "asinfo")) {
    return serveAddressSpaces(service, response);
  }
  if (!strcmp(serviceName, "dump")) {
    return serveSDUMPX(service, response);
  }

  respondWithError(response, HTTP_STATUS_NOT_FOUND, "Service not found");

  return 0;
}

#pragma export(zssSamplePluginServiceInstaller)

int zssSamplePluginServiceInstaller(DataService *dataService,
                                    HttpServer *server) {

  // ZLUX/plugins/org.zowe.zsssample/services/sample/**
  HttpService *service = makeHttpDataService(dataService, server);
  service->serviceFunction = serveSampleData;
  service->runInSubtask = FALSE;

  return 0;
}


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

