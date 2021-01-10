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
IPserver1=localhost
IPserver2=localhost
IPserver3=localhost
Puertoserver1=32060
Puertoserver2=32059
Puertoserver3=32058


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
