#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <utility>
#include <stdlib.h>
#include <cstdint>

enum Estado{
	executando = 0,
	pronto,
	bloqueado
};

int getQuantum();
int getPrioridade(int linha);

// ----- Funções de log -----
void inicializarLog(int quantum);
void fecharLog();
void logCarregando(const std::string& nome);
void logExecutando(const std::string& nome);
void logInterrompendo(const std::string& nome, int nInstrucoes);
void logES(const std::string& nome);
void logTerminado(const std::string& nome, int X, int Y);
void logEstatisticas(double mediaTrocas, double mediaInstrucoes, int quantum);


#endif
