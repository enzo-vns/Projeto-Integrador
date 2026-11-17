# Recanto do Bosque — sistema de chalés em C

Aplicação de terminal para o Projeto Integrador. Os dados são mantidos em memória e são perdidos ao encerrar o programa, conforme a etapa descrita no documento do projeto.

## Organização

O arquivo `backend/backend.c` reúne as regras e os dados do sistema; `backend/backend.h` contém as estruturas e declarações usadas pelos demais arquivos. Os arquivos `telas/telas.c` e `telas/telas.h` concentram os menus do terminal.

## Como executar

Na pasta do projeto, execute `make run`. Para compilar sem executar, use `make`; para testar as regras, use `make test`.

Usuários de demonstração: `marcos.recepcao`, `diretoria.admin` e `ana.governanca`, todos com senha `123456`. São credenciais fixas para testes, não para produção.

## Funcionalidades

- Consulta de disponibilidade por intervalo de datas, com saída exclusiva, bloqueio de reservas sobrepostas e entrada não anterior ao dia atual.
- Seleção de hóspedes, chalés e reservas em listas numeradas, sem solicitar identificadores internos.
- Cadastro, consulta por documento e atualização de hóspedes; reservas vinculadas a hóspedes cadastrados.
- Criação, alteração e cancelamento de reservas; registro de horário de entrada e saída.
- Lançamento de consumo em estadias ativas e fechamento com diárias, consumo e confirmação de pagamento.
- Mapa de duas semanas, atualização de limpeza e manutenção e trilha de auditoria em memória.
- Cadastro e atualização de chalés e relatório de ocupação e pagamentos, disponíveis ao administrador. A auditoria também é restrita ao administrador.

As quatro unidades iniciais têm tarifa zero, pois o documento não informa preços. O administrador deve cadastrar as tarifas reais antes de efetuar cobranças. A tarifa de cada reserva é fixada quando ela é criada ou alterada. O relatório calcula a ocupação a partir das datas reais de check-in e check-out e usa pagamentos registrados na data do check-out para faturamento; o fim do período é exclusivo. Uma entrada e saída no mesmo dia contam como um dia ocupado.

## Limitações da versão

- As credenciais permanecem fixas no código e ainda não usam hash e salt (RNF-03); não usar em produção. O programa não possui gestão de usuários ou armazenamento seguro de credenciais.
- A retenção e anonimização dos dados (RNF-05) ainda não foram implementadas. Os dados são voláteis e não sobrevivem à execução.
- Disponibilidade de 99%, SLA de correção e escalabilidade dependem também de infraestrutura e processos; não são garantidos por esta aplicação. Uma medição local com 10.000 reservas e compilação otimizada registrou média de 1,574 ms em 100 consultas; isso não garante o tempo máximo em outros ambientes. O documento apresenta metas divergentes (2 s no título e 100 ms na descrição).
- Os registros são limitados por vetores; não há persistência, integração de pagamento, autenticação forte nem definição de indisponibilidade futura por período de manutenção. O pagamento é apenas registrado em memória.
- O valor das diárias usa as datas previstas da reserva; ainda não há ajuste automático por saída antecipada ou tardia.

## Comparação com o documento

| Requisito | Situação nesta versão |
| --- | --- |
| RF-01: autenticação individual | Login por usuário e senha de demonstração. A proteção das senhas ainda está pendente, como descrito no RNF-03. |
| RF-02: perfis | Recepção e administrador possuem acessos distintos. O perfil adicional de governança foi mantido para as rotinas de limpeza previstas no documento. |
| RF-03: cadastro de chalés | Cadastro e atualização de tipo, capacidade e tarifa. |
| RF-04: hóspedes | Cadastro, consulta por documento, alteração e histórico de reservas em memória. |
| RF-05 a RF-07: disponibilidade e reserva | Consulta por período, vínculo com hóspede e bloqueio de conflitos; a entrada não pode ser anterior à data atual, conforme solicitado. |
| RF-08 e RF-09: reservas e estadias | Alteração e cancelamento antes do check-in, registro de entrada e saída, consumo e pagamento em memória. |
| RF-10: relatório | Dias ocupados por estadias efetivamente iniciadas e pagamentos registrados no período informado. |
| RNF-01: desempenho | Em teste local com 10.000 reservas e compilação `-O2`, a média de 100 consultas foi 1,574 ms de tempo de CPU. O tempo máximo em operação ainda não foi comprovado; o documento menciona tanto 2 s quanto 100 ms. |
| RNF-02: disponibilidade de 99% | Depende de operação, equipamentos e medição; não é garantida pelo programa. |
| RNF-03: hash e salt | Pendente. As senhas são fixas e estão em texto claro; esta versão não é segura para produção. |
| RNF-04: perfis e negação por padrão | As telas verificam os perfis e negam acesso por padrão. As funções internas de dados ainda não formam uma camada independente de autorização. |
| RNF-05: retenção e anonimização | Pendente. A versão atual perde os registros ao encerrar e não implementa retenção ou anonimização. O documento não especifica os prazos aplicáveis. |
| RNF-06: versionamento e SLA | O projeto usa Git; a correção em 24 horas depende da equipe e de processo operacional. |
| RNF-07: escalabilidade | O documento diverge entre título e descrição; crescimento de 3 vezes não foi comprovado. |
| RNF-08 e RNF-09: C estruturado e memória | Aplicação em C com dados em memória, sem banco de dados. Nem todos os tipos possuem operações completas de exclusão ou gestão de usuários. |
