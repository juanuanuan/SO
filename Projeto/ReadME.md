# Sistemas Operativos — Trabalho Prático
 
Implementação de um sistema de execução de comandos em contexto **Cliente-Servidor**, desenvolvido no âmbito da unidade curricular de Sistemas Operativos da Universidade do Minho.
 
O cliente (**Runner**) envia pedidos de execução ao servidor (**Controller**), que os gere através de uma fila de espera com política de escalonamento **FCFS** *(First-Come, First-Served)*. A comunicação entre ambos é estabelecida via **FIFOs com nome**, suportando execução de comandos em **pipeline** e em **paralelo**.
 
---
 
## Funcionalidades
 
- Execução de comandos simples, em pipeline (`|`) e em paralelo (`&`)
- Gestão de fila de espera com limite configurável (`MAX_QUEUE`)
- Controlo do nível de paralelismo (`MAX_PARALLEL`)
- Comandos especiais: `-status` e `-exit`
- Reporte do tempo de execução ao servidor
---
 
## Como Utilizar
 
```bash
make                        # Compilar
./bin/Controller            # Iniciar o servidor
./bin/Runner <comandos>     # Iniciar um cliente
make clean                  # Limpar ficheiros temporários
```
 
---
