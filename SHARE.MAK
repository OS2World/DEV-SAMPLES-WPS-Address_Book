#****************************************************************************
#  Dot directive definition area (usually just suffixes)
#****************************************************************************

.SUFFIXES:
.SUFFIXES: .c .obj .dll .csc .sc .h .ih .ph .psc .rc .res

#****************************************************************************
#  Environment Setup for the component(s).
#****************************************************************************

SOMTEMP = .\somtemp
SCPATH  = \toolkt20\sc
HPATH   = \toolkt20\c\os2h
LIBPATH = \toolkt20\os2lib

!if [set SMINCLUDE=.;$(SCPATH);] || \
    [set SMTMP=$(SOMTEMP)] || \
    [set SMEMIT=ih;h;ph;psc;sc;c]
!endif

!if [cd $(SOMTEMP)]
!  if [md $(SOMTEMP)]
!    error error creating $(SOMTEMP) directory
!  endif
!else
!  if [cd ..]
!    error - Couldn't cd .. from $(SOMTEMP) directory
!  endif
!endif

#
# Compiler/tools Macros
#
# /Kt+ tells which include files were included.

CC      = icc /c /Gd- /Se /Re /ss /Ms /Gm+ /Kb+ /N100

AFLAGS  = /Mx -t -z
ASM     = ml /c /Zm
LINK    = LINK386  $(LFLAGS)
LIBS    = DDE4MBS + OS2386
STLIBS  = DDE4SBS + OS2386
MTLIBS  = DDE4MBS + os2386
DLLLIBS = DDE4NBS + os2386
VLIBS   = DDE4SBS + vdh + os2386

CC      = $(CC) /Ge-
LINK    = LINK386
LDFLAGS = /noe /noi /map /nol /nod /exepack /packcode /packdata /align:16
DLLLIBS = $(MTLIBS) som.lib

#****************************************************************************
# Set up Macros that will contain all the different dependencies for the
# executables and dlls etc. that are generated.
#****************************************************************************

OBJS = list.obj somlist.obj notify.obj shrfoldr.obj \
       person.obj personv.obj addrbk.obj addrbkv.obj share.obj

#****************************************************************************
#   Setup the inference rules for compiling source code to
#   object code.
#****************************************************************************

.c.obj:
        $(CC) -I$(HPATH) -c $<

.csc.ih:
        sc -v $*.csc

.c.lst:
    $(CC) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
    $(CC) -Fo$*.obj $*.c

.asm.obj:
    $(ASM)   $*.asm

.ipf.hlp:
        ipfc $*.ipf /W3

.itl.hlp:
        cc  -P $*.itl
        ipfc $*.i
        del $*.i

.rc.res:
        rc -r -p -x $*.rc

all: share.dll shrinst.exe

#
# Specific Process Tag
#

list.obj: $$(@B).c $$(@B).h

somlist.ih:  $$(@B).csc

somlist.obj: $$(@B).c $$(@B).ih $$(@B).h  $$(@B).sc

notify.ih:  $$(@B).csc

notify.obj: $$(@B).c $$(@B).ih $$(@B).h  $$(@B).sc

person.ih:  $$(@B).csc

person.obj: $$(@B).c $$(@B).ih $$(@B).h  $$(@B).sc

personv.obj: $$(@B).c $$(@B).h

addrbk.ih:  $$(@B).csc

addrbk.obj: $$(@B).c $$(@B).ih $$(@B).h  $$(@B).sc

addrbkv.obj: $$(@B).c $$(@B).h

share.obj: $$(@B).c $$(@B).h  

share.res: share.rc share.dlg 
         rc -r $*.rc $*.res

share.dll: share.res $(OBJS)
         $(LINK) $(LDFLAGS) $(OBJS),$@,,$(DLLLIBS),share.def;
         mapsym share.map
         rc share.res $*.dll

shrinst.res: shrinst.dlg
         rc -r $*.dlg $*.res

shrinst.obj: shrinst.c
         icc /Ti+ /c /Ge+ /Gd- /Se /Sp2 /Re /Kb /ss /Ms /Gm+ shrinst.c

shrinst.exe: shrinst.obj shrinst.res
         link386 /st:40000 /noi /map /nol /nod /exepack /packcode /packdata /align:16 shrinst,shrinst,,som.lib os2386.lib dde4mbs.lib,shrinst.def
         rc shrinst.res shrinst.exe
