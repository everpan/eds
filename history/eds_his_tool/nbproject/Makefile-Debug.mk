#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/29dd86f/BitCompress.o \
	${OBJECTDIR}/_ext/29dd86f/TL_Common.o \
	${OBJECTDIR}/_ext/29dd86f/TL_Datetime.o \
	${OBJECTDIR}/_ext/29dd86f/TL_Exp.o \
	${OBJECTDIR}/_ext/29dd86f/TL_Mmap.o \
	${OBJECTDIR}/_ext/29dd86f/TL_Option.o \
	${OBJECTDIR}/_ext/29dd86f/sqlite3.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-ldl -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds_his_tool

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds_his_tool: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds_his_tool ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/29dd86f/BitCompress.o: ../../BitCompress.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/29dd86f
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29dd86f/BitCompress.o ../../BitCompress.cpp

${OBJECTDIR}/_ext/29dd86f/TL_Common.o: ../../TL_Common.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/29dd86f
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29dd86f/TL_Common.o ../../TL_Common.cpp

${OBJECTDIR}/_ext/29dd86f/TL_Datetime.o: ../../TL_Datetime.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/29dd86f
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29dd86f/TL_Datetime.o ../../TL_Datetime.cpp

${OBJECTDIR}/_ext/29dd86f/TL_Exp.o: ../../TL_Exp.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/29dd86f
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29dd86f/TL_Exp.o ../../TL_Exp.cpp

${OBJECTDIR}/_ext/29dd86f/TL_Mmap.o: ../../TL_Mmap.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/29dd86f
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29dd86f/TL_Mmap.o ../../TL_Mmap.cpp

${OBJECTDIR}/_ext/29dd86f/TL_Option.o: ../../TL_Option.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/29dd86f
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29dd86f/TL_Option.o ../../TL_Option.cpp

${OBJECTDIR}/_ext/29dd86f/sqlite3.o: ../../sqlite3.c
	${MKDIR} -p ${OBJECTDIR}/_ext/29dd86f
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29dd86f/sqlite3.o ../../sqlite3.c

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
