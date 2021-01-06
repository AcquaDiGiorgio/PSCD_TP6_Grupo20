#!/bin/bash
#*****************************************************************
# File:   ejecuta_p5_SERVER.bash
# Author: Alberto Carmona estradera
# Date:   Enero 2021
# Coms:   script de puesta en marcha del servidorde la práctica 5 de PSCD 
#        
#*****************************************************************
myPWD=$(pwd)

SERVER_DIR=ServerLinda
MONITORIZACION_DIR=ServerMonitorizacion
DESPLIEGUE_DIR=ServerDespliegue

MAKE_FILE_SERVER=Makefile_TP6_serverLinda
MAKE_FILE_MONITORIZACION=Makefile_TP6_serverMonitorizacion
MAKE_FILE_DESPLIEGUE=Makefile_TP6_serverDespliegue


echo "compliando servidor Linda..."
cd $SERVER_DIR
make -f $MAKE_FILE_SERVER
resMake=$?
if [[ "${resMake}" == 0 ]]
	then    #ha funcionado bien
		echo "${FORM}Compila correctamente"
        echo ""
	else
		echo "${FORM}ERROR: fallo al compilar el servidor Linda"
        cd $myPWD
		exit 0
fi
cd $myPWD

echo "compliando servidor de Despliegue..."
cd $DESPLIEGUE_DIR
make -f $MAKE_FILE_DESPLIEGUE
resMake=$?
if [[ "${resMake}" == 0 ]]
	then    #ha funcionado bien
		echo "${FORM}Compila correctamente"
        echo ""
	else
		echo "${FORM}ERROR: fallo al compilar el servidor de Despliegue"
        cd $myPWD
		exit 0
fi
cd $myPWD

echo "compliando servidor de Monitorizacion..."
cd $MONITORIZACION_DIR
make -f $MAKE_FILE_MONITORIZACION
resMake=$?
if [[ "${resMake}" == 0 ]]
	then    #ha funcionado bien
		echo "${FORM}Compila correctamente"
        echo ""
	else
		echo "${FORM}ERROR: fallo al compilar el servidor de Monitorizacion"
        cd $myPWD
		exit 0
fi
cd $myPWD





exit 0
