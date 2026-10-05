#!/bin/bash
# roda o escalonador com varios quanta, guarda os logs em logs/
# e extrai as metricas em logs/metricas.txt
#
# pode ser chamado de qualquer lugar:
#   ./testes/run.sh
#   cd testes && ./run.sh

set -e

# garante que trabalhamos a partir da raiz do projeto
cd "$(dirname "$0")/.."

QUANTA="1 2 4 6 8 10 12 14 16 18 20"
BIN="./bin/escalonador"
PROGRAMAS="programas"
QUANTUM_FILE="$PROGRAMAS/quantum.txt"
LOG_DIR="logs"
METRICAS="$LOG_DIR/metricas.txt"

mkdir -p "$LOG_DIR"
echo "quantum,media_trocas,media_instrucoes" > "$METRICAS"

for q in $QUANTA; do
    echo ">>> rodando com quantum = $q"
    echo "$q" > "$QUANTUM_FILE"

    if ! $BIN; then
        echo "erro rodando escalonador com quantum $q"
        continue
    fi

    if [ "$q" -lt 10 ]; then
        log="log0${q}.txt"
    else
        log="log${q}.txt"
    fi

    if [ ! -f "$log" ]; then
        echo "aviso: $log nao foi gerado"
        continue
    fi

    mv "$log" "$LOG_DIR/"

    trocas=$(grep "MEDIA DE TROCAS:" "$LOG_DIR/$log" | awk '{print $4}')
    instrucoes=$(grep "MEDIA DE INSTRUCOES:" "$LOG_DIR/$log" | awk '{print $4}')

    echo "$q,$trocas,$instrucoes" >> "$METRICAS"
done

echo "pronto. metricas em $METRICAS"
