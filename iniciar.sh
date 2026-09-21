#!/bin/bash

echo "============================================================"
echo "   Iniciando Sistema Recanto Bosque (Front-end + Back-end)  "
echo "============================================================"

# Se o Node.js estiver disponível, roda o servidor Node com API completa
if command -v node &> /dev/null; then
    node server.js
elif command -v python3 &> /dev/null; then
    python3 server.py
else
    echo "Erro: Nem Node.js nem Python3 foram encontrados no sistema."
    exit 1
fi
