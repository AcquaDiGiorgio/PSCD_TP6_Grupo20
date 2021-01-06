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

PuertoDespliegue=50000
IPserver1=localhost
IPserver2=localhost
IPserver3=localhost
Puertoserver1=50001
Puertoserver2=50002
Puertoserver3=50003


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
