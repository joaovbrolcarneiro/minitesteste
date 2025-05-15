/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_chain.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/15 22:12:40 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

/*static int	process_redir_chain_node(t_node_tree **current, \
	t_node_tree *redir_nodes[], int *redir_count, t_node_tree **command_node)
{
	if ((*current)->type >= AST_REDIR_IN && (*current)->type <= AST_HEREDOC)
	{
		if (*redir_count < MAX_REDIRECTIONS)
			redir_nodes[(*redir_count)++] = *current;
		else
			return (ft_putstr_fd("konosubash: too many redirections\n", 2), 1);
		if ((*current)->left)
			*current = (*current)->left;
		else if ((*current)->right)
			*current = (*current)->right;
		else
			*current = NULL;
	}
	else if ((*current)->type == AST_COMMAND)
	{
		*command_node = *current;
		*current = NULL;
	}
	else
	{
		ft_putstr_fd("konosubash: internal error: unexpected node type\n", 2);
		return (3);
	}
	return (0);
}
*/
/* Assuming collect_redir_and_command_nodes uses the above correctly */
static int  collect_redir_and_command_nodes(
    t_node_tree *redir_nodes[],
    int *redir_count,
    t_node_tree **command_node_ptr,
    t_node_tree *current_ast_node
)
{
    t_node_tree *out_redir = NULL;
    t_node_tree *in_redir1 = NULL;
    t_node_tree *in_redir2 = NULL;
    t_node_tree *cmd_node_found = NULL;
    bool        pattern_matched = false;

    *redir_count = 0;
    *command_node_ptr = NULL;
    for (int k = 0; k < MAX_REDIRECTIONS; k++) // Initialize array
        redir_nodes[k] = NULL;

    // Check if the root of the current segment is an output redirection
    if (current_ast_node != NULL &&
        (current_ast_node->type == AST_REDIR_OUT || current_ast_node->type == AST_APPEND) &&
        current_ast_node->left != NULL &&
        (current_ast_node->left->type == AST_REDIR_IN || current_ast_node->left->type == AST_HEREDOC))
    {
        out_redir = current_ast_node;       // e.g., > tg or > ht
        in_redir1 = current_ast_node->left; // e.g., << EOF

        // Case 1: AST like > F (L: << H1 (L: C, R: << H2))  (e.g. cat <<EOF <<EOF2 >file)
        if (in_redir1->left != NULL && in_redir1->left->type == AST_COMMAND)
        {
            cmd_node_found = in_redir1->left; // e.g., cat
            if (in_redir1->right != NULL &&
                (in_redir1->right->type == AST_REDIR_IN || in_redir1->right->type == AST_HEREDOC))
            {
                in_redir2 = in_redir1->right; // e.g., << EOF2
            }
            pattern_matched = true;
        }
        // Case 2: AST like > F (L: << H1 (R: << H2)) (no command node in between) (e.g. <<EOF <<EOF2 >file)
        else if (in_redir1->left == NULL && // No command node here
                 in_redir1->right != NULL &&
                 (in_redir1->right->type == AST_REDIR_IN || in_redir1->right->type == AST_HEREDOC))
        {
            // cmd_node_found remains NULL
            in_redir2 = in_redir1->right; // e.g., << EOF2
            pattern_matched = true;
        }
        // Case 3: AST like > F (L: << H1) (no command, no second input redir chained to H1)
        else if (in_redir1->left == NULL && in_redir1->right == NULL)
        {
            // cmd_node_found remains NULL, in_redir2 remains NULL
            pattern_matched = true;
        }


        if (pattern_matched)
        {
            // Populate redir_nodes in reverse order of command-line appearance:
            // For cmd << H1 << H2 > F, cmd line order is H1, H2, F.
            // We want redir_nodes to be [F, H2, H1] for apply_redirection_nodes.
            if (out_redir) redir_nodes[(*redir_count)++] = out_redir; // F (last on cmd line)
            if (in_redir2) redir_nodes[(*redir_count)++] = in_redir2; // H2 (middle)
            if (in_redir1) redir_nodes[(*redir_count)++] = in_redir1; // H1 (first on cmd line)
            *command_node_ptr = cmd_node_found; // NULL if no command in pattern
            return (0);
        }
    }

    // Fallback to original linear chain collection logic if no specific pattern matched above.
    // This handles chains like: R3(L: R2(L: R1(L: CMD))) or R1(L:CMD) or just CMD.
    // It adds nodes as it finds them, traversing left.
    // This results in [R_root, R_child, R_grandchild,...]
    // which is reverse order of appearance for apply_redirection_nodes.
    t_node_tree *current = current_ast_node;
    while (current)
    {
        if (current->type >= AST_REDIR_IN && current->type <= AST_HEREDOC)
        {
            if (*redir_count < MAX_REDIRECTIONS)
                redir_nodes[(*redir_count)++] = current;
            else { ft_putstr_fd("konosubash: too many redirections\n", 2); return (1); }
            
            // Standard traversal for simple chains (user's original logic was good here)
            if (current->left) current = current->left;
            else if (current->right && (current->right->type >= AST_REDIR_IN && current->right->type <= AST_HEREDOC))
                current = current->right; // Check right if left is null and right is input redir
            else if (current->right && current->right->type == AST_COMMAND) // Command on right?
                 current = current->right; // This was missing before, might be needed for some ASTs
            else
                current = NULL;
        }
        else if (current->type == AST_COMMAND)
        {
            *command_node_ptr = current;
            break; 
        }
        else if (current->type == AST_REDIR_OUT || current->type == AST_APPEND)
        {
             if (*redir_count < MAX_REDIRECTIONS)
                redir_nodes[(*redir_count)++] = current;
             else { ft_putstr_fd("konosubash: too many redirections\n", 2); return (1); }
             current = current->left; 
        }
        else
        {
            // If it's not a known redirection or command, it might be an error or end of parsable chain
            // For example, if current_ast_node was just a WORD token initially.
            // ft_putstr_fd("konosubash: internal error: unexpected node in fallback collection\n", 2);
            // return (3); // Or simply break if it's not part of the redir/cmd chain
            break; 
        }
    }
    return (0);
}


