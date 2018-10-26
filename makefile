
all: optimizer.cpp 
	$(CFLAGS) g++ -fopenmp optimizer_pg16.cpp stack.cpp simpleLinkedList.cpp sq3_data/pg16sim01.cpp -o optimizer -g 
clean:
	rm optimizer