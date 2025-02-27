C = gcc
CFLAGS = -Wall -Werror -Wextra
LBLIBS = -pthread -lrt

SRC_USER = user_main.c cli.c electronic_branches.c sync.c hash.c log.c loggers.c utils.c queue.c
SRC_SERVER = server_main.c electronic_branches.c sync.c producer.c hash.c cli.c log.c loggers.c utils.c queue.c
OBJ_USER = $(SRC_USER:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)
EXEC1 = server
EXEC2 = user
DEPS = $(patsubst %.c,%.d,$(wildcard *.c))

.PHONY: all clean

all: $(EXEC1) $(EXEC2)

%.o: %.c
	$(C) $(CFLAGS) -MMD -c $< -o $@


$(EXEC1): $(OBJ_SERVER)
	$(C) $(CFLAGS) -o $@ $(OBJ_SERVER) $(LBLIBS)

$(EXEC2): $(OBJ_USER)
	$(C) $(CFLAGS) -o $@ $(OBJ_USER) $(LBLIBS)

clean:
	rm -rf $(EXEC1) $(EXEC2) *.o *.d

-include $(DEPS)