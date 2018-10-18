all: optimizer.cpp 
	$(CFLAGS) g++ optimizer.cpp stack.cpp simpleLinkedList.cpp -o optimizer -g
clean:
	rm test