#include <iostream>
#include "utils.hpp"
#include "escalonador.hpp"

int main(){
    if(inicializarProcessos() != 0){
        std::cerr << "Erro ao inicializar processos.\n";
        return 1;
    }
    executarProcessos();
    fecharLog();
    return 0;
}
