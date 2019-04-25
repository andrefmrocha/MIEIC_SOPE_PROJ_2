C = gcc
CFLAGS = -Wall -Werror -Wextra
LBLIBS = -pthread -lrt

SRC_USER = user_main.c
SRC_SERVER = server_main.c
OBJ_USER = $(SRC_USER:.cc=.o)
OBJ_SERVER = $(SRC_SERVER:.cc=.o)
EXEC1 = server
EXEC2 = user

all: $(EXEC1) $(EXEC2)

$(EXEC1): $(OBJ_SERVER)
	$(C) $(CFLAGS) -o $@ $(OBJ_SERVER) $(LBLIBS)

$(EXEC2): $(OBJ_USER)
	$(C) $(CFLAGS) -o $@ $(OBJ_USER) $(LBLIBS)

clean:
	rm -rf $(EXEC) *.o