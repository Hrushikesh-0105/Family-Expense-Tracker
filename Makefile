CC = gcc

CFLAGS = -g

SRC = main.c linked_list.c B_Tree.c user.c family.c expense.c queue.c file_ops.c

OUT = project

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
