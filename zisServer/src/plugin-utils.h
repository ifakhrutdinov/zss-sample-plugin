

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

#ifndef ZISSERVER_SRC_PLUGIN_UTILS_H_
#define ZISSERVER_SRC_PLUGIN_UTILS_H_

#define DBG_MSG_PREFIX      "SMPL0000I"

#define LOG_DEBUG($fmt, ...) \
  zowelog(NULL, LOG_COMP_STCBASE, ZOWE_LOG_DEBUG, \
          DBG_MSG_PREFIX" "$fmt, ##__VA_ARGS__)

#define LOG_ERROR($fmt, ...) \
  zowelog(NULL, LOG_COMP_STCBASE, ZOWE_LOG_SEVERE, \
          DBG_MSG_PREFIX" "$fmt, ##__VA_ARGS__)

#define LOG_INFO($fmt, ...) \
  zowelog(NULL, LOG_COMP_STCBASE, ZOWE_LOG_INFO, \
          DBG_MSG_PREFIX" "$fmt, ##__VA_ARGS__)

#define DUMP_DEBUG($data, $size) \
  zowedump(NULL, LOG_COMP_STCBASE, ZOWE_LOG_INFO, $data, $size)

#endif /* ZISSERVER_SRC_PLUGIN_UTILS_H_ */


/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

  SPDX-License-Identifier: EPL-2.0

  Copyright Contributors to the Zowe Project.
*/

