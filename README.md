# Recanto do Bosque — sistema de chalés em C

Aplicação de terminal para o Projeto Integrador. Os dados são mantidos em memória e são perdidos ao encerrar o programa, conforme a etapa descrita no documento do projeto.

## Organização

O arquivo `backend/backend.c` reúne as regras e os dados do sistema; `backend/backend.h` contém as estruturas e declarações usadas pelos demais arquivos. Os arquivos `telas/telas.c` e `telas/telas.h` concentram os menus do terminal.

## Como executar

Na pasta do projeto, execute `make run`. Para compilar sem executar, use `make`; para testar as regras, use `make test`.

Usuários iniciais de demonstração: `marcos.recepcao`, `diretoria.admin` e `ana.governanca`, todos com senha `123456`. Essas credenciais são públicas para testes e não devem ser usadas em produção. Os valores de autenticação iniciais são hashes PBKDF2-HMAC-SHA256 com salts individuais; novas senhas usam salt aleatório. O administrador pode cadastrar, atualizar e excluir usuários durante a execução. Alterações desaparecem ao encerrar.

No macOS, a compilação usa CommonCrypto. No Linux, requer cabeçalhos e biblioteca de desenvolvimento OpenSSL (`libcrypto`).

### Executar no Windows com PowerShell

Instale o GCC para Windows (MinGW-w64) e adicione-o ao `PATH`. Na pasta do projeto, execute:

```powershell
powershell -ExecutionPolicy Bypass -File .\executar.ps1
```

O script compila e abre o sistema. Para compilar e executar os testes, use `powershell -ExecutionPolicy Bypass -File .\executar.ps1 -Testar`. O Windows usa a biblioteca criptográfica do próprio sistema; não é necessário instalar OpenSSL ou Make para usar o script.

## Funcionalidades

- Consulta de disponibilidade por intervalo de datas, com saída exclusiva, bloqueio de reservas sobrepostas e entrada não anterior ao dia atual.
- Listas de hóspedes exibem apenas ID e nome, com seleção pelo ID. Chalés e reservas são escolhidos em listas numeradas.
- Cadastro, consulta por ID ou documento e atualização de hóspedes; reservas vinculadas a hóspedes cadastrados. O administrador pode excluir hóspedes sem reservas, chalés sem reservas e reservas canceladas sem check-in.
- Gestão de usuários em memória pelo administrador, protegendo a própria conta e o último administrador contra exclusão.
- Criação, alteração e cancelamento de reservas; registro de horário de entrada e saída.
- Lançamento de consumo em estadias ativas e fechamento com diárias, consumo e confirmação de pagamento.
- Mapa mensal com navegação entre meses, atualização de limpeza e manutenção e trilha de auditoria em memória.
- Cadastro e atualização de chalés com opções de tipo Standard, Família ou Luxo, e relatório de ocupação e pagamentos, disponíveis ao administrador. A auditoria também é restrita ao administrador.

O sistema inicia com 10 hóspedes, 8 chalés e 12 reservas de demonstração, sendo duas estadias iniciadas e com consumos de exemplo. As datas são relativas ao dia de abertura. Os nomes, documentos, capacidades e tarifas são dados fictícios usados apenas para testes; o administrador deve ajustá-los antes de qualquer uso real. A tarifa de cada reserva é fixada quando ela é criada ou alterada. O check-in só é aceito entre a data prevista de entrada e o dia anterior à saída prevista. O relatório calcula a ocupação a partir das datas reais de check-in e check-out e usa pagamentos registrados na data do check-out para faturamento; o fim do período é exclusivo. Uma entrada e saída no mesmo dia contam como um dia ocupado.

## Limitações da versão

- As credenciais iniciais são públicas e comuns apenas para demonstração. Embora hashes com salts sejam usados, o programa não oferece recuperação de senha, bloqueio de tentativas, ocultação de entrada nem persistência de usuários. Não usar em produção.
- A retenção e anonimização dos dados (RNF-05) ainda não foram implementadas. Os dados são voláteis e não sobrevivem à execução. O documento não informa um prazo de guarda; a solução de retenção requer definir a política legal e um meio de armazenamento compatível com a etapa do projeto.
- Disponibilidade de 99%, SLA de correção e escalabilidade dependem também de infraestrutura e processos; não são garantidos por esta aplicação. Uma medição local com 10.000 reservas e compilação otimizada registrou média de 1,574 ms em 100 consultas; isso não garante o tempo máximo em outros ambientes. O documento apresenta metas divergentes (2 s no título e 100 ms na descrição).
- Os registros são limitados por vetores; não há persistência, integração de pagamento, autenticação forte nem definição de indisponibilidade futura por período de manutenção. O pagamento é apenas registrado em memória.
- O valor das diárias usa as datas previstas da reserva; ainda não há ajuste automático por saída antecipada ou tardia.

## Comparação com o documento

| Requisito | Situação nesta versão |
| --- | --- |
| RF-01: autenticação individual | Login por usuário e senha; contas iniciais são apenas para demonstração, e o administrador pode gerenciar usuários em memória. |
| RF-02: perfis | Recepção e administrador possuem acessos distintos. O perfil adicional de governança foi mantido para as rotinas de limpeza previstas no documento. |
| RF-03: cadastro de chalés | Cadastro e atualização de tipo, capacidade e tarifa. |
| RF-04: hóspedes | Cadastro, consulta por documento, alteração e histórico de reservas em memória. |
| RF-05 a RF-07: disponibilidade e reserva | Consulta por período, vínculo com hóspede e bloqueio de conflitos; a entrada não pode ser anterior à data atual, conforme solicitado. |
| RF-08 e RF-09: reservas e estadias | Alteração e cancelamento antes do check-in, registro de entrada e saída, consumo e pagamento em memória. |
| RF-10: relatório | Dias ocupados por estadias efetivamente iniciadas e pagamentos registrados no período informado. |
| RNF-01: desempenho | Em teste local com 10.000 reservas e compilação `-O2`, a média de 100 consultas foi 1,574 ms de tempo de CPU. O tempo máximo em operação ainda não foi comprovado; o documento menciona tanto 2 s quanto 100 ms. |
| RNF-02: disponibilidade de 99% | Depende de operação, equipamentos e medição; não é garantida pelo programa. |
| RNF-03: hash e salt | PBKDF2-HMAC-SHA256 com 310.000 iterações e salt individual; contas de demonstração têm senha pública e dados em memória. |
| RNF-04: perfis e negação por padrão | As telas verificam os perfis e negam acesso por padrão. As funções internas de dados ainda não formam uma camada independente de autorização. |
| RNF-05: retenção e anonimização | Pendente. A versão atual perde os registros ao encerrar e não implementa retenção ou anonimização. O documento não especifica os prazos aplicáveis. |
| RNF-06: versionamento e SLA | O projeto usa Git; a correção em 24 horas depende da equipe e de processo operacional. |
| RNF-07: escalabilidade | O documento diverge entre título e descrição; crescimento de 3 vezes não foi comprovado. |
| RNF-08 e RNF-09: C estruturado e memória | Aplicação em C com dados em memória, sem banco de dados. As exclusões preservam vínculos e o histórico: hóspedes e chalés com reservas não podem ser excluídos; reservas só podem ser excluídas após cancelamento e antes do check-in. |
