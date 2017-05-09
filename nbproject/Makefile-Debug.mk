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
	${OBJECTDIR}/API/C/edsapi/src/ArchClient.o \
	${OBJECTDIR}/API/C/edsapi/src/Backend.o \
	${OBJECTDIR}/API/C/edsapi/src/Errors.o \
	${OBJECTDIR}/API/C/edsapi/src/LiveClient.o \
	${OBJECTDIR}/BitCompress.o \
	${OBJECTDIR}/EdsLiveData.o \
	${OBJECTDIR}/EdsPointThread.o \
	${OBJECTDIR}/HttpServer.o \
	${OBJECTDIR}/KafkaServer.o \
	${OBJECTDIR}/PointData.o \
	${OBJECTDIR}/TL_Common.o \
	${OBJECTDIR}/TL_Datetime.o \
	${OBJECTDIR}/TL_Exp.o \
	${OBJECTDIR}/TL_IniFile.o \
	${OBJECTDIR}/TL_Logger.o \
	${OBJECTDIR}/TL_MemString.o \
	${OBJECTDIR}/TL_Mmap.o \
	${OBJECTDIR}/TL_Option.o \
	${OBJECTDIR}/TL_Thread.o \
	${OBJECTDIR}/TL_ThreadLock.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/sqlite3.o


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
LDLIBSOPTIONS=-LAPI/C/lib -Llib/rdkafka -larcheds2 -lliveeds2 -lloggereds2 -lobjeds2 -lsqlite_drveds2 -lPocoUtil -lPocoJSON -lPocoNet -lPocoFoundation lib/rdkafka/librdkafka++.a lib/rdkafka/librdkafka.a -lpthread -ldl -lcrypto

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds: lib/rdkafka/librdkafka++.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds: lib/rdkafka/librdkafka.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/eds ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/API/C/edsapi/src/ArchClient.o: API/C/edsapi/src/ArchClient.cpp
	${MKDIR} -p ${OBJECTDIR}/API/C/edsapi/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/API/C/edsapi/src/ArchClient.o API/C/edsapi/src/ArchClient.cpp

${OBJECTDIR}/API/C/edsapi/src/Backend.o: API/C/edsapi/src/Backend.cpp
	${MKDIR} -p ${OBJECTDIR}/API/C/edsapi/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/API/C/edsapi/src/Backend.o API/C/edsapi/src/Backend.cpp

${OBJECTDIR}/API/C/edsapi/src/Errors.o: API/C/edsapi/src/Errors.cpp
	${MKDIR} -p ${OBJECTDIR}/API/C/edsapi/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/API/C/edsapi/src/Errors.o API/C/edsapi/src/Errors.cpp

${OBJECTDIR}/API/C/edsapi/src/LiveClient.o: API/C/edsapi/src/LiveClient.cpp
	${MKDIR} -p ${OBJECTDIR}/API/C/edsapi/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/API/C/edsapi/src/LiveClient.o API/C/edsapi/src/LiveClient.cpp

${OBJECTDIR}/BitCompress.o: BitCompress.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/BitCompress.o BitCompress.cpp

${OBJECTDIR}/EdsLiveData.o: EdsLiveData.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EdsLiveData.o EdsLiveData.cpp

${OBJECTDIR}/EdsPointThread.o: EdsPointThread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EdsPointThread.o EdsPointThread.cpp

${OBJECTDIR}/HttpServer.o: HttpServer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HttpServer.o HttpServer.cpp

${OBJECTDIR}/KafkaServer.o: KafkaServer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/KafkaServer.o KafkaServer.cpp

${OBJECTDIR}/PointData.o: PointData.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PointData.o PointData.cpp

${OBJECTDIR}/TL_Common.o: TL_Common.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_Common.o TL_Common.cpp

${OBJECTDIR}/TL_Datetime.o: TL_Datetime.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_Datetime.o TL_Datetime.cpp

${OBJECTDIR}/TL_Exp.o: TL_Exp.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_Exp.o TL_Exp.cpp

${OBJECTDIR}/TL_IniFile.o: TL_IniFile.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_IniFile.o TL_IniFile.cpp

${OBJECTDIR}/TL_Logger.o: TL_Logger.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_Logger.o TL_Logger.cpp

${OBJECTDIR}/TL_MemString.o: TL_MemString.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_MemString.o TL_MemString.cpp

${OBJECTDIR}/TL_Mmap.o: TL_Mmap.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_Mmap.o TL_Mmap.cpp

${OBJECTDIR}/TL_Option.o: TL_Option.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_Option.o TL_Option.cpp

${OBJECTDIR}/TL_Thread.o: TL_Thread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_Thread.o TL_Thread.cpp

${OBJECTDIR}/TL_ThreadLock.o: TL_ThreadLock.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TL_ThreadLock.o TL_ThreadLock.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -IAPI/C/edsapi/include/ -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/sqlite3.o: sqlite3.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sqlite3.o sqlite3.c

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
