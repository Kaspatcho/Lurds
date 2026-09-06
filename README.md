# Gerenciador de Processos - Lurds

Este repositório contém o projeto acadêmico **Lurds**.
Ele será parecido com [htop](https://blog.ironlinux.com.br/o-comando-htop-no-linux/).

## Lista de Tarefas dos integrantes
### Walison
- [x] Criar ambiente inicial
- [x] Ler e processar /proc.
- [x] Criar struct `Process` e vetor dinamico `ProcessList`.
- [x] Criar `main.c` de exemplo como prova de conceito da estrutura inicial.

### Tiago
- [ ] Calcular métricas de consumo de memória e CPU.
- [ ] Pegar o nome do usuário pelo uid para mostrar na tabela.

### Rafer
- [ ] Melhorar inferface.
- [ ] Olhar sistemas parecidos com o htop para melhorar as informações da tabela.
- [ ] Se possível, adicionar base para interação com o usuário (parte do kaik).

### Kaik
- [ ] Processamento de sinais (SIGTERM, SIGSTOP, SIGCONT e SIGKILL).
- [ ] Interação com o usuário (usuário deve poder escolher um processo para enviar sinal).

### Hannah
- [ ] Filtros.
- [ ] Ordenação (boa sorte).

### Lurds
- [ ] Atualização periódica da tabela.
- [ ] Descobrir onde o programa explode para resolvermos no futuro (melhorar descobrirmos antes do carioca).

### Todos (em especial o lider / tiago)
- Registrar Lessons Learned registrar a evolução 
- Registrar evolução do programa
- Registrar mudanças no cronograma e escopo do projeto.
- Adicionar E RODAR os testes automáticos.

## Dependências

O ambiente Docker instala:

- GCC;
- Make;
- cmocka e seus headers de desenvolvimento.

Para utilizar o projeto, é necessário ter [Docker](https://docs.docker.com/desktop/setup/install/windows-install/)
com o plugin [Docker Compose](https://docs.docker.com/compose/install/) instalado no host.

## Ambiente Docker

Construa a imagem Fedora do projeto:

```bash
docker compose build
```

Para abrir um shell no container com o diretório atual montado em `/workspace`:

```bash
docker compose run --rm dev bash
```

As alterações feitas no host ficam disponíveis imediatamente no container. Também
é possível executar qualquer comando diretamente, por exemplo:

```bash
docker compose run --rm dev make test
```

## Erros esperados
Sei lá, Pra mim deu tudo certo. Boa sorte, espero ter ajudado :)

## Comandos de desenvolvimento

Dentro do container, execute:

```bash
make build
make run
make test
```

`make build` compila o programa mínimo, `make run` o executa e `make test`
compila e executa os testes unitário e de feature. Use `make clean` para remover
os arquivos gerados.

## Estrutura

```text
include/          Headers públicos
src/              Programas implementados
tests/unit/       Teste unitário com cmocka
tests/feature/    Teste de feature com cmocka
tests/fixtures/   Arquivos controlados usados pelos testes
build/            Artefatos gerados pelo Makefile (não versionados)
```

## Testes
**IMPORTANTE:** Não esqueça de adicionar novos testes e, principalmente, **NÃO ESQUEÇA DE RODAR OS TESTES ANTES DE SUBIR ALGO**.
Testes são importantes para garantir que o código novo não quebra o código antigo.

## Como adicionar novos testes:
Nesse projeto, tem dois tipos de testes. Testes de feature (tests/feature) e testes unitarios (tests/unit).
Testes unitarios testam comportamentos isolados do programa, como uma função especifica,
enquanto testes de feature testam funcionalidades esperadas.

Para adicionar um teste unitario, copie o teste de exemplo `./tests/unit/test_example.c` e altere conforme a necessidade,
alterando o nome das funcoes e os asserts. Para adicionar um testes de feature, copie `./tests/feature/test_fixture.c`.

## Padrão de Código
Sinceramente eu não ligo se virar varzea, mas, caso queiram deixar bonitinho,
eu usei o padrão `snake_case` para as funções e variáveis do programa,
com os nomes todos em inglês por uma questão de costume.
Os commits também são uma frase simples em inglês explicando o que foi feito e, se necessário,
um texto mais elaborado com detalhes duas linhas pra baixo.
Normalmente os titulos dos commits usam palavras padronizadas como "Add", "Fix" ou "Update".

**Exemplo de commit:**
```
Simple commit title.

Relevant details like problem description and solution.
```

## Comandos básicos Git
1. Para baixar o projeto: `git clone https://github.com/Kaspatcho/Lurds.git`
2. Marcar quais arquivos serão adicionados no commit: `git add .`
para todos os arquivos ou `git add example.c` para um arquivo especifico
3. Adicionar commit com as mudanças feitas (precisa de arquivos no stage): `git commit`
para mostrar uma interface ou `git commit -m "Meu titulo do commit"` para adicionar direto.
4. Subir commits salvos na nuvem (**TESTE ANTES**): `git push` (provavelmente vai te pedir pra conectar com o seu github)

**Dica:** O VSCode tem integração com o github, então tem uma aba "source control" que facilita mexer no git sem digitar comandos.

## Lessons Learned
Como o carioca deu a ideia, vou adicionar aqui o que eu aprendi durante o desenvolvimento:
1. Aprendi que eu me sinto um homem das cavernas programando em C (não tem bosta nenhuma nessa linguagem)
2. Preste atenção ao tamanho que você está pedindo pra alocar na memoria,
se for muito pequeno, pode dar segfault quando você for acessar no futuro.
3. Adicionar arquivos controlados no `./tests/fixtures` ajuda muito a testar o sistema ao inves de tentar
reproduzir no /proc do fedora toda hora.
4. O compilador do C é muito bom, ele mostra a cagada que você fez na maior parte das vezes
(a não ser que vc tenha se enfiado num segfault da vida).
