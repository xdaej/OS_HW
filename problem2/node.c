
/* struct for tree nodes*/
struct tree_node{
	int children_no;
	char name;
	struct node *left;
	struct node *right;
}

/* struct for creating new nodes */
struct node* newNode(int children_no, char data){
    struct node *temp = new struct node;
    temp->children_no = children_no
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

int main()
{
    struct node *root = new struct node;
    int x;
 
    /* Constructing tree given in the above figure */
    root = newNode(3);
    root->left = newNode(2);
    root->right = newNode(5);
    root->left->left = newNode(1);
    root->left->right = newNode(4);
 
    for (x = 1; x <=5; x++)
    {
      int level = getLevel(root, x);
      if (level)
        printf(" Level of %d is %d\n", x, getLevel(root, x));
      else
        printf(" %d is not present in tree \n", x);
 
    }
 
    getchar();
    return 0;
}
