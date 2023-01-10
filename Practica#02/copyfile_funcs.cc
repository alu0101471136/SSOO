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
#include <algorithm>
#include <sstream>
#include <sys/wait.h>
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
    return false;
  }
  if (numero_parametros != 1) {
    std::cerr << "Modo de Uso: ./copyfile" << std::endl;
    std::cerr << "Pruebe ./copyfile --help para más información" << std::endl;
    return false;
  }
  return true;
}
/**
 * @name: ReadFile
 * @brief: Funcion que aisla el uso de la funcion del sistema read()
 * @param: fd: File Descriptor del archivo a leer
 */
std::vector<uint8_t> ReadFile(const int fd) {
  std::vector<uint8_t> buffer(16ul * 1024 * 1024);
  ssize_t bytes_leidos{read(fd, buffer.data(), buffer.size())};
  if (bytes_leidos < 0) {
    throw std::system_error(errno, std::system_category(), "Error Reading file");
  }
  buffer.resize(bytes_leidos);
  return buffer;

}
/**
 * @name: WriteFile
 * @brief: Funcion que aisla el uso de la funcion del sistema write()
 * @param: fd: File Descriptor del archivo en el que se va a escribir, 
 *         buffer: vector en el que se encuentran los datos a escribir
 */
void WriteFile(int fd, const std::vector<uint8_t>& buffer) {
  ssize_t bytes_escritos = write(fd, buffer.data(), buffer.size());
  if (bytes_escritos < 0) {
    throw std::system_error(errno, std::system_category(), "Error Writing file");
  }
}
/**
 * @name: copy_file
 * @brief: Copia un archivo en un destino del dispositivo
 * @param: src_path: ruta del archivo que queremos copiar, 
 *         dst_path: ruta destino del archivo,
 *         preserve_all: controla si se copian los atributos del archivo  
 */
void copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all) {
  std::string copia_src_path{src_path};
  std::string copia_dst_path{dst_path};
  struct stat src_comprobacion;
  if (stat(src_path.c_str(), &src_comprobacion) < 0) {
    throw std::runtime_error("The source path doesn't exist .");
  }
  if (S_ISREG(src_comprobacion.st_mode) < 0) {
    throw std::runtime_error("The source file isn't a regular file.");
  }
  struct stat dst_comprobacion;
  if (stat(dst_path.c_str(), &dst_comprobacion) == 0) {
    if (src_comprobacion.st_ino != dst_comprobacion.st_ino) {
      if (S_ISDIR(dst_comprobacion.st_mode)) {
      char* src_file = const_cast<char*>(src_path.c_str());
      /// copia_dst_path.append("/"); NO SE AÑADE LA '/' PORQUE SE SUPONE QUE ESCRIBIRA LA RUTA COMPLETA
      copia_dst_path.append(basename(src_file));
      }
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
/**
 * @name: move_file
 * @brief: Mueve un archivo a un destino del dispositivo
 * @param: src_path: ruta del archivo que queremos mover, 
 *         dst_path: ruta destino del archivo,
 */
void move_file(const std::string& src_path, const std::string& dst_path) {
  std::string copia_dst_path{dst_path};
  struct stat src_comprobacion;
  if (stat(src_path.c_str(), &src_comprobacion)) {
    throw std::runtime_error("The source path doesn't exist.");
  }
  struct stat dst_comprobacion;
  if (stat(dst_path.c_str(), &dst_comprobacion) == 0) {
    if (S_ISDIR(dst_comprobacion.st_mode)) {
      char* src_file = const_cast<char*>(src_path.c_str());
      /// copia_dst_path.append("/"); MISMA RAZON DE ANTES
      copia_dst_path.append(basename(src_file));
    }
  }
  if (src_comprobacion.st_ino == dst_comprobacion.st_ino) {
    if (src_comprobacion.st_dev == dst_comprobacion.st_dev) {
      int rename_check = rename(src_path.c_str(), copia_dst_path.c_str());
      if (rename_check < 0) {
        throw std::runtime_error("Error on rename function.");
      }
    }
  } else {
    copy_file(src_path, dst_path, true);
    int unlink_check = unlink(src_path.c_str());
    if (unlink_check < 0) {
      throw std::runtime_error("Error on unlink function.");
    }
  }
}
/**
 * @name: print
 * @brief: Muestra en pantalla el texto que queramos
 * @param: str: Cadena de texto que queremos imprimir
 *         
 */
void print (const std::string& str) {
  int bytes_escritos = write(STDOUT_FILENO, str.c_str(), str.size());
  if (bytes_escritos < 0) {
    throw std::system_error(errno, std::system_category());
  }
}
/**
 * @name: print_primpt
 * @brief: Muestra en pantalla la primpt de la shell
 * @param: last_command_status: Entero que determina el 
 *                              estado del comando anterior
 *         
 */
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
  std::string final{"$> "};
  if (last_command_status < 0) {
    final = "$< ";
  }
  host_name = hostname;
  ruta_prompt = ruta_actual;
  salida_prompt = salida_prompt + usuario + "@" + hostname + ":" + ruta_prompt + " " + final;
  print(salida_prompt);
}
/**
 * @name: read_line
 * @brief: Lee del tecaldo estandar o de un fichero
 * @param: fd: File Descriptor del archivo que queremos leer (siempre sera STDIN_FILENO), 
 *         line: linea que modificaremos con el texto leido.
 *         
 */
void read_line(int fd, std::string& line) {
  static std::vector<uint8_t> pending_input;
  line.clear();
  while (true) {
    std::vector<uint8_t>::iterator primer_salto_linea{std::find(pending_input.begin(), pending_input.end(), '\n')};
    if (primer_salto_linea != pending_input.end()) {
      for (auto& i : pending_input)
        line += i;
      pending_input.erase(pending_input.begin(), primer_salto_linea + 1);
      return;
    } 
    std::vector<uint8_t> buffer = ReadFile(fd);
    if (buffer.empty()) {
      if (!pending_input.empty()) {
        for (auto& i : pending_input)
          line += i;
        line += '\n';
        pending_input.clear();
      }
      return;
    } else {
      pending_input.insert(pending_input.end(), buffer.begin(), buffer.end());
    }
  }
}
/**
 * @name: parse_line
 * @brief: Separa la linea en diferentes comandos
 * @param: line: Linea con los comandos que partiremos
 *         
 */
std::vector<shell::command> parse_line(const std::string& line) {
  std::istringstream iss(line);
  std::vector<shell::command> vector_comandos;
  shell::command vector_auxiliar;
  while (!iss.eof()) {
    std::string word;
    iss >> word;
    if (word[0] == '#' || word == "#" || word == "") { /// Si la palabra es o empieza por #, significa que no ha de
      break;                                           /// tenerse en cuenta por lo que se retorna lo que tenga
    }
    if (word[word.size() - 1] == ';' || word[word.size() - 1] == '|') {
      std::string cadena_aux;
      cadena_aux += word[word.size() - 1]; 
      word.erase(word.end() - 1);
      if (word == "") { /// si word esta vacio significa que se ha encontrado | o & unicamente
        vector_comandos.push_back(vector_auxiliar);
        vector_auxiliar.clear();
        continue;
      }
      vector_auxiliar.push_back(word);
      vector_comandos.push_back(vector_auxiliar);
      vector_auxiliar.clear();
      continue;
    }
    vector_auxiliar.push_back(word);
  }
  if (!vector_auxiliar.empty()) {
    vector_comandos.push_back(vector_auxiliar);
  }
  return vector_comandos;
}
/**
 * @name: echo_command
 * @brief: Funcion del comando echo, mustra por pantalla sus argumentos
 * @param: args: Todo lo que este dentro de este vector se considera texto
 *         
 */
int echo_command(const std::vector<std::string>& args) {
  std::string cadena_salida;
  for (unsigned i = 1; i < args.size(); ++i) {
    cadena_salida += args[i];
    cadena_salida += " "; 
  }
  cadena_salida += "\n";
  print(cadena_salida); /// print() se encarga de controlar sus errores
  return 0;
}
/**
 * @name: cd_command
 * @brief: Funcion del comando cd, cambia de directorio
 * @param: args: Solo debe contener el directorio destino
 *         
 */
int cd_command(const std::vector<std::string>& args) {
  if (args.size() != 2) {
    return -4;
  } 
  int chdir_value = chdir(args[1].c_str());
  if (chdir_value < 0) {
    std::cerr << "Error al intentar cambiar de directorio" << "\n";
    return chdir_value;
  }
  return 0;
}
/**
 * @name: cp_command
 * @brief: Funcion del comando cp, copia un archivo
 * @param: args: Debe tener el archivo a copiar y la ruta destino, 
 *               tambien podra tener la opcion -a
 *         
 */
int cp_command(const std::vector<std::string>& args) {
  if (args.size() == 3) {
    copy_file(args[1], args[2]);
  } else if (args.size() == 4) {
    if (args[1] == "-a") {
      copy_file(args[2], args[3], true);
    }
  } else {
    std::cerr << "Error de argumentos" << "\n";
    return -1;
  }
  return 0;
}
/**
 * @name: mv_command
 * @brief: Funcion del comando mv, mueve un archivo a otro directorio
 * @param: args: Debe tener el archivo a mover y la ruta destino
 *         
 */
int mv_command(const std::vector<std::string>& args) {
  if (args.size() != 3) {
    std::cerr << "Error de parametros" << "\n";
    return -1;
  } 
  move_file(args[1], args[2]);
  return 0;
}
/**
 * @name: execute
 * @brief: Funcion que aisla el comando exec() para un mejor uso
 * @param: args: Contiene el comando mas sus argumentos
 *         
 */
int execute(const std::vector<std::string>& args) {
  std::vector<const char*> argv;
  for (auto& arg : args)
    argv.push_back(arg.c_str());
  argv.push_back(nullptr);
  int comprobacion_comando = execvp(argv[0], const_cast<char* const*>(argv.data()));
  if (comprobacion_comando < 0) {
    std::cerr << "Fallo en la ejecución del comando " << argv[0] << "\n";
    return comprobacion_comando;
  }
  return 0;
}
/**
 * @name: execute_program
 * @brief: Funcion que ejecuta comandos externos no implementados 
 * @param: args: Contiene el comando mas sus argumentos,
 *         has_wait: Indica si debe esperar al proceso hijo.
 *         
 */
int execute_program(const std::vector<std::string>& args, bool has_wait=true) {
  pid_t child = fork(); 
  if (child == 0) {
    // Aquí solo entra el proceso hijo 
    int comprobacion_comando = execute(args);  
    if (comprobacion_comando < 0) {
      return comprobacion_comando;
    }
    return 0;
  } else if (child > 0) {
    // Aquí solo entra el proceso padre
    if (has_wait) {
      int status;
      wait(&status); 
      return 0;
    } else {
      return child;
    }
  } else {
    // Aquí solo entra el padre si no pudo crear el hijo
    std::cerr << "Error al crear al hijo" << "\n";
    return -1;
  }
}
/**
 * @name: execute_commands
 * @brief: Funcion que ejecuta comandos internos y externos 
 * @param: commands: Contiene los comandos mas sus argumentos
 *         
 */
shell::command_result execute_commands(const std::vector<shell::command>& commands) {
  int return_value{0};
  bool is_quit_requested{false};
  shell::command_result resultado_ejecucion{return_value, is_quit_requested};
  std::vector<std::pair<int, std::string>> vector_segundo_plano;
  for (unsigned i = 0; i < commands.size(); ++i) {
    if (commands[i][0] == "echo") {           /// COMANDOS INTERNOS 
      resultado_ejecucion.return_value = echo_command(commands[i]);
      if (resultado_ejecucion.return_value != 0) {
        std::cerr << "Error on echo command" << "\n";
        break; /// No se lanza un error porque no es un motivo para detener el funcionamiento normal del programa
      }
    } else if (commands[i][0] == "cd") {
      resultado_ejecucion.return_value = cd_command(commands[i]);
      if (resultado_ejecucion.return_value == -4) {
        std::cerr << "Fallo en los argumentos" << "\n";
      } else if (resultado_ejecucion.return_value < 0) {
        break;
      }
    } else if(commands[i][0] == "cp") {
      resultado_ejecucion.return_value = cp_command(commands[i]);
      if (resultado_ejecucion.return_value < 0) {
        break;
      }
    } else if(commands[i][0] == "mv") {
      resultado_ejecucion.return_value = mv_command(commands[i]);
      if (resultado_ejecucion.return_value < 0) {
        break;
      }
    } else if (commands[i][0] == "exit") {
      return shell::command_result::quit(resultado_ejecucion.return_value);
    } else {                  /// COMANDOS EXTERNOS 
      bool comprobacion_plano{true};
      for (auto& j : commands[i]) {
        if (j[j.size() - 1] == '&') {
          comprobacion_plano = false;
        }
      }
      resultado_ejecucion.return_value = execute_program(commands[i], comprobacion_plano);
      if (resultado_ejecucion.return_value < 0) {
        break;
      } else if (resultado_ejecucion.return_value > 0) { /// Se esta ejecutando en segundo plano
        vector_segundo_plano.push_back({resultado_ejecucion.return_value, commands[i][0]});
      }
    }
  }
  if (!vector_segundo_plano.empty()) { /// Comprueba si falta algun proceso en segundo plano que haya terminado
    for (unsigned j = 0; j < vector_segundo_plano.size(); ++j) {
      int status;
      waitpid(vector_segundo_plano[j].first, &status, WNOHANG);
      std::cout << "[" << vector_segundo_plano[j].first << "] " << vector_segundo_plano[j].second 
                << " ha salido (" << status << ")" << "\n";
    }
  }
  return resultado_ejecucion;
}