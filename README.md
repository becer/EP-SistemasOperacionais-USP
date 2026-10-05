# Escalonador de Processos — EP1 de Sistemas Operacionais

![USP](https://img.shields.io/badge/USP-Sistemas%20Operacionais-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus)
![License](https://img.shields.io/badge/license-MIT-green)

Implementação de um escalonador de tarefas para *time sharing* em uma máquina
fictícia de processador único, desenvolvida como primeiro exercício-programa da
disciplina **Sistemas Operacionais** da Universidade de São Paulo (USP).

O trabalho reproduz, em software, o comportamento de um escalonador de
processos com política de prioridades baseada em créditos — inspirada no
escalonador do Linux — e uma fila de bloqueados gerida por ordem de chegada
para simular espera por dispositivos de E/S.

## Visão geral

O escalonador gerencia dez programas concorrentes, cada um carregado em um
Bloco de Controle de Processo (BCP) próprio. A máquina fictícia possui apenas
quatro tipos de instruções:

| Instrução | Descrição |
|-----------|-----------|
| `X=<valor>` / `Y=<valor>` | Atribuição a registrador de uso geral |
| `COM` | Comando genérico (consumo de CPU) |
| `E/S` | Chamada de entrada/saída — bloqueia o processo |
| `SAIDA` | Término do programa |

Os programas são escalonados em surtos de CPU (quanta) de tamanho fixo, lido
de `programas/quantum.txt`. As prioridades iniciais são lidas de
`programas/prioridades.txt` e transformadas em créditos, que são consumidos a
cada surto de execução.

### Política de escalonamento

1. Cada processo recebe inicialmente `creditos = prioridade`.
2. A fila de prontos é mantida como uma fila de prioridade ordenada pelos
   créditos restantes (maior crédito primeiro).
3. Ao iniciar um surto, o processo perde **um crédito**.
4. Um processo executando pode:
   - consumir todo o quantum e voltar para a fila de prontos;
   - executar `E/S`, sendo movido para a fila de bloqueados com tempo de
     espera de **dois quanta**;
   - executar `SAIDA`, sendo removido do sistema.
5. O tempo de espera dos processos bloqueados é decrementado a cada surto de
   CPU concluído, e o processo retorna à fila de prontos quando ele chega a
   zero.
6. Quando **todos** os processos estiverem com zero crédito, os créditos são
   redistribuídos conforme as prioridades originais.

## Estrutura do repositório

```
.
├── Makefile
├── main.cpp
├── include/
│   ├── escalonador.hpp     # BCP, Process, assinaturas do escalonador
│   └── utils.hpp           # enums, comparadores e API de log
├── src/
│   ├── escalonador.cpp     # carregamento e loop principal
│   └── utils.cpp           # leitura de arquivos e escrita de log
├── programas/              # entradas do sistema
│   ├── 01.txt … 10.txt
│   ├── prioridades.txt
│   └── quantum.txt
├── testes/
│   ├── run.sh              # executa para varios quanta e extrai metricas
│   └── plot.py             # gera graficos a partir de logs/metricas.txt
└── README.md
```

## Compilação

Requisitos:

- `g++` com suporte a C++17
- `make`
- Opcional: `python3` + `matplotlib` (para gerar gráficos)

```bash
make
```

Isso produz o binário `bin/escalonador`.

## Uso

O binário deve ser executado **a partir da raiz do projeto**, pois os
caminhos de entrada (`programas/…`) e o arquivo de log (`logXX.txt`) são
relativos ao diretório de execução.

```bash
./bin/escalonador
```

Para alterar o quantum, edite `programas/quantum.txt`:

```bash
echo 3 > programas/quantum.txt
./bin/escalonador
```

O arquivo `logXX.txt` (onde `XX` é o valor do quantum, com dois dígitos) é
gerado no diretório corrente e contém:

- os nomes dos processos carregados;
- os eventos de execução, interrupção, E/S e término;
- o valor final de `X` e `Y` de cada processo;
- estatísticas do sistema: média de trocas por processo, média de instruções
  por quantum e o quantum utilizado.

## Geração de métricas e gráficos

Os scripts de automação ficam em `testes/` e devem ser executados **a partir
da raiz do projeto**, para que os caminhos relativos (`programas/`, `bin/`,
`logs/`) funcionem corretamente.

Para rodar o escalonador com uma faixa uniforme de quanta, mover os logs para
`logs/` e extrair as métricas em `logs/metricas.txt`:

```bash
./testes/run.sh
```

Para gerar os gráficos a partir das métricas:

```bash
python3 testes/plot.py
```

Os gráficos são salvos em `logs/` como PNG e podem ser usados diretamente no
relatório do trabalho.

## Estruturas de dados

| Estrutura | Tipo | Descrição |
|-----------|------|-----------|
| Tabela de processos | `std::vector<Process>` | Todos os processos vivos, dona dos BCPs |
| Fila de prontos | `std::priority_queue<Process*>` | Ordenada por créditos restantes |
| Fila de bloqueados | `std::queue<Process*>` | FIFO, ordenada por ordem de chegada |
| BCP | `struct BCP` | PC, estado, prioridade, créditos, X, Y, quantum, tempo de espera, código |

## Estatísticas calculadas

- **Média de trocas por processo**: número total de interrupções
  (fim de quantum, entrada em E/S e término) dividido pelo número de processos.
- **Média de instruções por quantum**: número total de instruções executadas
  em surtos de CPU dividido pelo número total de surtos.

## Referências

- Material da disciplina de Sistemas Operacionais — USP
- Enunciado do Primeiro Exercício-Programa
- Estrutura de escalonamento por créditos inspirada no escalonador do Linux

## Licença

Distribuído sob a licença MIT. Veja `LICENSE` para mais detalhes.
