#include "utils.hpp"


static std::ofstream logFile;


int getQuantum(){
	std::ifstream arquivo("programas/quantum.txt");
	if(!arquivo) return -1;

	std::string line;
	std::getline(arquivo, line);


	int quantum = std::atoi(line.c_str());
	return quantum;
}


int getPrioridade(int linha){
	std::ifstream arquivo("programas/prioridades.txt");
	if(!arquivo) return -1;

	std::string line;

	int count = 1;
	while(std::getline(arquivo, line)){
		if(count == linha) return std::atoi(line.c_str());
		count++;
	}
	return -1;
}

//funcoes de log


void inicializarLog(int quantum){
    std::string nome = "log";
    if(quantum < 10) nome += "0";
    nome += std::to_string(quantum) + ".txt";
    logFile.open(nome);
}

void fecharLog(){
    if(logFile.is_open()) logFile.close();
}

void logCarregando(const std::string& nome){
    logFile << "Carregando " << nome << "\n";
}

void logExecutando(const std::string& nome){
    logFile << "Executando " << nome << "\n";
}

void logInterrompendo(const std::string& nome, int nInstrucoes){
    logFile << "Interrompendo " << nome << " apos "
            << nInstrucoes << " instrucoes\n";
}

void logES(const std::string& nome){
    logFile << "E/S iniciada em " << nome << "\n";
}

void logTerminado(const std::string& nome, int X, int Y){
    logFile << nome << " terminado. X=" << X << ". Y=" << Y << "\n";
}

void logEstatisticas(double mediaTrocas, double mediaInstrucoes, int quantum){
    logFile << "MEDIA DE TROCAS: " << mediaTrocas << "\n";
    logFile << "MEDIA DE INSTRUCOES: " << mediaInstrucoes << "\n";
    logFile << "QUANTUM: " << quantum << "\n";
}

