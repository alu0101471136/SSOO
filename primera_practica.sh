#!/bin/bash

# filesysteminfo - Un script que informa de archivos montados en el sistema

##### Constantes

TITLE="Información del sistema para $HOSTNAME"

##### Estilos

TEXT_BOLD=$(tput bold)
TEXT_GREEN=$(tput setaf 2)
TEXT_RESET=$(tput sgr0)

####### Funciones ##########

filesysteminfo() {
  local lista_montados=$(mount | tr -s " " ":" | cut -d":" -f 5 | sort -u) 
  local lista_ordenada=$(echo "$lista_montados" | while read tipos; do  
  if df -t "$tipos" 2> /dev/null > /dev/null; then
    df -t "$tipos" | tail -n+2 | tr -s " " | sort -k 3 | tail -n1 | cut -d " " -f 1,3,6 # Mustra el tipo que mas usados tiene
  else
    continue
  fi
  done)
  echo $TEXT_BOLD"Tipos     Usados    Montado en "$TEXT_RESET
  echo "$lista_ordenada" | sort | tr -s " " | column -t
}

####### Programa principal ##########

cat << _EOF_
  $TEXT_BOLD$TITLE$TEXT_RESET
  $echo
_EOF_

filesysteminfo