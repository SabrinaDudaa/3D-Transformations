# 3D Transformations

Visualizador de objetos 3D em **C++ com SDL2**. Lê um modelo em wireframe (vértices e arestas) de um arquivo `.dcg` e permite **rotacionar, transladar e escalar** o objeto em tempo real, vendo o resultado projetado na tela.

As transformações são feitas com **matrizes 4x4 em coordenadas homogêneas**, implementadas do zero (sem biblioteca de álgebra): matriz de modelo (escala, rotações em X, Y e Z e translação), matriz de visualização da câmera e projeção em perspectiva (divisão pela profundidade) para desenhar as arestas na tela.

Projeto desenvolvido no Linux (Fedora) com Visual Studio Code.

## Recursos

- Carrega objetos de arquivos `.dcg` (formato descrito abaixo).
- Rotação nos eixos X, Y e Z, translação em X, Y e Z e escala uniforme, tudo pelo teclado.
- Câmera com posição, ponto de foco e vetor "para cima" (`defineCamera`).
- Menu dentro do programa (`ESC`) com os controles e a troca do arquivo carregado, sem fechar a janela.

## Dependências

- Compilador C++ com suporte a C++17 (`g++`)
- [SDL2](https://www.libsdl.org/) e SDL2_ttf

No Fedora, por exemplo:

```bash
sudo dnf install SDL2-devel SDL2_ttf-devel
```

## Como compilar e rodar

```bash
g++ -std=c++17 main.cpp algebra.cpp camera.cpp objeto.cpp tela.cpp -o t2_cg -lSDL2 -lSDL2_ttf -lm
./t2_cg cubo.dcg
```

O programa exige o arquivo do objeto como argumento. Sem ele, mostra `Uso: ./t2_cg <arquivo.dcg>` e encerra.

## Controles

| Ação | Teclas | Passo por toque |
| --- | --- | --- |
| Rotação no eixo X | `Q` / `E` | 0,05 |
| Rotação no eixo Y | `A` / `D` | 0,05 |
| Rotação no eixo Z | `Z` / `C` | 0,05 |
| Translação em X e Y | setas `←` `→` `↑` `↓` | 0,5 |
| Translação em Z | `R` / `F` | 0,5 |
| Escala | `+` / `-` | 0,1 (mínimo 0,01) |
| Menu | `ESC` | — |

No menu, use `↑` e `↓` para escolher e `Enter` para confirmar. Dá para ver os controles, trocar o arquivo ou sair.

## Formato do arquivo `.dcg`

```
8                  <- número de vértices
-5 5 1             <- um vértice por linha: x y z
5 5 1
...
12                 <- número de arestas
0 1                <- cada aresta liga dois vértices (índices a partir de 0)
1 2
...
```

O `cubo.dcg` incluído é um cubo com 8 vértices e 12 arestas.

## Testes (QA)

Ainda não há testes automatizados: a verificação é manual, pelo roteiro abaixo, que parte do comportamento do código.

| # | Cenário | Passos | Resultado esperado |
| --- | --- | --- | --- |
| 1 | Sem argumento | `./t2_cg` | Mostra a mensagem de uso e encerra com erro |
| 2 | Arquivo inexistente | `./t2_cg nao_existe.dcg` | Mostra `Erro ao abrir arquivo: nao_existe.dcg` e encerra |
| 3 | Carga do cubo | `./t2_cg cubo.dcg` | Janela 800x600 "T2 - Transformacoes 3D" com o cubo de 12 arestas |
| 4 | Rotação | Pressionar `Q`, `E`, `A`, `D`, `Z`, `C` | Gira no eixo certo; teclas opostas desfazem o giro |
| 5 | Translação | Pressionar as setas, `R` e `F` | Move em X/Y (setas) e em Z (`R`/`F`); teclas opostas voltam à posição |
| 6 | Escala | Pressionar `+` várias vezes e depois `-` | Aumenta e diminui de 0,1 em 0,1 |
| 7 | Limite de escala | Segurar `-` | A escala para em 0,01 e o objeto nunca inverte nem some por valor negativo |
| 8 | Menu | `ESC`, depois `ESC` de novo | Abre o menu e volta ao objeto sem perder as transformações |
| 9 | Navegação no menu | `↑` no primeiro item, `↓` no último | A seleção dá a volta (circular) |
| 10 | Trocar arquivo, entrada vazia | Menu > trocar arquivo > `Enter` sem digitar | Mostra "Digite um nome de arquivo!" e continua no diálogo |
| 11 | Trocar arquivo, cancelar | Menu > trocar arquivo > `ESC` | Volta ao menu, mantendo o objeto atual |
| 12 | Trocar arquivo, válido | Menu > trocar arquivo > nome de um `.dcg` válido | Carrega o novo objeto e zera rotação, translação e escala (1,0) |
| 13 | Fechar a janela | Fechar pela janela ou pelo menu | O programa encerra |

### Limitações conhecidas

- O conteúdo do `.dcg` não é validado: o retorno da leitura (`fscanf`) não é conferido, então um arquivo incompleto ou mal formatado não gera uma mensagem de erro.
- Os índices das arestas não são conferidos contra o número de vértices; um índice fora do intervalo pode causar leitura inválida de memória.

## Estrutura

```
├── main.cpp      # laço principal, teclado, menu e diálogo de troca de arquivo
├── algebra.cpp   # matrizes 4x4: identidade, produto matriz x matriz e matriz x ponto
├── objeto.cpp    # carga do .dcg e transformações do modelo
├── camera.cpp    # câmera e matriz de visualização
├── tela.cpp      # janela e desenho das arestas com SDL2
├── font_embed.h  # fonte embutida para o menu
└── cubo.dcg      # objeto de exemplo (cubo)
```
