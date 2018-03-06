/* struct for nodes*/
struct node{
	char data;
	struct node *left;
	struct node *right;
}

/* struct for creating new nodes */
struct node* newNode(char data)
{
    struct node *temp = new struct node;
    temp->data = data;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}
	
bool hasleft(node n){
	if(n->left != NULL)
		return true;
	else
		return false;
}

bool hasright(node n){
	if(n->right != NULL)
		return true;
	else
		return false;
}

/* Helper function for getLevel().  It returns level of the data if data is
   present in tree, otherwise returns 0.*/
int getLevelUtil(struct node *node, int data, int level)
{
    if (node == NULL)
        return 0;
 
    if (node->data == data)
        return level;
 
    int downlevel = getLevelUtil(node->left, data, level+1);
    if (downlevel != 0)
        return downlevel;
 
    downlevel = getLevelUtil(node->right, data, level+1);
    return downlevel;
}
 
/* Returns level of given data value */
int getLevel(struct node *node, int data)
{
    return getLevelUtil(node,data,1);
}
