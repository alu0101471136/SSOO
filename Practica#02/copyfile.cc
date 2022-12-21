/**
  * Universidad de La Laguna
  * Escuela Superior de Ingeniería y Tecnología
  * Grado en Ingeniería Informática
  * Computabilidad y Algoritmia 2022-2023
  *
  * @file copyfile.cc
  * @author Raúl Álvarez Pérez alu0101471136@ull.edu.es
  * @date Dec 12 2022
  * @brief Programa llamado copyfile, muy similar al conocido comando cp
  * @bug No existen fallos conocidos
  */

#include <iostream>
#include <unistd.h>
#include <string>
#include "tools.h"

int main(int argc, char* argv[]) {
  int numero_argumentos{argc};
  std::string primer_argumento{argv[1]};
  bool comprobacion_argumentos = Usage(numero_argumentos, primer_argumento);
  if (!comprobacion_argumentos) {
    exit(EXIT_FAILURE);
  }
  std::string linea_entrada;
  try {
    while (true) {
      int estado_comando_anterior{0};
      print_prompt(estado_comando_anterior);
      read_line(STDOUT_FILENO, linea_entrada);
    }
  } catch (std::exception& error) {
    std::cerr << "Error on runtime" << "\n";
    exit(EXIT_FAILURE);
  }
  return 0;
}