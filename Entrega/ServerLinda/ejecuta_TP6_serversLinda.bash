#!/bin/bash
#*****************************************************************
# File:   ejecuta_p5_SERVER.bash
# Author: Alberto Carmona estradera
# Date:   Enero 2021
# Coms:   script de puesta en marcha del servidor
#        
#*****************************************************************

MAKE_FILE=Makefile_TP6_serverLinda
EJECUTABLE=serverLinda
PUERTO1=32060
PUERTO2=32059
PUERTO3=32058



echo "compliando servidor..."
make -f $MAKE_FILE
resMake=$?
if [[ "${resMake}" == 0 ]]
	then    #ha funcionado bien
		echo "${FORM}Compila correctamente"
	else
		echo "${FORM}ERROR: fallo al compilar"
		exit 0
	fi
echo "ejecutando servidor..."
./$EJECUTABLE $PUERTO1 &
./$EJECUTABLE $PUERTO2 &
./$EJECUTABLE $PUERTO3 &
exit 0
