#ifndef ESCALONADOR_HPP
#define ESCALONADOR_HPP

#include <string>
#include "utils.hpp"
#include <vector>
#include <queue>


//registrador BCP
typedef struct BCP{
	Estado estado;
	int X = 0;
	int Y = 0;
	int quantum;
	int PC = 0;
	int creditos;
	int tempEspera = 0;
	std::string nomePrograma;
	std::vector<std::string> codigo;
}BCP;

class Process{
public:
	int prioridade;
	int id = 0;
	long ultimaExecucao = 0;
	BCP bcp;
	Process(int prioridade, BCP bcp);

};


int inicializarProcessos(int numProcessos = 10);
int escalonadorProcessos();
int executarProcessos();

struct comparaCreditos{
	bool operator()(const Process* a, const Process* b) const{
		if(a->bcp.creditos != b->bcp.creditos) return a->bcp.creditos < b->bcp.creditos;
		if(a->ultimaExecucao != b->ultimaExecucao) return a->ultimaExecucao < b->ultimaExecucao;
		return a->id > b->id;
	}
};


#endif
