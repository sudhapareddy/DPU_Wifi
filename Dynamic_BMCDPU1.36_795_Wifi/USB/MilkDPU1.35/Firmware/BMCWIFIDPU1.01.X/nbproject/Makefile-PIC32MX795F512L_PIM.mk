#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-PIC32MX795F512L_PIM.mk)" "nbproject/Makefile-local-PIC32MX795F512L_PIM.mk"
include nbproject/Makefile-local-PIC32MX795F512L_PIM.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=PIC32MX795F512L_PIM
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED="../../../../Microchip/MDD File System/FSIO.c" ../../../../Microchip/USB/usb_host.c "../../../../Microchip/USB/MSD Host Driver/usb_host_msd.c" "../../../../Microchip/USB/MSD Host Driver/usb_host_msd_scsi.c" "../../../../Microchip/USB/Printer Host Driver/usb_host_printer.c" "../../../../Microchip/USB/Printer Host Driver/usb_host_printer_esc_pos.c" ../usb_config.c ../BMCmain.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/491339551/FSIO.o ${OBJECTDIR}/_ext/926206843/usb_host.o ${OBJECTDIR}/_ext/921800108/usb_host_msd.o ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o ${OBJECTDIR}/_ext/90607120/usb_host_printer.o ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o ${OBJECTDIR}/_ext/1472/usb_config.o ${OBJECTDIR}/_ext/1472/BMCmain.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/491339551/FSIO.o.d ${OBJECTDIR}/_ext/926206843/usb_host.o.d ${OBJECTDIR}/_ext/921800108/usb_host_msd.o.d ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o.d ${OBJECTDIR}/_ext/90607120/usb_host_printer.o.d ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o.d ${OBJECTDIR}/_ext/1472/usb_config.o.d ${OBJECTDIR}/_ext/1472/BMCmain.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/491339551/FSIO.o ${OBJECTDIR}/_ext/926206843/usb_host.o ${OBJECTDIR}/_ext/921800108/usb_host_msd.o ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o ${OBJECTDIR}/_ext/90607120/usb_host_printer.o ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o ${OBJECTDIR}/_ext/1472/usb_config.o ${OBJECTDIR}/_ext/1472/BMCmain.o

