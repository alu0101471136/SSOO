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
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <libgen.h>
#include <array>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <utime.h>
#include "tools.h"
/**
 * @name: Usage
 * @brief: Controlar el uso del programa para que funcione correctamente
 * @param: numero_parametros: el numero de parametros pasados al ejecutar el programa, 
 *         primer_argumento: parametro en el que estara '--help'
 */
void Usage(int numero_parametros, std::string& primer_argumento) {
  if (primer_argumento == "--help" ) {
    std::cout << kHelpText << std::endl;
    exit(EXIT_SUCCESS); 
  } else if(primer_argumento == "-m" || primer_argumento == "-a") {
    if (numero_parametros != 4) {
    std::cout << "Modo de Uso: ./copyfile (-m|-a) ruta/de/origen ruta/de/destino" << std::endl;
    std::cout << "Pruebe ./copyfile --help para más información" << std::endl;
    exit(EXIT_SUCCESS);
  }
  }
  if (numero_parametros != 3) {
    std::cout << "Modo de Uso: ./copyfile ruta/de/origen ruta/de/destino" << std::endl;
    std::cout << "Pruebe ./copyfile --help para más información" << std::endl;
    exit(EXIT_SUCCESS);
  }
}

std::error_code copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all=false) {
  try {
    std::string copia_src_path{src_path};
    std::string copia_dst_path{dst_path};
    struct stat src_comprobacion;
    if (stat(src_path.c_str(), &src_comprobacion) == -1 && S_ISREG(src_comprobacion.st_mode) == 0) {
      return std::error_code(errno, std::system_category());
    }
    struct stat dst_comprobacion;
    if (stat(dst_path.c_str(), &dst_comprobacion)) {
      assert(src_path != dst_path);
      if (S_ISDIR(dst_comprobacion.st_mode)) {
        char* src_file = const_cast<char*>(src_path.c_str());
        copia_dst_path.append(basename(src_file));
      }
    }
    int src_fd = open(src_path.c_str(), O_RDONLY);
    int dst_fd = open(copia_dst_path.c_str(), O_CREAT | O_WRONLY, 0666);
    struct stat dst_copia_comprobacion;
    if (stat(copia_dst_path.c_str(), &dst_copia_comprobacion)) {
      open(copia_dst_path.c_str(), O_TRUNC);
    } else {
      open(copia_dst_path.c_str(), O_CREAT, 0666);
    }
    ssize_t datos;
    while (datos != 0) {
      
    }
    if (preserve_all) {
      chmod(copia_dst_path.c_str(), src_comprobacion.st_mode);
      chown(copia_dst_path.c_str(), src_comprobacion.st_uid, src_comprobacion.st_gid);
      struct utimbuf buf_time;
      buf_time.actime = src_comprobacion.st_mtim.tv_sec;
      buf_time.modtime = src_comprobacion.st_mtim.tv_sec;
      utime(copia_dst_path.c_str(), &buf_time);
    } 
  } catch (const std::exception& error) {
    std::cerr << "Error copying file" << std::endl;
    throw error;
  }
}

std::error_code move_file(const std::string& src_path, const std::string& dst_path) {
  return std::error_code(errno, std::system_category());
}
