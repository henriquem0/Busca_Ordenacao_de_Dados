João Paulo (         )
Henrique Mohr (         )
Pedro Henrique Costa Teixeira (18131584)

# Projeto 1 – Busca e Organização de Dados

SCC0202 – Algoritmos e Estruturas de Dados I (ICMC/USP)

Este README funciona como um relatório para o grupo: a ideia é que qualquer um de nós consiga abrir, ler e entender o que o projeto faz, como rodar e o que ainda falta.

---

## 1. O que é o projeto

É um programa em C que:

1. lê uma planilha grande (arquivo CSV) com registros de aproximação de asteroides da NASA (cerca de 89 mil linhas);
2. filtra as linhas que interessam, seguindo regras escritas num arquivo de texto (a "consulta");
3. dá uma nota de "quanto cada linha se parece com o que a gente procura";
4. guarda as linhas aprovadas numa estrutura de dados;
5. escreve um CSV novo só com o resultado.

O ponto principal do trabalho: **quem roda o programa escolhe quais estruturas de dados usar**, uma para guardar a planilha inteira (`--base`) e outra para guardar o resultado (`--result`). Todas foram feitas por nós (TADs), sem biblioteca pronta. As opções são:

`pilha`, `fila`, `lista_seq`, `lista_enc`, `lista_cabeca`, `lista_ord`, `lista_gen`, `lista_cruz`

Qualquer combinação funciona (são 8 × 8 = 64), e a escolha muda de verdade a forma como os itens ficam organizados.