# Source Files
SOURCEFILES=../../../../Microchip/MDD File System/FSIO.c ../../../../Microchip/USB/usb_host.c ../../../../Microchip/USB/MSD Host Driver/usb_host_msd.c ../../../../Microchip/USB/MSD Host Driver/usb_host_msd_scsi.c ../../../../Microchip/USB/Printer Host Driver/usb_host_printer.c ../../../../Microchip/USB/Printer Host Driver/usb_host_printer_esc_pos.c ../usb_config.c ../BMCmain.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-PIC32MX795F512L_PIM.mk ${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/491339551/FSIO.o: ../../../../Microchip/MDD\ File\ System/FSIO.c  .generated_files/flags/PIC32MX795F512L_PIM/224ce7a7a2727d0b2e4a31442db8f3ecf8174c13 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/491339551" 
	@${RM} ${OBJECTDIR}/_ext/491339551/FSIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/491339551/FSIO.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/491339551/FSIO.o.d" -o ${OBJECTDIR}/_ext/491339551/FSIO.o "../../../../Microchip/MDD File System/FSIO.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/926206843/usb_host.o: ../../../../Microchip/USB/usb_host.c  .generated_files/flags/PIC32MX795F512L_PIM/15926d36f89b2a236c844b640747ca10ef43843 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/926206843" 
	@${RM} ${OBJECTDIR}/_ext/926206843/usb_host.o.d 
	@${RM} ${OBJECTDIR}/_ext/926206843/usb_host.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/926206843/usb_host.o.d" -o ${OBJECTDIR}/_ext/926206843/usb_host.o ../../../../Microchip/USB/usb_host.c    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/921800108/usb_host_msd.o: ../../../../Microchip/USB/MSD\ Host\ Driver/usb_host_msd.c  .generated_files/flags/PIC32MX795F512L_PIM/28c50bba4e1ed9f4fe1d063e95e3a2e7ef4ca31e .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/921800108" 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd.o.d 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/921800108/usb_host_msd.o.d" -o ${OBJECTDIR}/_ext/921800108/usb_host_msd.o "../../../../Microchip/USB/MSD Host Driver/usb_host_msd.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o: ../../../../Microchip/USB/MSD\ Host\ Driver/usb_host_msd_scsi.c  .generated_files/flags/PIC32MX795F512L_PIM/69802789927a779ea261b271cf631b3e7856eb75 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/921800108" 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o.d 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o.d" -o ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o "../../../../Microchip/USB/MSD Host Driver/usb_host_msd_scsi.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/90607120/usb_host_printer.o: ../../../../Microchip/USB/Printer\ Host\ Driver/usb_host_printer.c  .generated_files/flags/PIC32MX795F512L_PIM/df5616ae6906c66b821c053c4d3ada93e424e84e .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/90607120" 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer.o.d 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/90607120/usb_host_printer.o.d" -o ${OBJECTDIR}/_ext/90607120/usb_host_printer.o "../../../../Microchip/USB/Printer Host Driver/usb_host_printer.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o: ../../../../Microchip/USB/Printer\ Host\ Driver/usb_host_printer_esc_pos.c  .generated_files/flags/PIC32MX795F512L_PIM/191bad24775f514118965402d6f7791187c5d905 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/90607120" 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o.d 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o.d" -o ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o "../../../../Microchip/USB/Printer Host Driver/usb_host_printer_esc_pos.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1472/usb_config.o: ../usb_config.c  .generated_files/flags/PIC32MX795F512L_PIM/c8acdfefd7fa78a33d1cd97dc20c484b241208a4 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_config.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1472/usb_config.o.d" -o ${OBJECTDIR}/_ext/1472/usb_config.o ../usb_config.c    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1472/BMCmain.o: ../BMCmain.c  .generated_files/flags/PIC32MX795F512L_PIM/1f7e6f5db2274de1f321730a02748364617bb6c2 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/BMCmain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BMCmain.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1472/BMCmain.o.d" -o ${OBJECTDIR}/_ext/1472/BMCmain.o ../BMCmain.c    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
else
${OBJECTDIR}/_ext/491339551/FSIO.o: ../../../../Microchip/MDD\ File\ System/FSIO.c  .generated_files/flags/PIC32MX795F512L_PIM/2c245a5e80c807f04192a6da7a6f25ac397a69ff .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/491339551" 
	@${RM} ${OBJECTDIR}/_ext/491339551/FSIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/491339551/FSIO.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/491339551/FSIO.o.d" -o ${OBJECTDIR}/_ext/491339551/FSIO.o "../../../../Microchip/MDD File System/FSIO.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/926206843/usb_host.o: ../../../../Microchip/USB/usb_host.c  .generated_files/flags/PIC32MX795F512L_PIM/2455356e3883d642cefb6605aa25403dbbf5a4d9 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/926206843" 
	@${RM} ${OBJECTDIR}/_ext/926206843/usb_host.o.d 
	@${RM} ${OBJECTDIR}/_ext/926206843/usb_host.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/926206843/usb_host.o.d" -o ${OBJECTDIR}/_ext/926206843/usb_host.o ../../../../Microchip/USB/usb_host.c    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/921800108/usb_host_msd.o: ../../../../Microchip/USB/MSD\ Host\ Driver/usb_host_msd.c  .generated_files/flags/PIC32MX795F512L_PIM/1d21fe1d006ae9de7f371a48df34dda353b088f7 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/921800108" 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd.o.d 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/921800108/usb_host_msd.o.d" -o ${OBJECTDIR}/_ext/921800108/usb_host_msd.o "../../../../Microchip/USB/MSD Host Driver/usb_host_msd.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o: ../../../../Microchip/USB/MSD\ Host\ Driver/usb_host_msd_scsi.c  .generated_files/flags/PIC32MX795F512L_PIM/e954b42794ed9e6798a4daf8bd8e091b6abefc10 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/921800108" 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o.d 
	@${RM} ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o.d" -o ${OBJECTDIR}/_ext/921800108/usb_host_msd_scsi.o "../../../../Microchip/USB/MSD Host Driver/usb_host_msd_scsi.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/90607120/usb_host_printer.o: ../../../../Microchip/USB/Printer\ Host\ Driver/usb_host_printer.c  .generated_files/flags/PIC32MX795F512L_PIM/436090a367136eaad65e648d448fc5b5896f1cab .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/90607120" 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer.o.d 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/90607120/usb_host_printer.o.d" -o ${OBJECTDIR}/_ext/90607120/usb_host_printer.o "../../../../Microchip/USB/Printer Host Driver/usb_host_printer.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o: ../../../../Microchip/USB/Printer\ Host\ Driver/usb_host_printer_esc_pos.c  .generated_files/flags/PIC32MX795F512L_PIM/6698196fea3158d6281c41c37f86897d3446213a .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/90607120" 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o.d 
	@${RM} ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o.d" -o ${OBJECTDIR}/_ext/90607120/usb_host_printer_esc_pos.o "../../../../Microchip/USB/Printer Host Driver/usb_host_printer_esc_pos.c"    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1472/usb_config.o: ../usb_config.c  .generated_files/flags/PIC32MX795F512L_PIM/663bd4d1943648df7cb0b6e10dcbed8b45a3a781 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_config.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1472/usb_config.o.d" -o ${OBJECTDIR}/_ext/1472/usb_config.o ../usb_config.c    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1472/BMCmain.o: ../BMCmain.c  .generated_files/flags/PIC32MX795F512L_PIM/a270403fabd3c2886c463f1f30e4fe65978096f0 .generated_files/flags/PIC32MX795F512L_PIM/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/BMCmain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/BMCmain.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -I".." -I"../../../../Microchip/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1472/BMCmain.o.d" -o ${OBJECTDIR}/_ext/1472/BMCmain.o ../BMCmain.c    -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC02000:0x1FC02FEF -mreserve=boot@0x1FC02000:0x1FC024FF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=3000 
	
else
${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_PIC32MX795F512L_PIM=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=3000 
	${MP_CC_DIR}\\xc32-bin2hex ${DISTDIR}/BMCWIFIDPU1.01.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
