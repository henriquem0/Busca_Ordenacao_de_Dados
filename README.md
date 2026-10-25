João Paulo (         )
Henrique Mohr (         )
Pedro Henrique Costa Teixeira (18131584)

# Projeto 1 – Busca e Organização de Dados

SCC0202 – Algoritmos e Estruturas de Dados I (ICMC/USP)

Este README funciona como um relatório para o grupo: a ideia é que qualquer um de nós consiga abrir, ler e entender o que o projeto faz, como rodar e o que ainda falta.

---

## 1. O que é o projeto

É um programa em C que:

1. lê uma planilha (arquivo CSV) com as **autorizações de investimento em concessões de rodovias** da ANTT (3403 linhas, 17 colunas);
2. filtra as linhas que interessam, seguindo regras escritas num arquivo de texto (a "consulta");
3. dá uma nota de "quanto cada linha se parece com o que a gente procura";
4. guarda as linhas aprovadas numa estrutura de dados;
5. escreve um CSV novo só com o resultado.

O ponto principal do trabalho: **quem roda o programa escolhe quais estruturas de dados usar**, uma para guardar a planilha inteira (`--base`) e outra para guardar o resultado (`--result`). Todas foram feitas por nós (TADs), sem biblioteca pronta. As opções são:

`pilha`, `fila`, `lista_seq`, `lista_enc`, `lista_cabeca`, `lista_ord`, `lista_gen`, `lista_cruz`

Qualquer combinação funciona (são 8 × 8 = 64), e a escolha muda de verdade a forma como os itens ficam organizados.

