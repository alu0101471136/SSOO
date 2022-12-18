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
#include <string>
#include "copyfile_funcs.h"

int main(int argc, char* argv[]) {
  int numero_argumentos{argc};
  std::string primer_argumento{argv[1]};
  Usage(numero_argumentos, primer_argumento);
  std::string segundo_argumento{argv[2]};
  try {
    if (primer_argumento == "-m") {
      std::string tercer_argumento{argv[3]};
      move_file(segundo_argumento, tercer_argumento);
    } else if (primer_argumento == "-a") {
      std::string tercer_argumento{argv[3]};
      copy_file(segundo_argumento, tercer_argumento, true);
    } else {
      copy_file(primer_argumento, segundo_argumento);
    }
  } catch (std::exception& error) {
    std::cerr << "Error on runtime" << "\n";
    return -1;
  }
  return 0;
}