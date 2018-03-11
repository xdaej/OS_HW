//  
//  Eric Jiang
//  node.c
//  OS_Problem-2
//

#include <stdio.h>
#include <stdbool.h>

/* struct for tree nodes*/
struct tree_node{
	int children_no;
	char name;
	struct tree_node *left;
	struct tree_node *right;
}typedef tree_node;

/* struct for creating new nodes */
struct tree_node* newNode(int children_no, char name){
    struct tree_node *temp = (struct tree_node*)malloc(sizeof(struct tree_node));
    temp->children_no = children_no;
    temp->name = name;
    temp->left = NULL;
    temp->right = NULL;
    return (temp);
}
	
bool hasleft(tree_node *n){
	if(n->left != NULL)
		return true;
	else
		return false;
}

bool hasright(tree_node *n){
	if(n->right != NULL)
		return true;
	else
		return false;
}
