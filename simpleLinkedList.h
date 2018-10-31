
typedef struct {
	int x;
	int y;
    int z;
} DATA;

typedef struct node {
    DATA data;
    struct node* next;
} NODE;

void init(NODE** head); 

bool find(NODE* head, DATA data);

void print_list(NODE* head); 

NODE* add(NODE* node, DATA data); 

void add_at(NODE* node, DATA data); 

void remove_node(NODE* head); 

NODE * reverse_rec(NODE * ptr, NODE * previous); 

NODE * reverse(NODE * node); 

NODE *free_list(NODE *head); 

NODE *sort_list(NODE *head); 

