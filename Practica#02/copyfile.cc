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
#include <algorithm>
#include <string>
#include "tools.h"

int main(int argc, char* argv[]) {
  int numero_argumentos{argc};
  bool comprobacion_argumentos{true};
  if (numero_argumentos > 1) {
    std::string primer_argumento{argv[1]};
    comprobacion_argumentos = Usage(numero_argumentos, primer_argumento);
  }
  if (!comprobacion_argumentos) {
    return 0;
  }
  std::string linea_entrada;
  shell::command_result resultado_execute{0, false};
  try {
    while (true) {
      if (isatty(STDIN_FILENO)) {
        print_prompt(resultado_execute.return_value);
      }
      read_line(STDIN_FILENO, linea_entrada);
      if (!linea_entrada.empty()) {
        std::vector<shell::command> comandos_entrantes = parse_line(linea_entrada);
        if (comandos_entrantes.empty()) {
          continue;
        } 
        resultado_execute = execute_commands(comandos_entrantes);
        if (resultado_execute.is_quit_requested) {
          return resultado_execute.return_value;
        }
      } else {
        break;
      }
    }
  } catch (std::exception& error) {
    std::cerr << "Error on runtime" << "\n";
    return 0;
  }
  return 0;
}