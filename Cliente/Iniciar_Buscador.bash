#!/bin/bash
#*****************************************************************
# File:   ejecuta_p5_SERVER.bash
# Author: Alberto Carmona estradera
# Date:   Enero 2021
# Coms:   script de puesta en marcha del servidor
#        
#*****************************************************************

MAKE_FILE=Makefile_TP6_Cliente
BUSCADOR=buscador

IPDESPLIEGUE=localhost
PUERTODESPLIEGUE=50000



echo "------------------------------------------"
echo "compliando cliente..."
make -f $MAKE_FILE
resMake=$?
if [[ "${resMake}" == 0 ]]
	then    #ha funcionado bien
		echo "${FORM}Compila correctamente"
	else
		echo "${FORM}ERROR: fallo al compilar"
		exit 0
	fi
echo "ejecutando cliente..."
./$BUSCADOR  $IPDESPLIEGUE $PUERTODESPLIEGUE

exit 0
