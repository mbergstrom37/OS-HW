#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/random.h>
#include <linux/string.h>

#define WIDTH  5
#define HEIGHT 5

#define N 1
#define S 2
#define E 4
#define W 8

//Module metadata
MODULE_AUTHOR("Michael Bergstrom");
MODULE_DESCRIPTION("Raptor maze driver");
MODULE_LICENSE("GPL");

static struct proc_dir_entry* proc_entry;

int dx(int direction)
{
	switch (direction)
	{
		case E:
			return 1;
		case W:
			return -1;
		case N:
		case S:
			return 0;
	}

	return 0;
}

int dy(int direction)
{
	switch (direction)
	{
		case E:
		case W:
			return 0;
		case N:
			return -1;
		case S:
			return 1;
	}

	return 0;
}

int opposite(int direction)
{
	switch (direction)
	{
		case E:
			return W;
		case W:
			return E;
		case N:
			return S;
		case S:
			return N;
	}

	return 0;
}

void shuffle(int directions[4])
{
	int i;
	unsigned int j;
	int temp;
	
	for (i = 3; i > 0; i--)
	{
	get_random_bytes(&j, sizeof(j));
		j = j % (i + 1);

		temp = directions[i];
		directions[i] = directions[j];
		directions[j] = temp;
	}
}

//Iterative version of a recursive function made by ChatGPT
void carve_passages_from(int start_x, int start_y, int grid[HEIGHT][WIDTH])
{
	// Each stack entry stores an x/y position
	int stack_x[WIDTH * HEIGHT];
	int stack_y[WIDTH * HEIGHT];

	int top = 0;

	int directions[4];
	int direction;
	int nx;
	int ny;
	int i;
	int cx;
	int cy;
	int found;

	// Start at the initial cell
	stack_x[top] = start_x;
	stack_y[top] = start_y;
	top++;

	while (top > 0)
	{
		// Look at the cell on top of the stack
		cx = stack_x[top - 1];
		cy = stack_y[top - 1];

		// Create and shuffle directions
		directions[0] = N;
		directions[1] = S;
		directions[2] = E;
		directions[3] = W;

		shuffle(directions);

		// Look for an unvisited neighboring cell
		found = 0;

		for (i = 0; i < 4; i++)
		{
			direction = directions[i];

			nx = cx + dx(direction);
			ny = cy + dy(direction);

			// Check that the new position is inside the maze
			if (ny >= 0 && ny < HEIGHT &&
				nx >= 0 && nx < WIDTH &&
				grid[ny][nx] == 0)
			{
				// Create passage from current cell
				grid[cy][cx] |= direction;

				// Create opposite passage in new cell
				grid[ny][nx] |= opposite(direction);

				// Push new cell onto stack
				stack_x[top] = nx;
				stack_y[top] = ny;
				top++;

				found = 1;
				break;
			}
		}

		// No unvisited neighbors:
		// remove this cell from the stack
		if (!found)
		{
			top--;
		}
	}
}

// --------------------------------------------------------------------
// 4. Print the maze as ASCII
// --------------------------------------------------------------------

void print_maze(int grid[HEIGHT][WIDTH], char *maze)
{
	int index = 0;
	int i;
	int y;
	int x;

	// Top border
	maze[index++] = ' ';

	for (i = 0; i < WIDTH * 2 - 1; i++)
	{
		maze[index++] = '_';
	}

	maze[index++] = '\n';

	// Each row
	for (y = 0; y < HEIGHT; y++)
	{
		maze[index++] = '|';

		for (x = 0; x < WIDTH; x++)
		{
			// Bottom wall or passage
			if ((grid[y][x] & S) != 0)
			{
				maze[index++] = ' ';
			}
			else
			{
				maze[index++] = '_';
			}

			// Right wall or passage
			if ((grid[y][x] & E) != 0)
			{
				if (x + 1 < WIDTH &&
					((grid[y][x] | grid[y][x + 1]) & S) != 0)
				{
					maze[index++] = ' ';
				}
				else
				{
					maze[index++] = '_';
				}
			}
			else
			{
				maze[index++] = '|';
			}
		}

		maze[index++] = '\n';
	}

	// Null terminator
	maze[index] = '\0';
}

/*
Name: Michael Bergstrom
Date: 9/7/2026
Description: custom read function
*/
static ssize_t custom_read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
	// Initialize variables
	int grid[HEIGHT][WIDTH] = {0};
	char maze[75];
	int maze_length;

	if(*offset > 0)
		return 0;

	// Generate maze starting at (0, 0)
	carve_passages_from(0, 0, grid);

	// Print maze
	print_maze(grid, maze);

	maze_length = strlen(maze);

	if(count < maze_length)
		maze_length = count;

	if(copy_to_user(user_buffer, maze, maze_length) != 0)
		return -EFAULT;

	*offset = maze_length;

	return maze_length;
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