**Parte 1** (5,0 pts): asteroides da NASA. **Parte 2** (5,0 pts): dataset do [dados.gov.br](https://dados.gov.br/home), reaproveitando *as mesmas estruturas*. Só o "leitor" (o código que transforma uma linha em item) pode ser específico de cada dataset.

Esta versão do repositório trabalha com o dataset da **ANTT** (leitor `antt`). O leitor da NASA (`reader_nasa.c`) e o `asteroids.csv` continuam no histórico do Git (commit `a896145`), caso seja preciso rodar a Parte 1 de novo.

---

## 2. Como o programa funciona (passo a passo)

1. Lê os argumentos da linha de comando (`cli.c`).
2. Lê o arquivo de consulta com as regras (`query.c`).
3. Cria duas estruturas vazias: a **base** e o **resultado** (`estrutura.c`).
4. **Carga:** lê o CSV e coloca cada linha, como um ITEM, na base (`reader_antt.c`).
5. **Busca:** percorre a base inteira. O item que passa em todos os filtros e tem nota suficiente é aceito.
6. **Inserção:** coloca os aceitos na estrutura de resultado.
7. **Saída:** grava no CSV de saída os primeiros `top-k` itens do resultado, na ordem em que a estrutura entrega.
8. Imprime o relatório (quantidades e tempos de cada etapa) e libera toda a memória.

---

## 3. Como compilar e rodar

```bash
make            # gera o executável ./trabalho
make run        # roda um exemplo pronto
```

Comando completo:

```bash
./trabalho --reader antt --input dados/autorizacao-investimento-utf8.csv \
           --base lista_enc --result lista_ord \
           --query consultas/q1.txt --output resultado.csv \
           --top-k 100 --limit 0
```

| Flag | O que faz |
|---|---|
| `--reader` | qual leitor usar (hoje só `antt`) |
| `--input` | CSV de entrada (**em UTF-8**, veja a seção 4) |
| `--base` | estrutura que guarda o dataset inteiro |
| `--result` | estrutura que guarda os aceitos |
| `--query` | arquivo com as regras da busca |
| `--output` | CSV que será gerado |
| `--top-k` | máximo de linhas na saída (`0` = todas) |
| `--limit` | máximo de linhas lidas do CSV (`0` = tudo) |

O programa imprime algo assim (os tempos variam). Este é o resultado de `consultas/q2.txt`:

```
STATUS=OK
INPUT_RECORDS=3403
MATCHED_RECORDS=652
OUTPUT_RECORDS=100
LOAD_MS=6.879
SEARCH_MS=2.098
RESULT_INSERT_MS=0.293
WRITE_MS=0.406
```

Se algo der errado (arquivo não existe, estrutura ou leitor com nome errado, flag faltando), ele imprime `STATUS=ERROR` e termina com código 1.

---

## 4. Os dados da ANTT

Arquivo: `dados/autorizacao-investimento-utf8.csv`. Cada linha é uma autorização de um projeto de investimento numa concessão de rodovia.

**Formato do arquivo (importante):**

- separador de colunas: `;` (ponto e vírgula);
- decimal com **vírgula** (`213,1`), então o programa aceita vírgula e ponto nos números;
- fim de linha `\r\n`, que o programa preserva na saída;
- codificação **UTF-8**. O arquivo original que recebemos da ANTT está em Latin-1; por isso a gente converteu uma vez (`iconv -f ISO-8859-1 -t UTF-8 original.csv > autorizacao-investimento-utf8.csv`, sem perda). Sem isso, os nomes de coluna e valores com acento (`situação_do_Projeto`, `Não Objeção`, `6º TA`) nunca batem com a consulta. Guarde o arquivo original da ANTT como backup.

**Colunas** (os nomes nas consultas têm que ser **idênticos** aos do cabeçalho, com maiúsculas e acentos):

| Coluna | O que é | Observação |
|---|---|---|
| `concessionaria` | concessionária (21 valores diferentes) | nunca vazia |
| `id_sigicor` | número do projeto no SIGICOR | repete entre linhas |
| `emitida_no_SEI` | foi emitida no SEI? | sempre `Sim` |
| `processo_SEI_Autorização` | número do processo SEI da autorização | 4 vazias |
| `n_SEI` | número SEI | **mistura** número (`15361913`) e texto (`SEI nº 5393821`, 301 linhas), então trate como texto |
| `item_do_PER` | item do Programa de Exploração da Rodovia | texto, como `3.2.1.2` |
| `origem_PER` | `ORIGINAL`, `1º TA`, `6º TA` etc. | texto |
| `tipo_de_Obra` | tipo da obra | texto |
| `rodovia` | rodovia/UF, como `BR-116/SP` | texto |
| `km_Inicial`, `km_Final` | km do PER/contrato | número; `km_Final` vazio em 742 linhas |
| `prazo_Concessão` | ano da concessão e período | texto |
| `tipo_de_Projeto` | `Projeto Executivo`, `Anteprojeto`, `As built`... | 33 vazias |
| `situação_do_Projeto` | `Aguardando Análise`, `Não Objeção`... | 33 vazias |
| `processo` | processo do projeto | 33 vazias |
| `km_inicial`, `km_final` | km do projeto executivo | **são colunas diferentes** de `km_Inicial` e `km_Final` (só muda a caixa); `km_final` vazio em 1346 linhas |

**Cuidados com os dados:**

- Não existe coluna com valor único por linha: a primeira coluna (`concessionaria`) repete, e há 1007 linhas totalmente iguais a outra. Por isso cada item recebe um **identificador gerado** (1, 2, 3... na ordem de leitura), usado por `buscar` e `remover` nas listas. Ele é texto (`char`) para não precisar mexer em nenhum TAD, e **não aparece** no CSV de saída.
- Campo vazio não vira zero: o filtro reprova o item, o `SIM` dá nota 0 e o `GROUP`/`CROSS` coloca o item no grupo `(vazio)`.

---

## 5. O arquivo de consulta

Cada linha é um comando, separado por `|`. Exemplo (`consultas/q2.txt`):

```
FILTER|km_Inicial|>=|100
FILTER|km_Inicial|<=|400
SIM|km_Inicial|250|100|1.0
SIM|km_Final|300|100|0.5
ACCEPT|0.30
ORDER|similarity|DESC
GROUP|concessionaria|tipo_de_Projeto
CROSS|concessionaria|tipo_de_Projeto
```

| Comando | Significa |
|---|---|
| `FILTER` | condição **obrigatória**: `campo \| operador \| valor`. Quem não passa, está fora |
| `SIM` | critério de parecença: `campo \| alvo \| tolerância \| peso` |
| `ACCEPT` | nota mínima para ser aceito |
| `ORDER` | como a `lista_ord` ordena: `similarity` (padrão) ou um campo numérico, com `ASC` ou `DESC` |
| `GROUP` | campos usados pela `lista_gen` (grupo e subgrupo) |
| `CROSS` | campos usados pela `lista_cruz` (eixo A e eixo B) |

**Operadores do `FILTER`:** `==`, `!=`, `<=`, `>=`, `<`, `>`. Se os dois lados são números, compara como número (`km_Inicial >= 100`). Senão compara como **texto exato**, e só `==` e `!=` fazem sentido (`rodovia == BR-116/SP`, `situação_do_Projeto == Não Objeção`).

**Números na consulta** podem usar vírgula ou ponto (`250,5` e `250.5` dão o mesmo resultado).

**Como a nota é calculada.** Para cada `SIM`: `nota = max(0, 1 − |valor − alvo| / tolerância)`.
Exemplo: alvo 250, tolerância 100, `km_Inicial` 300 → `1 − 50/100 = 0,5`. Valor igual ao alvo dá 1, e a nota chega a 0 quando está longe demais. A **nota final é a média ponderada** pelos pesos. Se não houver nenhum `SIM`, só os filtros decidem (e todos os aceitos ficam com nota 1).

**Exemplos prontos** (3403 registros lidos):

| Consulta | O que busca | Aceitos |
|---|---|---|
| `consultas/q1.txt` | projetos executivos da `RIOSP` com `km_Inicial` ≥ 100 (só filtros) | 353 |
| `consultas/q2.txt` | `km_Inicial` entre 100 e 400, parecido com km 250 (e `km_Final` perto de 300), nota ≥ 0,30 | 652 |

---

## 6. As 8 estruturas em uma frase

| Estrutura | Ideia | Ponto de atenção |
|---|---|---|
| `pilha` | vetor; entra e sai pelo topo | percorre do topo para a base, então a saída sai "invertida" |
| `fila` | vetor circular; entra no fim, sai no início | quando enche, dobra e copia na ordem certa |
| `lista_seq` | lista em vetor | remover desloca os itens da direita |
| `lista_enc` | lista de nós ligados | buscar e remover são sequenciais |
| `lista_cabeca` | lista ligada com um **nó sentinela** (cabeça) que não guarda registro | como sempre existe um nó antes do primeiro, remover o primeiro é igual a remover qualquer outro |
| `lista_ord` | lista que **já nasce ordenada** (ordena na inserção, nunca no final) | inserir custa mais (procura o lugar). Empates ficam na ordem de chegada |
| `lista_gen` | lista generalizada: **grupos → subgrupos → itens** (ex.: `concessionaria` → `tipo_de_Projeto`) | grupos aparecem na ordem em que foram criados |
| `lista_cruz` | lista cruzada: o **mesmo item** está em duas organizações ao mesmo tempo, sem cópia | cada nó tem dois ponteiros "próximo", um por eixo |

---

## 7. O que todo mundo do grupo precisa saber

**Quem é dono dos itens.** Cada linha do CSV vira um ITEM criado uma vez só. A **base é dona** dos itens (é ela que apaga). O **resultado só guarda ponteiros** para os mesmos itens. Por isso cada TAD tem duas formas de destruir: `_apagar` (apaga os itens também) e `_apagar_sem_itens` (só a estrutura). Se usar a errada, ou vaza memória ou dá `free` duas vezes.

**O ITEM é genérico (importante para qualquer dataset).** Ele guarda a linha original e as colunas como texto, e o acesso é *pelo nome da coluna* (`item_get_campo(item, "rodovia")`). Os TADs não sabem nada de rodovias nem de asteroides. Para um dataset novo, só criamos um leitor novo (como o `reader_antt.c`) e registramos o nome dele no `--reader`. Não se mexe nas estruturas.

**As consultas não usam os getters.** Filtros, `SIM`, `ORDER`, `GROUP` e `CROSS` leem os dados pelo nome da coluna (`item_get_campo` e `item_get_campo_numerico`). Os getters prontos (`item_get_rodovia`, `item_get_km_inicio`...) são atalhos para código C e hoje ninguém os chama. Eles devolvem `-1.0` (números) quando a coluna está vazia, e nenhum km da base é negativo, então não há confusão com um valor real.

**A saída é igual à entrada.** O item guarda a linha original, então o CSV de saída tem o mesmo cabeçalho, as mesmas colunas e o mesmo fim de linha (inclusive `\r\n`). Não pode ter coluna extra, e vazio continua vazio.

**A ordem da saída depende da estrutura de resultado.** `fila`, `lista_seq`, `lista_enc` e `lista_cabeca` entregam na ordem de inserção. `pilha` entrega do topo para a base. `lista_ord` entrega por nota. `lista_gen` entrega grupo por grupo. `lista_cruz` entrega cadeia por cadeia. É isso que mostra que as estruturas são diferentes.

**`lista_cruz`: só o eixo A é percorrido no programa.** O TAD aceita os dois eixos (a função de percurso recebe o eixo), mas hoje o programa sempre percorre pelo eixo A (o primeiro campo do `CROSS`, `concessionaria` na `q2`). O eixo B (`tipo_de_Projeto`) precisa aparecer em algum teste ou demonstração, porque o enunciado pede "percorrer por um eixo **ou** pelo outro".

**`lista_gen` e `lista_cruz` não têm remover.** Não foi implementado (o programa não precisa). Se alguém precisar, tem que fazer.

**`--limit` lê só as primeiras linhas do CSV.** Um limite pequeno só olha o começo do arquivo, então uma consulta mais específica pode devolver poucos aceitos (ou 0). Para a base inteira, use `--limit 0`.

**`lista_ord` é mais lenta na inserção.** Ela procura o lugar de cada item para manter a ordem, então a etapa `RESULT_INSERT_MS` cresce mais que nas outras estruturas conforme aumenta o número de aceitos. Esse é o custo esperado de manter a ordem a cada inserção, e é um bom ponto de discussão no relatório experimental.

---

## 8. Onde fica cada coisa

```
.
├── Makefile
├── apps/main.c          # programa principal (as 8 etapas do item 2)
├── include/             # headers (.h)
│   └── tads/            # um header por estrutura
├── source/
│   ├── cli.c            # lê as flags
│   ├── reader_antt.c    # lê o CSV da ANTT e preenche a base
│   ├── item.c           # um ITEM = uma linha do CSV
│   ├── query.c          # lê e avalia a consulta
│   ├── estrutura.c      # "capa" que esconde qual TAD está por baixo
│   └── tads/            # pilha, fila, lista_seq, lista_enc, lista_cabeca,
│                        # lista_ord, lista_gen, lista_cruz
├── consultas/           # q1.txt, q2.txt
├── dados/               # autorizacao-investimento-utf8.csv (veja a seção 4)
└── tests/               # testes unitários e script de validação
```

---

## 9. Testes

```bash
make teste       # testes unitários dos TADs
```

O código deve compilar **sem nenhum warning** com `-Wall -Wextra -pedantic`. Antes de entregar, rode `make clean && make` e confira.

O script `tests/validar_combinacoes.sh` e a referência em Python (`tests/referencia.py`) foram feitos para a base de asteroides. **Ainda precisam ser adaptados** para a ANTT (campos das consultas e regras da referência) antes de valerem como prova das 64 combinações. Veja "O que falta".

---

## 10. O que falta

- [ ] Adaptar `tests/` (script de validação e `referencia.py`) para a base da ANTT e rodar as 64 combinações de `--base` × `--result` com `q1` e `q2`.
- [ ] Demonstrar pelo menos uma estrutura avançada na Parte 2 (`lista_cabeca`, `lista_gen` ou `lista_cruz`), incluindo o percurso pelo eixo B da `lista_cruz`.
- [ ] Conferir se os testes unitários dos TADs ainda passam com o identificador gerado do item (`item_get_identificador` agora devolve o número da linha, e não a primeira coluna).
- [ ] Relatório do projeto, com 6 exemplos completos de execução (3 da Parte 1 e 3 da Parte 2): comando, consulta e resultado.
- [ ] Relatório experimental: tabela de complexidade das operações e tabela de tempos (variando N e top-k, no mínimo 3 execuções por configuração, apresentando a média).
- [ ] Revisar os comentários do código. O professor pediu comentários **simples e informais, com as nossas palavras**.

---

## 11. Mini-guia de Git e GitHub

**Primeira vez (baixar o projeto):**

```bash
git clone https://github.com/henriquem0/Busca_Ordenacao_de_Dados
cd Busca_Ordenacao_de_Dados
```

Se o Git reclamar de nome e e-mail:

```bash
git config --global user.name "Seu Nome"
git config --global user.email "seu@email.com"
```

**Rotina para subir uma mudança:**

```bash
git pull                              # 1. sempre ANTES de começar a mexer
# ... edita os arquivos, roda make e testa ...
git status                            # 2. vê o que mudou
git add source/query.c                # 3. escolhe o que vai (ou "git add ." para tudo)
git commit -m "corrige leitura de filtro vazio"   # 4. mensagem curta dizendo o que mudou
git push                              # 5. envia
```

**Trabalhando em branch (recomendado, como já fizemos no Pull Request #1):**

```bash
git checkout -b feat/leitor-parte2    # cria sua branch
git push -u origin feat/leitor-parte2 # envia a branch (só na primeira vez)
```

Depois, no GitHub, clique em **Compare & pull request**, escreva o que mudou e peça para alguém do grupo olhar antes do **Merge**. Depois do merge, todo mundo volta para a principal com `git checkout main && git pull`.

**Se o `git push` for recusado** (`rejected` / `non-fast-forward`), alguém subiu algo antes de você:

```bash
git pull --rebase
git push
```

**Se der conflito:** abra o arquivo, procure `<<<<<<<`, `=======` e `>>>>>>>`, deixe só a versão certa e apague as marcas. Depois:

```bash
git add arquivo.c
git rebase --continue
```

**Sem usar o Git (pelo site):** na página do repositório, clique em **Add file → Upload files**, arraste os arquivos, escreva uma mensagem e clique em **Commit changes**.

**Regras de ouro do grupo:**
- `git pull` antes de mexer e `make clean && make` antes de subir.
- Não suba o executável `trabalho` (nem `trabalho.exe`), os `.o` ou a pasta `build/` (o `.gitignore` já cuida dos dois primeiros; confira se `build/` também está lá).
- Commits pequenos e com mensagem que explica o que mudou.
- Não mexa no mesmo arquivo que outra pessoa sem combinar.

---

Usamos IA (Claude, da Anthropic) como ajuda em partes do projeto, incluindo a revisão do código, dos testes e este README. Onde isso é mais claro, está indicado nos comentários do código. O objetivo é que cada pessoa do grupo consiga **explicar o que cada parte faz**, e foi por isso que este guia foi escrito.
