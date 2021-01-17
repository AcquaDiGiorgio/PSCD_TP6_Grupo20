#!/bin/bash
#*****************************************************************
# File:   ejecuta_p5_SERVER.bash
# Author: Alberto Carmona estradera
# Date:   Enero 2021
# Coms:   script de puesta en marcha del servidor
#        
#*****************************************************************

MAKE_FILE=Makefile_TP6_serverDespliegue
EJECUTABLE=serverDespliegue

PuertoDespliegue=2200
IPserver1=155.210.152.12
IPserver2=155.210.152.12
IPserver3=155.210.152.12
Puertoserver1=32100
Puertoserver2=32099
Puertoserver3=32098


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
./$EJECUTABLE $1 $PuertoDespliegue $IPserver1 $Puertoserver1 $IPserver2 $Puertoserver2 $IPserver3 $Puertoserver3
exit 0