static int execute_redir_chain_core(t_shell *shell, t_node_tree *node)
{
    int         status;
    t_node_tree *command_node;
    t_node_tree *redir_nodes[MAX_REDIRECTIONS];
    int         redir_count;

    command_node = NULL; // Initialize
    redir_count = 0;     // Initialize

    // Use the revised collector
    status = collect_redir_and_command_nodes(redir_nodes, &redir_count,
                                             &command_node, node);
    if (status != 0)
    {
        set_current_exit_status(status);
        return (status);
    }

    // Debug collected nodes
    // ft_putstr_fd("DEBUG execute_redir_chain_core: Collected redirs: ", STDERR_FILENO);
    // for (int k=0; k < redir_count; ++k) {
    //     char s[20]; sprintf(s, "type %d, ", redir_nodes[k]->type); ft_putstr_fd(s, STDERR_FILENO);
    // }
    // if (command_node) ft_putstr_fd(" CMD found.", STDERR_FILENO); else ft_putstr_fd(" NO CMD.", STDERR_FILENO);
    // ft_putstr_fd("\n", STDERR_FILENO);


    status = 0; // Reset status before applying redirections
    if (redir_count > 0)
    {
        // apply_redirection_nodes expects redir_nodes[0] to be the LAST on cmd line,
        // redir_nodes[redir_count-1] to be the FIRST on cmd line.
        // It iterates i = redir_count - 1 down to 0.
        status = apply_redirection_nodes(redir_nodes, redir_count, shell);
    }

    if (status == 0) // If redirections applied successfully (or no redirections)
    {
        if (command_node)
            status = execute_simple_command(shell, command_node);
        else if (redir_count > 0) // Redirections but no command (e.g. `> file << EOF`)
            status = 0; // Bash exits 0 for this
        // If no command and no redirections, status remains 0 (should be caught by parser)
    }
    return (set_current_exit_status(status), status);
}


/* Assuming apply_redirection_nodes uses handle_redirections correctly */
int  apply_redirection_nodes(t_node_tree *redir_nodes[],
                                    int redir_count, t_shell *shell)
{
    int i;
    int status;
    int final_output_fd; 
     char i_str[12]; 
     char status_str[12]; 

    status = 0;
    final_output_fd = -1; 

    i = redir_count - 1; 
    while (i >= 0)
    {
        ft_putstr_fd("DEBUG apply_redirection_nodes: Loop start, i = ", STDERR_FILENO);
         sprintf(i_str, "%d", i); 
         ft_putstr_fd(i_str, STDERR_FILENO);
         ft_putstr_fd(", node type = ", STDERR_FILENO);
         if (redir_nodes[i]) {
             sprintf(i_str, "%d", redir_nodes[i]->type); 
             ft_putstr_fd(i_str, STDERR_FILENO);
         } else {
             ft_putstr_fd("NULL", STDERR_FILENO);
         }
         ft_putstr_fd("\n", STDERR_FILENO);

        if (redir_nodes[i] == NULL) 
        {
            i--;
            continue;
        }
        if (redir_nodes[i]->type == AST_HEREDOC)
        {
            status = handle_heredoc(redir_nodes[i], shell);
             ft_putstr_fd("DEBUG apply_redirection_nodes: After handle_heredoc, status = ", STDERR_FILENO);
             sprintf(status_str, "%d", status);
            ft_putstr_fd(status_str, STDERR_FILENO);
            ft_putstr_fd("\n", STDERR_FILENO);
        }
        else if (redir_nodes[i]->type == AST_REDIR_IN)
            status = handle_redir_in(redir_nodes[i]);  
        else if (redir_nodes[i]->type == AST_REDIR_OUT || \
                 redir_nodes[i]->type == AST_APPEND)
            status = open_and_set_final_output_fd(redir_nodes[i], &final_output_fd);
        
        if (status != 0) 
        {
             ft_putstr_fd("DEBUG apply_redirection_nodes: Status non-zero, breaking. i = ", STDERR_FILENO);
            sprintf(i_str, "%d", i); 
             ft_putstr_fd(i_str, STDERR_FILENO);
             ft_putstr_fd("\n", STDERR_FILENO);
            if (final_output_fd != -1) 
                close(final_output_fd); 
            return (status); 
        }
        i--; 
    }

    if (final_output_fd != -1)
    {
        if (dup2(final_output_fd, STDOUT_FILENO) == -1)
        {
            perror("minishell: dup2 final_output_fd to STDOUT_FILENO");
            close(final_output_fd);
            return (1); 
        }
        close(final_output_fd); 
    }
    return (0); 
}

int	execute_redirection_chain(t_shell *shell, t_node_tree *node)
{
	int	status;
	int	original_fds[2];
	int	fds_saved;

	if (!node)
		return (1);
	fds_saved = (save_original_fds(original_fds) == 0);
	status = 0;
	if (!fds_saved)
	{
		perror("konosubash: execute_redirection_chain: Failed to save fds");
		status = 1;
		set_current_exit_status(status);
		return (status);
	}
	status = execute_redir_chain_core(shell, node);
	restore_fds(original_fds);
	close_fds(original_fds);
	return (status);
}
