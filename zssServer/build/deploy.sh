# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

#!/bin/sh

BASEDIR=$(dirname "$0")
cd $BASEDIR

appServerDir=$1

if [ -z "$appServerDir" ]
then
  appServerDir=../../../zlux-app-server
fi

echo
echo "Info:  installing plugin to ${appServerDir}"
echo

cp ../../org.zowe.zsssample.json $appServerDir/deploy/instance/ZLUX/plugins/
if [[ $? -ne 0 ]]
then
  exit 8
fi

cp ../lib/zssSamplePlugin.so $appServerDir/bin/
if [[ $? -ne 0 ]]
then
  exit 8
fi

extattr +p $appServerDir/bin/zssSamplePlugin.so

echo "Info:  plugin deployed"
echo

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

