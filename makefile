# Macros:
CC = clang++
CPP = -std=c++11 -g
GAME = Tanktastic-o
EXE = run
EXE_C = ttc_client
EXE_S = ttc_server
NAME = -o
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
INC = -ISFML/include/
LIB = -LSFML/lib

SERVER_DEP = main_server.o Server.o Protocol.o Tank.o
CLIENT_DEP = main_client.o Client.o Protocol.o Tank.o Projectile.o

.PHONY: server client clean all install uninstall

# Targets:
all: client server
	@echo "**Built project."

%.o: %.cpp
	$(CC) -c $< -o $@ $(CPP) $(INC)

%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CPP) $(INC)

client: $(CLIENT_DEP)
	@echo
	@echo "**Built client, linking."
	$(CC) $(CLIENT_DEP) $(NAME) $(EXE_C) $(SFML_LIBS) $(LIB)
	@echo

server: $(SERVER_DEP)
	@echo
	@echo "**Built server, linking."
	$(CC) $(SERVER_DEP) $(NAME) $(EXE_S) $(SFML_LIBS) $(LIB)

clean:
	@echo "**Cleaning the mess."
	rm -f *.o $(EXE_C) $(EXE_S)

install: all
	@echo "**Installing the game."
	@cp $(EXE) /usr/bin/$(GAME)
	@cp $(EXE_C) /usr/bin/$(EXE_C)
	@cp $(EXE_S) /usr/bin/$(EXE_S)
	@echo "**Installation complete, you may now run the game by typing $(GAME) -{s|c|h}."

uninstall: clean
	@echo "**Uninstalling the game."
	@rm -f /usr/bin/$(GAME)
	@rm -f /usr/bin/$(EXE_C)
	@rm -f /usr/bin/$(EXE_S)
	@echo "**Uninstallation complete."

#Dependencies:
main_client.o: main_client.cpp
main_server.o: main_server.cpp
Client.o: src/Client.hpp src/Client.cpp src/Tank.hpp src/Projectile.hpp
Server.o: src/Server.hpp src/Server.cpp src/Tank.hpp
Protocol.o: src/Protocol.hpp src/Protocol.cpp
Tank.o: src/Tank.hpp src/Tank.cpp
Projectile.o: src/Projectile.hpp src/Projectile.cpp src/Protocol.hpp
