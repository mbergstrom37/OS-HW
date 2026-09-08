#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/random.h>
#include <linux/string.h>

//Module metadata
MODULE_AUTHOR("Michael Bergstrom");
MODULE_DESCRIPTION("Raptor maze driver");
MODULE_LICENSE("GPL");

static struct proc_dir_entry* proc_entry;

/*
Note: I transalted the Ruby code from the Kruskal's algorithm site given to C with ChatGPT,
      but converted the code myself into kernel code.
*/
typedef struct Edge {
	int x;
	int y;
	int direction;
} Edge;

typedef struct Tree {
	struct Tree *parent;
} Tree;

//1 is north, 2 is south, 4 is east, and 8 is west
int dx(int direction)
{
	switch(direction) {
		case 4:
			return 1;
		case 8:
			return -1;
		case 1:
		case 2:
			return 0;
		default:
			return 0;
	}
}

int dy(int direction)
{
	switch(direction) {
		case 4:
		case 8:
			return 0;
		case 1:
			return -1;
		case 2:
			return 1;
		default:
			return 0;
	}
}

int opposite(int direction)
{
	switch(direction) {
		case 4:
			return 8;
		case 8:
			return 4;
		case 1:
			return 2;
		case 2:
			return 1;
		default:
			return 0;
	}
}

void initialize_tree(Tree *tree)
{
	tree->parent = NULL;
}

Tree *root(Tree *tree)
{
	if(tree->parent != NULL) {
		return root(tree->parent);
	}

	return tree;
}

bool connected(Tree *tree1, Tree *tree2)
{
	return root(tree1) == root(tree2);
}

void connect(Tree *tree1, Tree *tree2)
{
	root(tree2)->parent = tree1;
}

void display_maze(int grid[5][5], char* greeting)
{
	strcat(greeting, "\033[H");
	printk(KERN_INFO "\033[H");
	strcat(greeting, " ");
	printk(KERN_INFO " ");
	int i;
	for(i=0; i<5*2-1; i++) {
		strcat(greeting, "_");
		printk(KERN_INFO "_");
	}

	strcat(greeting, "\n");
	printk(KERN_INFO "\n");
	int y;
	int x;
	for(y=0; y<5; y++) {
		strcat(greeting, "|");
		printk(KERN_INFO "|");

		for(x=0; x<5; x++) {
			int cell = grid[y][x];

			if(cell == 0) {
				strcat(greeting, "\033[47m");
				printk(KERN_INFO "\033[47m");
			}

			if((cell & 2) != 0) {
				strcat(greeting, " ");
				printk(KERN_INFO " ");
			}
			else {
				strcat(greeting, "_");
				printk(KERN_INFO "_");
			}

			if((cell & 4) != 0) {
				if(x+1 < 5) {
					if(((cell | grid[y][x+1]) & 2) != 0) {
						strcat(greeting, " ");
						printk(KERN_INFO " ");
					}
					else {
						strcat(greeting, "_");
						printk(KERN_INFO "_");
					}
				}
				else {
					strcat(greeting, " ");
					printk(KERN_INFO " ");
				}
			}
			else {
				strcat(greeting, "|");
				printk(KERN_INFO "|");
			}

			if(cell == 0) {
				strcat(greeting, "\033[m");
				printk(KERN_INFO "\033[m");
			}
		}
	strcat(greeting, "\n");
	printk(KERN_INFO "\n");
	}
}

void shuffle_edges(Edge edges[], int count)
{
	int i;
	int j;
	Edge temp;
	for(i = count-1; i>0; i--) {
		int j = get_random_u32() % (i + 1);

		Edge temp = edges[i];
		edges[i] = edges[j];
		edges[j] = temp;
	}
}
/*
Name: Michael Bergstrom
Date: 9/7/2026
Description: custom read function
*/
static ssize_t custom_read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
	char greeting[500];
	int grid[5][5];
	Tree sets[5][5];

	int y;
	int x;
	for(y=0; y<5; y++) {
		for(x=0; x<5; x++) {
			grid[y][x] = 0;
		}
	}

	for(y=0; y<5; y++) {
		for(x=0; x<5; x++) {
			initialize_tree(&sets[y][x]);
		}
	}

	int max_edges = (5 * (5-1)) + ((5-1) * 5);

	Edge edges[max_edges];

	int edge_count = 0;

	for(y=0; y<5; y++) {
		for(x=0; x<0; x++) {
			if(y>0) {
				edges[edge_count].x = x;
				edges[edge_count].y = y;
				edges[edge_count].direction = 1;

				edge_count++;
			}

			if(x>0) {
				edges[edge_count].x = x;
				edges[edge_count].y = y;
				edges[edge_count].direction = 8;

				edge_count++;
			}
		}
	}

	shuffle_edges(edges, edge_count);

	strcat(greeting, "\033[2J");
	printk(KERN_INFO "\033[2J");

	Edge current_edge;
	int direction;
	int nx;
	int ny;
	while(edge_count > 0) {
		edge_count--;

		current_edge = edges[edge_count];

		x = current_edge.x;
		y = current_edge.y;
		direction = current_edge.direction;

		nx = x + dx(direction);
		ny = y + dy(direction);

		Tree *set1 = &sets[y][x];
		Tree *set2 = &sets[ny][nx];

		if(!connected(set1, set2)) {
			display_maze(grid, greeting);
			connect(set1, set2);
			grid[y][x] |= direction;
			grid[ny][nx] |= opposite(direction);
		}
	}

	display_maze(grid, greeting);

	int greeting_length = strlen(greeting);

	if(*offset > 0)
		return 0;

	copy_to_user(user_buffer, greeting, greeting_length);
	*offset = greeting_length;

	return greeting_length;
}

/*
Name: Michael Bergstrom
Date: 9/7/2026
Description: file operations struct
*/
static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.read = custom_read
};

/*
Name: Michael Bergstrom
Date: 9/7/2026
Description: custom init function
*/
static int __init custom_init(void)
{
	proc_entry = proc_create("raptormazedriver", 0666, NULL, &fops);
	printk(KERN_INFO "Raptor maze driver loaded.");
	return 0;
}

/*
Name: Michael Bergstrom
Date: 9/7/2026
Description: custom exit function
*/
static void __exit custom_exit(void)
{
	proc_remove(proc_entry);
	printk(KERN_INFO "Goodbye my friend, I shall miss you dearly...");
}

module_init(custom_init);
module_exit(custom_exit);
