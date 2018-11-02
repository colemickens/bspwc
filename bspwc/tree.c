#include "bspwc/tree.h"

struct node *create_node(struct desktop *desktop)
{
	wlr_log(WLR_DEBUG, "Creating node");
	struct node *node = malloc(sizeof(struct node));

	node->window = NULL;

	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;

	wl_list_insert(&desktop->nodes, &node->link);

	return node;
}

void destroy_node(struct node *node)
{
	wlr_log(WLR_DEBUG, "Destroying node");
	if (node->left != NULL)
	{
		destroy_node(node->left);
	}

	if (node->right != NULL)
	{
		destroy_node(node->right);
	}

	destroy_window(node->window);

	wl_list_remove(&node->link);

	free(node);
}

bool insert_node(const struct server *server, struct node **root,
		struct node *child)
{
	const struct config *config = server->config;
	const struct output *output = get_current_output(server);

	if (config->mode == AUTOMATIC)
	{
		wlr_log(WLR_INFO, "Automatic insertion mode is not implemented");
		return false;
	}

	if (*root == NULL)
	{
		wlr_log(WLR_DEBUG, "Inserting %p at root", (void*)child);

		*root = child;

		// TODO: padding
		set_window_position((*root)->window, 0, 0);
		resize_window((*root)->window, output->wlr_output->width,
				output->wlr_output->height);
	}
	else // *root != NULL
	{
		wlr_log(WLR_DEBUG, "Inserting %p into %p", (void*)child, (void*)root);

		double x = (*root)->window->x;
		double y = (*root)->window->y;
		uint32_t width = (*root)->window->width;
		uint32_t height = (*root)->window->height;

		struct node* other_child = create_node(output->desktop);
		other_child->window = (*root)->window;
		(*root)->window = NULL;

		if (config->polarity == LEFT)
		{
			(*root)->left = child;
			(*root)->right = other_child;
		}
		else // config->polarity == RIGHT
		{
			(*root)->left = other_child;
			(*root)->right = child;
		}

		other_child->parent = (*root);
		child->parent = (*root);

		if (config->split == VERTICAL)
		{
			// Resize left
			set_window_position((*root)->left->window, x, y);
			resize_window((*root)->left->window, (width / 2), height);

			// Resize right
			set_window_position((*root)->right->window, x + ((double)width / 2),
					y);
			resize_window((*root)->right->window, (width / 2), height);
		}
		else // config->split == HORIZONTAL
		{
			wlr_log(WLR_ERROR, "Horizontal split is not implemented");
			return false;
		}
	}

	return true;
}

void render_tree(const struct node *root)
{
	if (root == NULL)
	{
		return;
	}

	if (root->window != NULL)
	{
		render_window(root->window);
	}
	else // root->window == NULL
	{
		if (root->left != NULL)
		{
			render_tree(root->left);
		}

		if (root->right != NULL)
		{
			render_tree(root->right);
		}
	}
}
