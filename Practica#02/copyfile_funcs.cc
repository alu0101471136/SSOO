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
#include "scope.hpp"

/**
 * @name: Usage
 * @brief: Controlar el uso del programa para que funcione correctamente
 * @param: numero_parametros: el numero de parametros pasados al ejecutar el programa, 
 *         primer_argumento: parametro en el que estara '--help'
 */
bool Usage(int numero_parametros, std::string& primer_argumento) {
  if (primer_argumento == "--help" ) {
    std::cout << kHelpText << std::endl;
    throw std::runtime_error(""); 
  } else if(primer_argumento == "-m" || primer_argumento == "-a") {
    if (numero_parametros != 4) {
      std::cerr << "Modo de Uso: ./copyfile (-m|-a) ruta/de/origen ruta/de/destino" << std::endl;
      std::cerr << "Pruebe ./copyfile --help para más información" << std::endl;
      return false;
    }
  } else if (numero_parametros != 3) {
    std::cerr << "Modo de Uso: ./copyfile ruta/de/origen ruta/de/destino" << std::endl;
    std::cerr << "Pruebe ./copyfile --help para más información" << std::endl;
    return false;
  }
  return true;
}

std::vector<uint8_t> ReadFile(const int fd) {
  std::vector<uint8_t> buffer(16ul * 1024 * 1024);
  ssize_t bytes_leidos{read(fd, buffer.data(), buffer.size())};
  if (bytes_leidos < 0) {
    throw std::system_error(errno, std::system_category(), "Error Reading file");
  }
  buffer.resize(bytes_leidos);
  return buffer;

}

void WriteFile(int fd, const std::vector<uint8_t>& buffer) {
  ssize_t bytes_escritos = write(fd, buffer.data(), buffer.size());
  if (bytes_escritos < 0) {
    throw std::system_error(errno, std::system_category(), "Error Writing file");
  }
}

void copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all) {
  std::string copia_src_path{src_path};
  std::string copia_dst_path{dst_path};
  struct stat src_comprobacion;
  if (stat(src_path.c_str(), &src_comprobacion) == -1 && S_ISREG(src_comprobacion.st_mode) == 0) {
    throw std::runtime_error("The source path doesn't exist or the source file isn't a regular file.");
  }
  struct stat dst_comprobacion;
  if (stat(dst_path.c_str(), &dst_comprobacion) == 0) {
    assert(src_path != dst_path);
    if (S_ISDIR(dst_comprobacion.st_mode)) {
      char* src_file = const_cast<char*>(src_path.c_str());
      copia_dst_path.append(basename(src_file));
    }
  } else {
    throw std::system_error(errno, std::system_category());
  }
  int fd_src = open(src_path.c_str(), O_RDONLY);
  scope::scope_exit src_exit([fd_src](){
    close(fd_src);
  });
  if (fd_src < 0) {
    throw std::system_error(errno, std::system_category());
  }
  struct stat dst_copia_comprobacion;
  int fd_dst;
  if (stat(copia_dst_path.c_str(), &dst_copia_comprobacion) == 0) {
    fd_dst = open(copia_dst_path.c_str(), O_WRONLY | O_TRUNC);
  } else {
    fd_dst = open(copia_dst_path.c_str(), O_WRONLY | O_CREAT, 0666);
  }
  if (fd_dst < 0) {
    throw std::system_error(errno, std::system_category());
  }
  scope::scope_exit dst_exit([fd_dst](){
    close(fd_dst);
  });
  while (true) {
    std::vector<uint8_t> buffer = ReadFile(fd_src);
    if (buffer.empty()) {
      break;
    }
    WriteFile(fd_dst, buffer);
  }
  if (preserve_all) {
    chmod(copia_dst_path.c_str(), src_comprobacion.st_mode);
    chown(copia_dst_path.c_str(), src_comprobacion.st_uid, src_comprobacion.st_gid);
    struct utimbuf buf_time;
    buf_time.actime = src_comprobacion.st_mtim.tv_sec;
    buf_time.modtime = src_comprobacion.st_mtim.tv_sec;
    utime(copia_dst_path.c_str(), &buf_time);
  } 
}

void move_file(const std::string& src_path, const std::string& dst_path) {
  int xd;
}

void print (const std::string& str) {
  int bytes_escritos = write(STDOUT_FILENO, str.c_str(), str.size());
  if (bytes_escritos < 0) {
    throw std::system_error(errno, std::system_category());
  }
}

void print_prompt(int last_command_status) {
  std::string salida_prompt;
  std::string usuario, host_name, ruta_prompt;
  char* ruta_actual;
  usuario = getlogin();
  size_t MAX_SIZE{20};
  char hostname[MAX_SIZE];
  int hostname_err = gethostname(&hostname[0], MAX_SIZE);
  if (hostname_err < 0) {
    throw std::system_error(errno, std::system_category());
  }
  ruta_actual = getcwd(NULL, 0);
  std::string final{"$>"};
  if (last_command_status < 0) {
    final = "$<";
  }
  host_name = hostname;
  ruta_prompt = ruta_actual;
  salida_prompt = salida_prompt + usuario + "@" + hostname + ":" + ruta_prompt + " " + final;
  print(salida_prompt);
}

void read_line(int fd, std::string& line) {
  static std::vector<uint8_t> pending_input;
  while (true) {
    if (std::find(pending_input.begin(), pending_input.end(), '\n') != pending_input.end()) {
      int iterador_salto_linea;
      for (unsigned i = 0; i < pending_input.size(); ++i) {
        if (pending_input[i] == '\n') {
          line += pending_input[i];
          iterador_salto_linea = i;
          break;
        }
        line += pending_input[i];
      }
      pending_input.erase(pending_input.begin(), pending_input.begin() + iterador_salto_linea);
      return;
    } 
  }
}
