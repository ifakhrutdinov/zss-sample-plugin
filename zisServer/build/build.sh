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
-I $COMMON/h -I $ZSS/h -I $SAMPLE/zisServer/src -I $SAMPLE/zisServer/include \
-I $ZSS/zis-aux/include -I $ZSS/zis-aux/src)

ASFLAGS=(-mgoff -mobject -mflag=nocont --TERM --RENT)

LDFLAGS=(-V -b ac=1 -b rent -b case=mixed -b map -b xref -b reus)

xlc "${CFLAGS[@]}" -DCMS_CLIENT \
$COMMON/c/alloc.c \
$COMMON/c/as.c \
$COMMON/c/cmutils.c \
$COMMON/c/collections.c \
$COMMON/c/crossmemory.c \
$COMMON/c/isgenq.c \
$COMMON/c/le.c \
$COMMON/c/logging.c \
$COMMON/c/metalio.c \
$COMMON/c/pause-element.c \
$COMMON/c/pc.c \
$COMMON/c/qsam.c \
$COMMON/c/recovery.c \
$COMMON/c/scheduling.c \
$COMMON/c/shrmem64.c \
$COMMON/c/timeutls.c \
$COMMON/c/utils.c \
$COMMON/c/zos.c \
$COMMON/c/zvt.c \
$ZSS/c/zis/client.c \
$ZSS/c/zis/parm.c \
$ZSS/c/zis/plugin.c \
$ZSS/c/zis/service.c \
$ZSS/zis-aux/src/aux-manager.c \
$ZSS/zis-aux/src/aux-guest.c \
$ZSS/zis-aux/src/aux-utils.c \
$SAMPLE/zisServer/src/sdumpx-service.c \
$SAMPLE/zisServer/src/main-zis.c \
$SAMPLE/zisServer/src/main-aux.c \
 || exit 1

as "${ASFLAGS[@]}" -aegimrsx=alloc.asm alloc.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=as.asm as.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=cmutils.asm cmutils.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=collections.asm collections.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=crossmemory.asm crossmemory.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=isgenq.asm isgenq.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=le.asm le.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=logging.asm logging.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=metalio.asm metalio.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=pause-element.asm pause-element.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=pc.asm pc.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=qsam.asm qsam.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=recovery.asm recovery.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=scheduling.asm scheduling.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=shrmem64.asm shrmem64.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=timeutls.asm timeutls.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=utils.asm utils.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=zos.asm zos.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=zvt.asm zvt.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=client.asm client.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=parm.asm parm.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=plugin.asm plugin.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=service.asm service.s || exit 1

as "${ASFLAGS[@]}" -aegimrsx=aux-manager.asm aux-manager.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=aux-guest.asm aux-guest.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=aux-utils.asm aux-utils.s || exit 

as "${ASFLAGS[@]}" -aegimrsx=sdumpx-service.asm sdumpx-service.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=main-zis.asm main-zis.s || exit 1
as "${ASFLAGS[@]}" -aegimrsx=main-aux.asm main-aux.s || exit 1

export _LD_SYSLIB="//'SYS1.CSSLIB'://'CEE.SCEELKEX'://'CEE.SCEELKED'://'CEE.SCEERUN'://'CEE.SCEERUN2'://'CSF.SCSFMOD0'"

ld "${LDFLAGS[@]}" -e getPluginDescriptor \
-o $SAMPLE/zisServer/lib/zwessmpl \
alloc.o \
as.o \
cmutils.o \
collections.o \
crossmemory.o \
le.o \
logging.o \
metalio.o \
pause-element.o \
pc.o \
qsam.o \
recovery.o \
scheduling.o \
shrmem64.o \
timeutls.o \
utils.o \
zos.o \
zvt.o \
client.o \
parm.o \
plugin.o \
service.o \
aux-manager.o \
aux-utils.o \
sdumpx-service.o \
main-zis.o \
> ZWESSMPL.link || exit 1

ld "${LDFLAGS[@]}" -e getModuleDescriptor \
-o $SAMPLE/zisServer/lib/zwessmpa \
alloc.o \
cmutils.o \
collections.o \
crossmemory.o \
le.o \
logging.o \
metalio.o \
qsam.o \
recovery.o \
scheduling.o \
timeutls.o \
utils.o \
zos.o \
zvt.o \
aux-guest.o \
client.o \
parm.o \
main-aux.o \
> ZWESSMPA.link

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

