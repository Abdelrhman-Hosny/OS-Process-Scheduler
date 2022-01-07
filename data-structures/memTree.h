#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MEM_SIZE 128
#define EMPTY 0
#define HAS_PROCESS 1
#define HAS_CHILD 2
#define ROOT 4

struct Node
{
    int size;
    int pid;
    int state;
    int mem_start_position;
    int capacitiy;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
};

struct Node *create_node(int size, struct Node *parent)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->size = size;
    node->pid = -1;
    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    node->capacitiy = 0;
    return node;
}

struct memTree
{
    /* data */
    struct Node *root;
};

struct memTree *create_memTree()
{
    struct memTree *tree = (struct memTree *)malloc(sizeof(struct memTree));
    tree->root = create_node(MEM_SIZE, NULL);
    tree->root->state = ROOT;
    tree->root->mem_start_position = 0;
    return tree;
}

struct Node *find_size(struct Node *root, int size)
{
    if (root == NULL)
        return NULL;
    if (root->size == size)
    {
        root->capacitiy += size;
        return root;
    }

    if (root->size > size)
    {
        // no children
        if (root->state == EMPTY || (root->state == ROOT && root->left == NULL && root->right == NULL))
        {

            struct Node *new_left = create_node(root->size / 2, root);
            struct Node *new_right = create_node(root->size / 2, root);

            root->state = HAS_CHILD;

            root->left = new_left;
            root->right = new_right;

            root->left->mem_start_position = root->mem_start_position;
            root->right->mem_start_position = root->mem_start_position + root->size / 2;

            if (root->left->size == size)
            {
                new_left->capacitiy = new_left->size;
                root->capacitiy += new_left->capacitiy;
                // printf("(root value , capacity): %d\n", (root->size, root->capacitiy));
                return root->left;
            }
            else
            {
                struct Node *found = find_size(root->left, size);
                root->capacitiy += found->capacitiy;
                // printf("(root value , capacity): %d\n", (root->size, root->capacitiy));
                return found;
            }
        }
        else if (root->state == HAS_CHILD)
        {
            // you should go for lowest capacity first
            // printf("has child \n");
            struct Node *found_node = NULL;
            int full_size = root->left->size;
            // printf("(full_size - root->left->capacitiy): %d\n", (full_size - root->left->capacitiy));
            // printf("(full_size - root->right->capacitiy): %d\n", (full_size - root->right->capacitiy));

            if ((full_size - root->left->capacitiy) < (full_size - root->right->capacitiy) &&
                (full_size - root->left->capacitiy) >= size)
            {
                // printf("entered left \n");
                // means left is tighter than right
                found_node = find_size(root->left, size);
                if (found_node != NULL && found_node->state == EMPTY)
                {
                    root->capacitiy += found_node->capacitiy;
                    return found_node;
                }
            }
            else
            {
                // printf(" entered right subtree of root %d , %d \n", root->size, root->capacitiy);
                found_node = find_size(root->right, size);
                if (found_node != NULL && found_node->state == EMPTY)
                {
                    root->capacitiy += found_node->capacitiy;

                    return found_node;
                }
            }
        }
    }
    return NULL;
}

struct Node *get_process_node(struct Node *root, int pid)
{
    if (root == NULL)
        return NULL;
    if (root->pid == pid)
        return root;

    struct Node *found_node = get_process_node(root->left, pid);
    if (found_node != NULL)
        return found_node;

    found_node = get_process_node(root->right, pid);
    if (found_node != NULL)
        return found_node;

    return NULL;
}

struct Node *find_closest_size(struct memTree *tree, int size)
{
    // get log2 of size
    float log2_size = log2(size);
    int wanted_size = pow(2, ceil(log2_size));

    // call find_closest_node
    struct Node *found_node = find_size(tree->root, wanted_size);

    return found_node;
}

int allocate(struct memTree *tree, int pid, int process_size)
{
    // find closest node
    struct Node *found_node = find_closest_size(tree, process_size);

    // if found node is null
    if (found_node == NULL)
        return -1;

    // if node is found
    found_node->state = HAS_PROCESS;
    found_node->pid = pid;

    return found_node->mem_start_position;
}

void delete_children(struct Node *found_node_parent)
{
    free(found_node_parent->left);
    found_node_parent->left = NULL;
    free(found_node_parent->right);
    found_node_parent->right = NULL;
    found_node_parent->state = EMPTY;
}

void recombine_memory(struct Node *parent)
{
    struct Node *found_node_parent = parent;
    struct Node *my_parent_temp = parent;

    while (found_node_parent)
    {
        if (found_node_parent->left->state == EMPTY &&
            found_node_parent->right->state == EMPTY)
        {
            delete_children(found_node_parent);
        }

        if (found_node_parent->state != ROOT)
            found_node_parent = found_node_parent->parent;
    }
}
void decrementCapacity(struct Node *parent)
{
    // decrementing till root
    int size_to_decrement = parent->size / 2;
    while (parent != NULL)
    {
        parent->capacitiy -= (size_to_decrement);
        parent = parent->parent;
    }
}

int deallocate(struct memTree *tree, int pid)
{
    // find process node
    struct Node *found_node = get_process_node(tree->root, pid);

    // if found node is null
    if (found_node == NULL)
        return -1;

    printf("deallocate: %d\n", pid);

    found_node->state = EMPTY;
    found_node->pid = -1;

    struct Node *parent = found_node->parent;
    decrementCapacity(parent);

    if (parent->left == found_node)
    {
        if (parent->right->state == EMPTY || parent->right->state == ROOT)
        {
            recombine_memory(parent);
        }
    }
    else if (parent->right == found_node)
    {
        if (parent->left->state == EMPTY || parent->right->state == ROOT)
        {
            recombine_memory(parent);
        }
    }
    return 1;
}

void print_tree(struct Node *root)
{
    if (root == NULL)
        return;

    print_tree(root->left);
    printf("%d %d %d %d - %d\n", root->size, root->pid, root->capacitiy, root->mem_start_position, root->mem_start_position + root->size);

    print_tree(root->right);
}
