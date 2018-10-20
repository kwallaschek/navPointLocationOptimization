
all: optimizer.cpp 
	$(CFLAGS) g++ optimizer.cpp stack.cpp simpleLinkedList.cpp sq3_data/sim05test.cpp -o optimizer -g
clean:
	rm optimizer