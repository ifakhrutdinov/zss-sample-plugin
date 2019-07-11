# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

#!/bin/sh

export _C89_LSYSLIB=CEE.SCEELKEX:CEE.SCEELKED:SYS1.CSSLIB:CSF.SCSFMOD0

BASEDIR=$(dirname "$0")
cd $BASEDIR

mkdir ../lib 2>/dev/null
mkdir tmp 2>/dev/null

cd tmp

rm ../../lib/zssSamplePlugin.so 2>/dev/null

ZOWECOMMON=../../../deps/zowe-common-c
ZSS=../../../deps/zss
SAMPLE=../../../

echo
echo "Info:  building Rocket discovery shared library"
echo "  ZOWECOMMON=${ZOWECOMMON}"
echo "  ZSS=${ZSS}"
echo

c89 -D_OPEN_THREADS -D_XOPEN_SOURCE=600 -DNOIBMHTTP \
"-Wa,goff" "-Wc,langlvl(EXTC99),float(HEX),agg,exp,list(),so(),search(),\
goff,xref,gonum,roconst,gonum,asm,asmlib('SYS1.MACLIB'),asmlib('CEE.SCEEMAC'),dll" \
 -I ../../src -I ${ZOWECOMMON}/h -I ${ZSS}/h -I ${SAMPLE}/zisServer/include \
 -o ../../lib/zssSamplePlugin.so \
../../src/zssServer.x \
../../src/main.c \
${ZOWECOMMON}/c/crossmemory.c \
${ZOWECOMMON}/c/zos.c \
${ZOWECOMMON}/c/zvt.c \
${ZSS}/c/zis/client.c \

if [[ $? -ne 0 ]]; then
  echo
  echo "Error:  build failed"
  echo
  exit 8
fi

extattr +p ../../lib/zssSamplePlugin.so

echo
echo "Info:  shared library has been successfully built"
echo

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

