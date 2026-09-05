# Gerenciador de Processos - Lurds

Este repositório contém o projeto acadêmico **Lurds**.
Ele será parecido com [htop](https://blog.ironlinux.com.br/o-comando-htop-no-linux/).

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
src/              Programa mínimo e implementação de exemplo
tests/unit/       Teste unitário com cmocka
tests/feature/    Teste de feature com cmocka
tests/fixtures/   Arquivos controlados usados pelos testes
build/            Artefatos gerados pelo Makefile (não versionados)
```

## Testes
**IMPORTANTE:** Não esqueça de adicionar novos testes e, principalmente, **NÃO ESQUEÇA DE RODAR OS TESTES ANTES DE SUBIR ALGO**.
Testes são importantes para garantir que o código novo não quebra o código antigo.
