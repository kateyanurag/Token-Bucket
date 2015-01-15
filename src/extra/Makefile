CC = gcc
CFLAGS = -Wall
DEPS = token_bucket.h my402list.h
OBJ = main.o packet_arrival_thread.o token_depositing_thread.o server_thread.o statistics.o my402list.o clean_up.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -g -c -pthread -o $@ $< -I.

go: $(OBJ)
	gcc -pthread -o $@ $^ -lm  
