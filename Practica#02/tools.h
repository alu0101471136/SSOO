#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <string>

const std::string kHelpText{"Si no contiene opciones el programa se ejecuta asi: ./copyfile ruta/de/origen ruta/de/destino.\nSi contiene opciones se podran usar -m y .a"};
void Usage(int numero_parametros, std::string& primer_argumento);
void copy_file(const std::string& src_path, const std::string& dst_path, bool preserve_all=false);
void move_file(const std::string& src_path, const std::string& dst_path);

#endif