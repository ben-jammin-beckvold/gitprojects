# ---------------------------------------------------------------------------
VERSION = BCB.04.04
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = SDIAPP.exe
OBJFILES = SDIAPP.obj SDIMain.obj ftusbdev.obj test_bm.obj
RESFILES = sdiapp.res
DEFFILE =
RESDEPEN = $(RESFILES) SDIMain.dfm
LIBFILES =
LIBRARIES = vclx40.lib vcl40.lib
# SPARELIBS = vcl40.lib vclx40.lib vcldb40.lib vcldbx40.lib
SPARELIBS = vcl40.lib vclx40.lib
PACKAGES = VCL40.bpi VCLX40.bpi bcbsmp40.bpi dclocx40.bpi
PATHASM = .;
PATHCPP = .;
PATHPAS = .;
PATHRC = .;
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
SYSDEFINES = NO_STRICT
USERDEFINES =
# ---------------------------------------------------------------------------
CFLAG1 = -I..\..;$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl -O2 -Hc \
  -H=$(BCB)\lib\vcl40.csm -w -Ve -a4 -k- -vi -D$(SYSDEFINES);$(USERDEFINES) -c \
  -b- -w-par -w-inl -Vx -WE -tWM
CFLAG2 =
CFLAG3 =
PFLAGS = -U..\..;$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib;$(RELEASELIBPATH) \
  -I..\..;$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl \
  -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE -$Y- \
  -$L- -$D- -v -JPHNV -M -JPHNE
RFLAGS = -i..\..;$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl
AFLAGS = /i..\.. /i$(BCB)\projects /i$(BCB)\include /i$(BCB)\include\vcl /mx /w2 /zd
LFLAGS = -L..\..;$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib;$(RELEASELIBPATH) -aa -Tpe -x
IFLAGS =
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj sysinit.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cp32mt.lib ftd2xx.lib
PROJECT = SDIAPP.exe
# ---------------------------------------------------------------------------
.autodepend

!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=1
AutoIncBuild=1
MajorVer=3
MinorVer=3
Release=1
Build=8
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0
Locale=1033
CodePage=1252

[Version Info Keys]
CompanyName=FTDI
FileDescription=US232B Serializer and Tester
FileVersion=3.3.1.8
InternalName=ftdi_st.exe
LegalCopyright=Copyright (c) 2003 FTDI Ltd.
LegalTrademarks=
OriginalFilename=ftdi_st.exe
ProductName=USB232B Serializer and Tester
ProductVersion=3.3.1.0
Comments=

[HistoryLists\hlIncludePath]
Count=3
Item0=..\..;$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl
Item1=..\..\..\Projects;$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl
Item2=$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl

[HistoryLists\hlLibraryPath]
Count=4
Item0=..\..;$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib
Item1=..\..\..\Projects;$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib
Item2=.\;..\..\..\Projects;$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib
Item3=$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib

[HistoryLists\hlUnitAliases]
Count=1
Item0=WinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE

[Debugging]
DebugSourceDirs=

[Parameters]
RunParams=
HostApplication=
RemoteHost=
RemotePath=
RemoteDebug=0

[Compiler]
InMemoryExe=0
ShowInfoMsgs=0

!endif

$(PROJECT): $(OBJFILES) $(RESDEPEN)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB),, +
    $(ALLRES) 
!

.pas.hpp:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.pas.obj:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.cpp.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $* 

.c.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $**

.rc.res:
    $(BCB)\BIN\brcc32 $(RFLAGS) $<
#-----------------------------------------------------------------------------