**Parte 1** (5,0 pts): asteroides. **Parte 2** (5,0 pts): um dataset do [dados.gov.br](https://dados.gov.br/home), reaproveitando *as mesmas estruturas*. Só o "leitor" (o código que transforma uma linha em item) pode ser específico de cada dataset.

---

## 2. Como o programa funciona (passo a passo)

1. Lê os argumentos da linha de comando (`cli.c`).
2. Lê o arquivo de consulta com as regras (`query.c`).
3. Cria duas estruturas vazias: a **base** e o **resultado** (`estrutura.c`).
4. **Carga:** lê o CSV e coloca cada linha, como um ITEM, na base (`reader_nasa.c`).
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
./trabalho --reader nasa --input dados/asteroids.csv \
           --base lista_enc --result lista_ord \
           --query consultas/q1.txt --output resultado.csv \
           --top-k 100 --limit 0
```

| Flag | O que faz |
|---|---|
| `--reader` | qual leitor usar (hoje só `nasa`) |
| `--input` | CSV de entrada |
| `--base` | estrutura que guarda o dataset inteiro |
| `--result` | estrutura que guarda os aceitos |
| `--query` | arquivo com as regras da busca |
| `--output` | CSV que será gerado |
| `--top-k` | máximo de linhas na saída (`0` = todas) |
| `--limit` | máximo de linhas lidas do CSV (`0` = tudo) |

O programa imprime algo assim (os tempos variam):

```
STATUS=OK
INPUT_RECORDS=89227
MATCHED_RECORDS=11409
OUTPUT_RECORDS=100
LOAD_MS=1196.744
SEARCH_MS=157.607
RESULT_INSERT_MS=495.525
WRITE_MS=0.960
```

Se algo der errado (arquivo não existe, estrutura com nome errado, flag faltando), ele imprime `STATUS=ERROR` e termina com código 1.

---

## 4. O arquivo de consulta

Cada linha é um comando, separado por `|`. Exemplo (`consultas/q2.txt`):

```
FILTER|is_future_event|==|true
FILTER|distance_au|<=|0.08
SIM|velocity_km_s|15.0|10.0|1.0
SIM|risk_score|0.70|0.30|2.0
ACCEPT|0.30
ORDER|similarity|DESC
GROUP|threat_category|year
CROSS|threat_category|year
```

| Comando | Significa |
|---|---|
| `FILTER` | condição **obrigatória**. Quem não passa, está fora |
| `SIM` | critério de parecença: `campo \| alvo \| tolerância \| peso` |
| `ACCEPT` | nota mínima para ser aceito |
| `ORDER` | como a `lista_ord` ordena (padrão: similaridade, decrescente) |
| `GROUP` | campos usados pela `lista_gen` (grupo e subgrupo) |
| `CROSS` | campos usados pela `lista_cruz` (eixo A e eixo B) |

**Como a nota é calculada.** Para cada `SIM`: `nota = max(0, 1 − |valor − alvo| / tolerância)`.
Exemplo: alvo 15, tolerância 10, valor 20 → `1 − 5/10 = 0,5`. Valor igual ao alvo dá 1, e a nota chega a 0 quando está longe demais. A **nota final é a média ponderada** pelos pesos. Se não houver nenhum `SIM`, só os filtros decidem.

Campo vazio na linha: o filtro reprova aquele item e o `SIM` dá nota 0 (a leitura nunca para por causa de valor ausente).

---

## 5. As 8 estruturas em uma frase

| Estrutura | Ideia | Ponto de atenção |
|---|---|---|
| `pilha` | vetor; entra e sai pelo topo | percorre do topo para a base, então a saída sai "invertida" |
| `fila` | vetor circular; entra no fim, sai no início | quando enche, dobra e copia na ordem certa |
| `lista_seq` | lista em vetor | remover desloca os itens da direita |
| `lista_enc` | lista de nós ligados | buscar e remover são sequenciais |
| `lista_cabeca` | lista ligada com um **nó sentinela** (cabeça) que não guarda registro | como sempre existe um nó antes do primeiro, remover o primeiro é igual a remover qualquer outro |
| `lista_ord` | lista que **já nasce ordenada** (ordena na inserção, nunca no final) | inserir custa mais (procura o lugar). Empates ficam na ordem de chegada |
| `lista_gen` | lista generalizada: **grupos → subgrupos → itens** (ex.: `threat_category` → `year`) | grupos aparecem na ordem em que foram criados |
| `lista_cruz` | lista cruzada: o **mesmo item** está em duas organizações ao mesmo tempo, sem cópia | cada nó tem dois ponteiros "próximo", um por eixo |

---

## 6. O que todo mundo do grupo precisa saber

**Quem é dono dos itens.** Cada linha do CSV vira um ITEM criado uma vez só. A **base é dona** dos itens (é ela que apaga). O **resultado só guarda ponteiros** para os mesmos itens. Por isso cada TAD tem duas formas de destruir: `_apagar` (apaga os itens também) e `_apagar_sem_itens` (só a estrutura). Se usar a errada, ou vaza memória ou dá `free` duas vezes.

**O ITEM é genérico (importante para a Parte 2).** Ele guarda a linha original e as colunas como texto, e o acesso é *pelo nome da coluna* (`item_get_campo(item, "year")`). Os TADs não sabem nada de asteroides. Para um dataset novo, só criamos um leitor novo (como o `reader_nasa.c`) e registramos o nome dele no `--reader`. Não se mexe nas estruturas.

**A saída é igual à entrada.** O item guarda a linha original, então o CSV de saída tem o mesmo cabeçalho, as mesmas colunas e o mesmo fim de linha (inclusive `\r\n`). Não pode ter coluna extra.

**A ordem da saída depende da estrutura de resultado.** `fila`, `lista_seq`, `lista_enc` e `lista_cabeca` entregam na ordem de inserção. `pilha` entrega do topo para a base. `lista_ord` entrega por nota. `lista_gen` entrega grupo por grupo. `lista_cruz` entrega cadeia por cadeia. É isso que mostra que as estruturas são diferentes.

**`lista_cruz`: só o eixo A é percorrido no programa.** O TAD aceita os dois eixos (a função de percurso recebe o eixo), mas hoje o programa sempre percorre pelo eixo A (`threat_category`). O eixo B (`year`) precisa aparecer em algum teste ou demonstração, porque o enunciado pede "percorrer por categoria **ou** por ano".

**`lista_gen` e `lista_cruz` não têm remover.** Não foi implementado (o programa não precisa). Se alguém precisar, tem que fazer.

**`--limit 10000` na `q1` dá 0 resultados.** Não é bug: nos primeiros 22 mil registros do CSV não existe nenhum evento futuro, e a `q1` filtra por `is_future_event == true`. Para ver resultados, use `--limit 0` ou um limite maior que 22063.

**`lista_ord` é mais lenta na inserção.** Com 11 a 13 mil aceitos, a etapa `RESULT_INSERT_MS` fica em centenas de milissegundos, contra poucos milissegundos nas outras. É o custo de manter a ordem a cada inserção. Isso é esperado e é um bom ponto de discussão no relatório experimental.

---

## 7. Onde fica cada coisa

```
.
├── Makefile
├── apps/main.c          # programa principal (as 8 etapas do item 2)
├── include/             # headers (.h)
│   └── tads/            # um header por estrutura
├── source/
│   ├── cli.c            # lê as flags
│   ├── reader_nasa.c    # lê o CSV da NASA e preenche a base
│   ├── item.c           # um ITEM = uma linha do CSV
│   ├── query.c          # lê e avalia a consulta
│   ├── estrutura.c      # "capa" que esconde qual TAD está por baixo
│   └── tads/            # pilha, fila, lista_seq, lista_enc, lista_cabeca,
│                        # lista_ord, lista_gen, lista_cruz
├── consultas/           # q1.txt, q2.txt
├── dados/               # asteroids.csv (versão oficial do e-Disciplinas)
└── tests/               # testes unitários e script de validação
```

---

## 8. Testes

```bash
make teste                                          # testes unitários dos TADs
bash tests/validar_combinacoes.sh dados/asteroids.csv 30000
```

O segundo roda as 64 combinações de `--base` × `--result` com as duas consultas e compara o resultado com uma versão independente escrita em Python (`tests/referencia.py`, precisa do `python3`). Demora cerca de 45 segundos. Se aparecer `TODAS AS COMBINACOES PASSARAM`, está tudo certo.

O código deve compilar **sem nenhum warning** com `-Wall -Wextra -pedantic`. Antes de entregar, rode `make clean && make` e confira.

---

## 9. O que falta

- [ ] Parte 2: escolher o dataset no dados.gov.br (um grupo por dataset), criar o leitor novo e pelo menos duas consultas úteis.
- [ ] Demonstrar pelo menos uma estrutura avançada na Parte 2 (`lista_cabeca`, `lista_gen` ou `lista_cruz`).
- [ ] Relatório do projeto, com 6 exemplos completos de execução (3 da Parte 1 e 3 da Parte 2): comando, consulta e resultado.
- [ ] Relatório experimental: tabela de complexidade das operações e tabela de tempos (variando N e top-k, no mínimo 3 execuções por configuração, apresentando a média).
- [ ] Revisar os comentários do código. O professor pediu comentários **simples e informais, com as nossas palavras**.

---

## 10. Mini-guia de Git e GitHub

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
- Não suba o executável `trabalho` nem os `.o` (o `.gitignore` já cuida disso).
- Commits pequenos e com mensagem que explica o que mudou.
- Não mexa no mesmo arquivo que outra pessoa sem combinar.

---


Usamos IA (Claude, da Anthropic) como ajuda em partes do projeto, incluindo a revisão do código, dos testes e este README. Onde isso é mais claro, está indicado nos comentários do código. O objetivo é que cada pessoa do grupo consiga **explicar o que cada parte faz**, e foi por isso que este guia foi escrito.
