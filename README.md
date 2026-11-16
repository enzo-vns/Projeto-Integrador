# Recanto do Bosque — sistema de chalés em C

Aplicação de terminal para o Projeto Integrador. Os dados são mantidos em memória e são perdidos ao encerrar o programa, conforme a etapa descrita no documento do projeto.

## Organização

O arquivo `backend/backend.c` reúne as regras e os dados do sistema; `backend/backend.h` contém as estruturas e declarações usadas pelos demais arquivos. Os arquivos `telas/telas.c` e `telas/telas.h` concentram os menus do terminal.

## Como executar

Na pasta do projeto, execute `make run`. Para compilar sem executar, use `make`; para testar as regras, use `make test`.

Usuários de demonstração: `marcos.recepcao`, `diretoria.admin` e `ana.governanca`, todos com senha `123456`. São credenciais fixas para testes, não para produção.

## Funcionalidades

- Consulta de disponibilidade por intervalo de datas, com saída exclusiva e bloqueio de reservas sobrepostas.
- Cadastro, consulta por documento e atualização de hóspedes; reservas vinculadas a hóspedes cadastrados.
- Criação, alteração e cancelamento de reservas; registro de horário de entrada e saída.
- Lançamento de consumo em estadias ativas e fechamento com diárias, consumo e confirmação de pagamento.
- Mapa de duas semanas, atualização de limpeza e manutenção e trilha de auditoria em memória.
- Cadastro e atualização de chalés e relatório de ocupação e pagamentos, disponíveis ao administrador. A auditoria também é restrita ao administrador.

As quatro unidades iniciais têm tarifa zero, pois o documento não informa preços. O administrador deve cadastrar as tarifas reais antes de efetuar cobranças. A tarifa de cada reserva é fixada quando ela é criada ou alterada. O relatório usa diárias reservadas para a taxa de ocupação, e pagamentos registrados na data do check-out para faturamento; o fim do período é exclusivo.

## Limitações da versão

- As credenciais permanecem fixas no código e ainda não usam hash e salt (RNF-03); não usar em produção. O programa não possui gestão de usuários ou armazenamento seguro de credenciais.
- A retenção e anonimização dos dados (RNF-05) ainda não foram implementadas. Os dados são voláteis e não sobrevivem à execução.
- Disponibilidade de 99%, SLA de correção e escalabilidade dependem também de infraestrutura e processos; não são garantidos por esta aplicação. O tempo de consulta para 10.000 reservas ainda precisa de medição formal. O documento apresenta metas divergentes para esse tempo (2 s no título e 100 ms na descrição).
- Os registros são limitados por vetores; não há persistência, integração de pagamento, autenticação forte nem definição de indisponibilidade futura por período de manutenção. O pagamento é apenas registrado em memória.
- O relatório apresenta ocupação prevista com base em reservas, não comprovação de presença. Datas de estadia usadas no cálculo são as previstas; não há ajuste automático por saída antecipada ou tardia.
