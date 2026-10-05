#include "escalonador.hpp"

std::vector<Process> tabelaProcessos;
std::priority_queue<Process*, std::vector<Process*>, comparaCreditos> tabelaProntos;
std::queue<Process*> tabelaBloqueados;


Process::Process(int prioridade, BCP bcp) : prioridade(prioridade), bcp(bcp){}

// So redistribui quando TODOS (prontos + bloqueados) estiverem com zero credito.
// Ao redistribuir, restaura creditos = prioridade e reconstroi o heap.
static void redistribuirCreditos(){
    for(const auto& p : tabelaProcessos)
        if(p.bcp.creditos > 0) return;

    for(auto& p : tabelaProcessos)
        p.bcp.creditos = p.prioridade;

    // Reconstroi o heap: sem isso, a priority_queue continua ordenada
    // pelos creditos antigos (heap property e invalidada ao mudar creditos).
    std::priority_queue<Process*, std::vector<Process*>, comparaCreditos> nova;
    while(!tabelaProntos.empty()){
        nova.push(tabelaProntos.top());
        tabelaProntos.pop();
    }
    tabelaProntos.swap(nova);
}


int inicializarProcessos(int numProcessos){
	std::string nomeArquivo;
	int quantum = getQuantum();
	inicializarLog(quantum);
	tabelaProcessos.reserve(numProcessos);


	for(uint8_t i = 0; i < numProcessos; i++){
		if(i+1 < 10){
			nomeArquivo = "programas/0.txt";
			nomeArquivo.insert(11, std::to_string(i+1));
		} else{
			nomeArquivo = "programas/.txt";
			nomeArquivo.insert(10, std::to_string(i+1));
		}

			
		BCP bcp_programa_atual;
		bcp_programa_atual.quantum = quantum;
		bcp_programa_atual.creditos = getPrioridade(i+1);


		std::ifstream arquivo(nomeArquivo);
		if(!arquivo) return -1;
		

		std::string linha;

		//leitura de cada linha e adicao do codigo
		std::getline(arquivo, bcp_programa_atual.nomePrograma);
		while(std::getline(arquivo, linha)) { bcp_programa_atual.codigo.push_back(linha); }
		bcp_programa_atual.PC = 0;
		
		tabelaProcessos.emplace_back(Process(getPrioridade(i+1), bcp_programa_atual));
		logCarregando(bcp_programa_atual.nomePrograma);
		tabelaProntos.push(&tabelaProcessos.back());
	}
	return 0;
}

int escalonadorProcessos(){
	//meio que ja esta feito?
	return 0;
}

int executarProcessos(){
	int numProcessos = tabelaProcessos.size();

	//contadores globais do sistema
	int totalTrocas = 0;          //uma troca = interrupcao + escolha de outro
	int totalInstrucoes = 0;      //soma das instrucoes executadas em todos os surtos
	int totalQuanta = 0;          //numero de surtos executados

	while(numProcessos > 0){

		//nao ha prontos, avanca os bloqueados e volta a tentar
		if(tabelaProntos.empty()){
			int n = tabelaBloqueados.size();
			for(int i = 0; i < n; i++){
				Process* b = tabelaBloqueados.front();
				tabelaBloqueados.pop();

				b->bcp.tempEspera--;
				if(b->bcp.tempEspera <= 0){
					b->bcp.estado = pronto;
					tabelaProntos.push(b);
				} else {
					tabelaBloqueados.push(b);
				}
			}
			continue;
		}

		//tira o processo do topo (com maior credito) para trabalhar com ele
		Process* p = tabelaProntos.top();
		tabelaProntos.pop();

		p->bcp.estado = executando;
		p->bcp.creditos--;                       //perde 1 credito ao comecar
		int quantumRestante = p->bcp.quantum;    //copia local, NAO mexe no BCP

		//anuncia quem vai executar
		logExecutando(p->bcp.nomePrograma);

		bool bloqueou = false;
		bool terminou = false;
		int instrucoesNesteSurto = 0;            //instrucoes do surto atual

		while(quantumRestante-- > 0){

			if(p->bcp.codigo[p->bcp.PC][0] == 'X'){
				p->bcp.X = std::stoi(p->bcp.codigo[p->bcp.PC].substr(2));
				p->bcp.PC++;
			}
			else if(p->bcp.codigo[p->bcp.PC][0] == 'Y'){
				p->bcp.Y = std::stoi(p->bcp.codigo[p->bcp.PC].substr(2));
				p->bcp.PC++;
			}
			else if(p->bcp.codigo[p->bcp.PC][0] == 'C'){ // COM
				p->bcp.PC++;
			}
			else if(p->bcp.codigo[p->bcp.PC][0] == 'E'){ // E/S
				p->bcp.PC++;
				p->bcp.estado = bloqueado;
				p->bcp.tempEspera = 2;
				tabelaBloqueados.push(p);
				instrucoesNesteSurto++;          //E/S conta nas estatisticas
				logES(p->bcp.nomePrograma);
				bloqueou = true;
				break;                           //sai do while interno
			}
			else if(p->bcp.codigo[p->bcp.PC][0] == 'S'){ // SAIDA
				p->bcp.PC++;
				instrucoesNesteSurto++;          //SAIDA conta nas estatisticas
				logTerminado(p->bcp.nomePrograma, p->bcp.X, p->bcp.Y);
				terminou = true;
				break;
			}

			instrucoesNesteSurto++;
		}

		totalInstrucoes += instrucoesNesteSurto;
		totalQuanta++;

		//decide o destino do processo
		if(terminou){
			numProcessos--;                      //NAO volta para nenhuma fila
		}
		else if(bloqueou){
			//ja foi empurrado para tabelaBloqueados
			logInterrompendo(p->bcp.nomePrograma, instrucoesNesteSurto);
			totalTrocas++;
		}
		else {
			//fim do quantum, sem bloquear nem terminar
			logInterrompendo(p->bcp.nomePrograma, instrucoesNesteSurto);
			totalTrocas++;
			p->bcp.estado = pronto;
			tabelaProntos.push(p);
		}

		//decrementa o tempo de espera de TODOS os bloqueados
		int n = tabelaBloqueados.size();
		for(int i = 0; i < n; i++){
			Process* b = tabelaBloqueados.front();
			tabelaBloqueados.pop();

			b->bcp.tempEspera--;
			if(b->bcp.tempEspera <= 0){
				b->bcp.estado = pronto;
				tabelaProntos.push(b);
			}
			else {
				tabelaBloqueados.push(b);
			}
		}

		//tenta redistribuir creditos ao fim de cada rodada
		redistribuirCreditos();
	}

	//estatisticas finais do sistema
	double mediaTrocas = (double)totalTrocas / tabelaProcessos.size();
	double mediaInstrucoes = totalQuanta > 0
	                         ? (double)totalInstrucoes / totalQuanta
	                         : 0.0;
	logEstatisticas(mediaTrocas, mediaInstrucoes, tabelaProcessos.front().bcp.quantum);

	return 0;
}
