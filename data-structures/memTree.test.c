#include "memTree.h"

int main()
{
    struct memTree *tree = create_memTree(128);

    allocate(tree, 1, 20);
    print_tree(tree->root);
    printf("----------------\n");

    // deallocate(tree, 1);
    // print_tree(tree->root);
    // printf("----------------\n");
    // return 0;
    // return 0 ; 
    allocate(tree, 2, 15);
    print_tree(tree->root);
    printf("----------------\n");
    allocate(tree, 3, 10);
    print_tree(tree->root);
    printf("----------------\n");
    allocate(tree, 4, 25);
    print_tree(tree->root);
    printf("----------------\n");
    deallocate(tree, 1);
    print_tree(tree->root);
    printf("----------------\n");
    deallocate(tree, 3);
    print_tree(tree->root);
    printf("----------------\n");

    allocate(tree, 6, 8);
    print_tree(tree->root);
    printf("----------------\n");
    allocate(tree, 5, 30);
    print_tree(tree->root);
    printf("----------------\n");
    deallocate(tree, 2);
    print_tree(tree->root);
    printf("----------------\n");
    allocate(tree, 7, 15);
    print_tree(tree->root);
    printf("----------------\n");
    deallocate(tree, 6);
    deallocate(tree, 5);
    deallocate(tree, 7);
    print_tree(tree->root);
    printf("----------------\n");
    

    return 0;
}
