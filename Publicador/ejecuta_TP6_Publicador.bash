#!/bin/bash
#*****************************************************************
# File:   ejecuta_TP6_Publicador.bash
# Author: Víctor Marcuello Baquero (741278)
# Date:   7 de Enero 2021
# Coms:   Script de puesta en marcha del proceso publicador
#        
#*****************************************************************

MAKE_FILE=Makefile_TP6_Publicador
EJECUTABLE=Publicador

IPDESPLIEGUE=localhost
PUERTODESPLIEGUE=50000

echo "------------------------------------------"
echo "compliando publicador..."
make -f $MAKE_FILE
resMake=$?
if [[ "${resMake}" == 0 ]]
	then    #ha funcionado bien
		echo "${FORM}Compila correctamente"
	else
		echo "${FORM}ERROR: fallo al compilar"
		exit 0
	fi
echo "ejecutando publicador..."
./$EJECUTABLE  $IPDESPLIEGUE $PUERTODESPLIEGUE
exit 0
