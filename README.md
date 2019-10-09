This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.

# ZSS sample plug-in

## Overview

A sample plug-in that allows:
* Returning z/OS address spaces using the following REST endpoint

```
GET http://zsshost:zssport/ZLUX/plugins/org.zowe.zsssample/services/sample/asinfo
```
* Dumping z/OS address spaces using the following REST endpoint

```
POST http://zsshost:zssport/ZLUX/plugins/org.zowe.zsssample/services/sample/dump/{asid}
Optional body:
{
  "title": "svc-dump-title"
}
```

### Getting the code:
* Use the following command to clone this repository with all the dependencies:
```
git clone --recursive git@github.com:ifakhrutdinov/zss-sample-plugin.git
```

### Building and deploying the ZSS plug-in:
  * Go to the build directory in zssServer/build
  * Run build.sh
  * Run deploy.sh
  * Restart your ZSS

### Building and deploying the ZIS plug-in:
  * Go to the build directory in zisServer/build
  * Run build.sh (**WARNING: the SDUMPX call is made only when compiled with ZSS_SAMPLE_PLUGIN_SDUMPX_ENABLED**)
  * Copy the ZIS plugin load module to your server's STEPLIB dataset
  ```
  cp -X zisServer/lib/zwessmpl "//'USR.LOADLIB'"
  ```
  * Copy the AUX guest module to your server's STEPLIB dataset
  ```
  cp -X zisServer/lib/zwessmpa "//'USR.LOADLIB'"
  ```
  * Add the following like to your server's PARMLIB member
  ```
  ZWES.PLUGIN.ZSMP=ZWESSMPL
  ```
  * If you use a custom AUX server STC, add the following like to your server's PARMLIB member
  ```
ZSMP.AUXNAME=your-aux-stc-name
  ```
  * Restart the cross-memory server

This program and the accompanying materials are
made available under the terms of the Eclipse Public License v2.0 which accompanies
this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html

SPDX-License-Identifier: EPL-2.0

Copyright Contributors to the Zowe Project.
