import csv
import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# garante que trabalhamos a partir da raiz do projeto
ROOT = Path(__file__).resolve().parent.parent
METRICAS = ROOT / "logs" / "metricas.txt"
LOG_DIR = ROOT / "logs"

if not METRICAS.exists():
    print(f"erro: {METRICAS} nao encontrado. rode ./testes/run.sh primeiro.")
    sys.exit(1)

QUANTA, TROCAS, INSTRUCOES = [], [], []

with METRICAS.open() as f:
    reader = csv.DictReader(f)
    for row in reader:
        QUANTA.append(int(row["quantum"]))
        TROCAS.append(float(row["media_trocas"]))
        INSTRUCOES.append(float(row["media_instrucoes"]))

# --- grafico 1: dois eixos no mesmo plot ---
fig, ax1 = plt.subplots(figsize=(9, 5))

cor1 = "tab:red"
ax1.set_xlabel("Quantum")
ax1.set_ylabel("Media de trocas", color=cor1)
ax1.plot(QUANTA, TROCAS, "o-", color=cor1, label="Trocas")
ax1.tick_params(axis="y", labelcolor=cor1)
ax1.grid(True, alpha=0.3)

ax2 = ax1.twinx()
cor2 = "tab:blue"
ax2.set_ylabel("Media de instrucoes por quantum", color=cor2)
ax2.plot(QUANTA, INSTRUCOES, "s-", color=cor2, label="Instrucoes")
ax2.tick_params(axis="y", labelcolor=cor2)

plt.title("Trocas e instrucoes por quantum")
plt.tight_layout()
plt.savefig(LOG_DIR / "grafico_combinado.png", dpi=150)
plt.close()

# --- grafico 2: trocas isoladas ---
plt.figure(figsize=(8, 5))
plt.plot(QUANTA, TROCAS, "o-", color="tab:red")
plt.xlabel("Quantum")
plt.ylabel("Media de trocas por processo")
plt.title("Media de trocas vs quantum")
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig(LOG_DIR / "grafico_trocas.png", dpi=150)
plt.close()

# --- grafico 3: instrucoes isoladas ---
plt.figure(figsize=(8, 5))
plt.plot(QUANTA, INSTRUCOES, "s-", color="tab:blue")
plt.xlabel("Quantum")
plt.ylabel("Media de instrucoes por quantum")
plt.title("Media de instrucoes vs quantum")
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig(LOG_DIR / "grafico_instrucoes.png", dpi=150)
plt.close()

print("graficos salvos em logs/")
print()
print("| Quantum | Trocas | Instrucoes |")
print("|---------|--------|------------|")
for q, t, i in zip(QUANTA, TROCAS, INSTRUCOES):
    print(f"| {q} | {t:.2f} | {i:.3f} |")
