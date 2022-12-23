#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <string>
#include <vector>

namespace shell {
  using command = std::vector<std::string>;
  struct command_result {
    int return_value; 
    bool is_quit_requested;
    command_result(int return_value, bool request_quit=false) : 
    return_value{return_value}, is_quit_requested{request_quit} {}
    static command_result quit(int return_value=0) {
    return command_result{return_value, true};
    }
  };
}

const std::string kHelpText{"Si no contiene opciones el programa se ejecuta asi: ./copyfile ruta/de/origen ruta/de/destino.\nSi contiene opciones se podran usar -m y .a"};
bool Usage(int numero_parametros, std::string& primer_argumento);
void copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all=false);
void move_file(const std::string& src_path, const std::string& dst_path);
void print_prompt(int last_command_status);
void read_line(int fd, std::string& line);
std::vector<shell::command> parse_line(const std::string& line);
shell::command_result execute_commands(const std::vector<shell::command>& commands);

#endif