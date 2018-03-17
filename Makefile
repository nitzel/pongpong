EXEFILE=game.exe
CC=g++
CFLAGS=-c -std=c++17 -I ./SFML-2.4.2/include -g
LDFLAGS=-L ./SFML-2.4.2/lib -lsfml-graphics -lsfml-window -lsfml-system

GameState.o: GameState.cpp GameState.hpp IState.hpp StateManager.hpp helpers.hpp makefile
	$(CC) $(CFLAGS) $(LDFLAGS) GameState.cpp

MenuState.o: MenuState.hpp MenuState.cpp IState.hpp StateManager.hpp IPaddleController.hpp makefile
	$(CC) $(CFLAGS) $(LDFLAGS) MenuState.cpp

PaddleAI.o: IPaddleController.hpp PaddleAI.cpp makefile
	$(CC) $(CFLAGS) $(LDFLAGS) PaddleAI.cpp

PaddleKeyboard.o: IPaddleController.hpp PaddleKeyboard.cpp makefile
	$(CC) $(CFLAGS) $(LDFLAGS) PaddleKeyboard.cpp

StateManager.o: StateManager.hpp StateManager.cpp IState.hpp makefile
	$(CC) $(CFLAGS) $(LDFLAGS) StateManager.cpp

helpers.o: helpers.cpp helpers.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) helpers.cpp

main.o: main.cpp StateManager.hpp MenuState.hpp GameState.hpp makefile
	$(CC) $(CFLAGS) $(LDFLAGS) main.cpp

$(EXEFILE): GameState.o MenuState.o PaddleAI.o PaddleKeyboard.o StateManager.o main.o helpers.o makefile
	$(CC) -o $(EXEFILE) GameState.o MenuState.o PaddleAI.o PaddleKeyboard.o StateManager.o helpers.o main.o $(LDFLAGS)

