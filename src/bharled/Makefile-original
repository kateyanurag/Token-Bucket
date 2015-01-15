CC = gcc
INCLUDE = ../include
CFLAGS = -g -c -Wall -pthread -I$(INCLUDE)

_HEADER = $(INCLUDE)/*.h
HEADER = $(patsubst %,$(INCLUDE)/%,$(_HEADER))

OBJ_DIR =obj
_OBJ = packet_arrival_thread.o server_thread.o token_depositing_thread.o main.o clean_up.o statistics.o my402list.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))




$(OBJ_DIR)/%.o: %.c $(HEADER)
	$(CC) -c -o $@ $< $(CFLAGS)

warmup2: $(OBJ)
	gcc -pthread -o $@ $^ -lm 

.PHONY: clean
clean:
	rm -f *.o *~
	
