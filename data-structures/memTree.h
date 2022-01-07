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
    return tree;
}


struct Node *find_size(struct Node *root, int size)
{
    if (root == NULL)
        return NULL;
    if (root->size == size)
        return root;

    if (root->size > size && root->size != 1)
    {
        struct Node *new_left = create_node(root->size / 2, root);
        struct Node *new_right = create_node(root->size / 2, root);

        root->state = HAS_CHILD;

        root->left = new_left;
        root->right = new_right;

        struct Node *found_node = find_size(root->left, size);
        if (found_node != NULL)
            return found_node;

        found_node = find_size(root->right, size);
        if (found_node != NULL)
            return found_node;
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
        return 0;

    // if node is found
    found_node->state = HAS_PROCESS;
    found_node->pid = pid;

    return 1;
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

int deallocate(struct memTree *tree, int pid)
{
    // find process node
    struct Node *found_node = get_process_node(tree->root, pid);

    // if found node is null
    if (found_node == NULL)
        return 0;

    struct Node *parent = found_node->parent;

    if (parent->left == found_node)
    {
        if (parent->right->state == EMPTY)
        {
            recombine_memory(parent);
        }
    }
    else if (parent->right == found_node)
    {
        if (parent->left->state == EMPTY)
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
    printf("%d %d %d\n", root->size, root->pid, root->state);
    print_tree(root->right);
}

