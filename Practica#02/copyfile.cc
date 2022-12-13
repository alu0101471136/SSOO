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
#include "tools.h"
/*
 * @name: Usage
 * @brief: Controlar el uso del programa para que funcione correctamente
 * @param: numero_parametros: el numero de parametros pasados al ejecutar el programa, 
 *         primer_argumento: parametro en el que estara '--help'
 */
void Usage(int numero_parametros, std::string& primer_argumento) {
  if (primer_argumento == "--help" ) {
    std::cout << kHelpText << std::endl;
    exit(EXIT_SUCCESS); 
  }
  if (numero_parametros != 3) {
    std::cout << "Modo de Uso: ./p08_grammar2CNF input.gra output.gra" << std::endl;
    std::cout << "Pruebe ./p08_grammar2CNF --help para más información" << std::endl;
    exit(EXIT_SUCCESS);
  }
}

void copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all=false);


void move_file(const std::string& src_path, const std::string& dst_path);