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

echo "------------------------------------------"
if [[ "$#" -ne 1 ]]
then
	echo "Invocar como: $0 PUERTO"
	echo "------------------------------------------"
	exit
fi

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
./$EJECUTABLE $1 
exit 0
