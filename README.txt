INSTALE A BIBLIOETCA SDL2 NA SUA MÁQUINA ANTES DE PROSSEGUIR!
INSTALL THE SDL2 LIBRARY ON YOUR COMPUTER BEFORE PROCEEDING!

================================================================================================================
Projeto feito inteiramente no LINUX (Fedora) com a linguagem C++ na IDE do Visual Code.

Para compilar o projeto, basta rodar no terminal:

g++ -std=c++17 main.cpp algebra.cpp camera.cpp objeto.cpp tela.cpp -o t2_cg -lSDL2 -lSDL2_ttf -lm
./t2_cg cubo.dcg

O programa conta com um menu para demais dúvidas sobre os controles, basta clicar "ESC" quando o programa abrir.

=================================================================================================================

Project developed entirely on Linux (Fedora) using C++ in Visual Studio Code.

To compile the project, run the following commands in your terminal:

g++ -std=c++17 main.cpp algebra.cpp camera.cpp objeto.cpp tela.cpp -o t2_cg -lSDL2 -lSDL2_ttf -lm
./t2_cg cubo.dcg

The program includes a help menu with information about the controls. Simply press "ESC" after launching the program.

From this menu, you can:
- View the controls
- Change the input file
- Exit the program
