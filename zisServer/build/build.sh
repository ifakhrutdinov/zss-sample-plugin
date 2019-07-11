# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

#!/bin/bash

COMMON=../../../deps/zowe-common-c
ZSS=../../../deps/zss
SAMPLE=../../../

mkdir -p ../lib || exit 1
mkdir -p tmp || exit 1

cd tmp

CFLAGS=(-S -M -qmetal -q64 -DSUBPOOL=132 -DMETTLE=1 -DMSGPREFIX='"IDX"'
-qreserved_reg=r12
-Wc,"arch(8),agg,exp,list(),so(),off,xref,roconst,longname,lp64" 
-I $COMMON/h -I $ZSS/h -I $SAMPLE/zisServer/src -I $SAMPLE/zisServer/include)

ASFLAGS=(-mgoff -mobject -mflag=nocont --TERM --RENT)

LDFLAGS=(-V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus)

xlc "${CFLAGS[@]}" -DCMS_CLIENT \
$COMMON/c/alloc.c \
$COMMON/c/cmutils.c \
$COMMON/c/crossmemory.c \
$COMMON/c/metalio.c \
$COMMON/c/qsam.c \
$COMMON/c/timeutls.c \
$COMMON/c/utils.c \
$COMMON/c/zos.c \
$COMMON/c/zvt.c \
$ZSS/c/zis/plugin.c \
$ZSS/c/zis/service.c \
$SAMPLE/zisServer/src/sdumpx-service.c \
$SAMPLE/zisServer/src/main.c || exit 1

as "${ASFLAGS[@]}" -aegimrsx=alloc.asm alloc.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=cmutils.asm cmutils.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=crossmemory.asm crossmemory.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=metalio.asm metalio.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=qsam.asm qsam.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=timeutls.asm timeutls.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=utils.asm utils.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=zos.asm zos.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=zvt.asm zvt.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=plugin.asm plugin.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=service.asm service.s || exit 1

as "${ASFLAGS[@]}" -aegimrsx=sdumpx-service.asm sdumpx-service.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=main.asm main.s || exit 1

export _LD_SYSLIB="//'SYS1.CSSLIB'://'CEE.SCEELKEX'://'CEE.SCEELKED'://'CEE.SCEERUN'://'CEE.SCEERUN2'://'CSF.SCSFMOD0'"

ld "${LDFLAGS[@]}" -e getPluginDescriptor \
-o $SAMPLE/zisServer/lib/zwessmpl \
alloc.o \
cmutils.o \
crossmemory.o \
metalio.o \
qsam.o \
timeutls.o \
utils.o \
zos.o \
zvt.o \
plugin.o \
service.o \
sdumpx-service.o \
main.o \
> ZWESSMPL.link || exit 1

echo ""
echo "The plug-in has been successfully built, see README.md for the deploy instructions"
echo ""

exit 0

# This program and the accompanying materials are
# made available under the terms of the Eclipse Public License v2.0 which accompanies
# this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
# 
# SPDX-License-Identifier: EPL-2.0
# 
# Copyright Contributors to the Zowe Project.

