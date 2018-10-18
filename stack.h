
struct Stack;

struct Stack* createStack(unsigned capacity);

int isFull(struct Stack* stack);
int isEmpty(struct Stack* stack);
void push(struct Stack* stack, int item);
int pop(struct Stack* stack);
