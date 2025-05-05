/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 15:45:03 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <signal.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <errno.h>
# include <dirent.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <limits.h>
# include <string.h>
# include "../libft/libft.h"
# include "../libft/ft_printf.h"
# include "../libft/get_next_line.h"

/*
** Colors
*/
# define RED				"\033[0;31m"
# define REDK				"\033[38;2;220;20;60m"
# define PINK				"\033[38;2;255;0;127m"
# define BLUE				"\033[0;34m"
# define CYAN				"\033[0;36m"
# define GREEN				"\033[0;32m"
# define YELLOW				"\033[38;2;255;150;0m"
# define BROWN				"\033[38;2;105;50;0m"
# define PURPLE				"\033[0;35m"
# define RESET				"\033[0;0m"
# define MAX_REDIRECTIONS	1024
# define GNL_CLEANUP -42
/*
** Types
*/

typedef enum e_token_type
{
	TOKEN_PIPE = 0,
	TOKEN_REDIR_IN = 1,
	TOKEN_REDIR_OUT = 2,
	TOKEN_APPEND = 3,
	TOKEN_HEREDOC = 4,
	TOKEN_CMD = 5,
	TOKEN_WORD,
	TOKEN_EOF,
	REDIR,
	TOKEN_ASSIGNMENT
}	t_token_type;

typedef enum e_ast_type
{
	AST_PIPE = 0,
	AST_REDIR_IN = 1,
	AST_REDIR_OUT = 2,
	AST_APPEND = 3,
	AST_HEREDOC = 4,
	AST_COMMAND = 5,
}	t_ast_type;

typedef enum e_ranking
{
	RANK_F,
	RANK_E,
	RANK_D,
	RANK_C,
	RANK_B,
	RANK_A,
	RANK_S,
	RANK_SS
}	t_ranking;

typedef struct s_token
{
	long long		id;
	t_token_type	type;
	char			*value;
	struct s_token	*next;
	t_ranking		rank;
	bool			used;
	int				err;
	char			**args;
	char			*file;
	t_token_type	coretype;
	bool			literal;
	bool			join_next;
}	t_token;

typedef struct s_node_tree
{
	t_ranking			rank;
	t_ast_type			type;
	char				**args;
	char				*file;
	char				*content;
	struct s_node_tree	*left;
	struct s_node_tree	*right;
	bool				used;
	int					err;
	t_token_type		coretype;
	bool				literal;
	bool				merge_next;
}	t_node_tree;

typedef struct s_inpt_hndlr
{
	t_token		*token_list;
	t_node_tree	*tree;
	bool		parse_error_flagged_in_tokens;
	t_token		*temp;
}	t_inpt_hndlr;

typedef t_node_tree	*t_tree;

typedef enum e_direction_node
{
	RIGHT,
	LEFT
}	t_direction_node;

typedef struct s_shell
{
	char		**env;
	int			exit_status;
	int			saved_stdin;
	int			saved_stdout;
	int			heredoc_fd;
	int			in_heredoc;
	t_node_tree	*ast_root;
}	t_shell;

typedef struct s_exp_vars
{
	const char	*input;
	char		**env;
	int			last_exit_status;
	size_t		i;
	size_t		res_len;
	size_t		res_cap;
	char		*result;
	size_t		var_start;
	int			pos;
	char		*var_value;
}	t_exp_vars;

/* Structure to hold variables for quote_handler_cpy helper */
typedef struct s_exp_cpy_vars
{
	char		*dst;
	int			i;
	int			start;
	size_t		count;
}	t_exp_cpy_vars;

typedef struct s_command_redir
{
	t_token_type	type;
	char			*file;
}	t_command_redir;

typedef struct s_gthr_arg_vrs
{
	char	**args;
	t_token	*current;
	int		arg_count_total;
	int		arg_capacity;
	int		i;
	char	**temp_realloc;
}	t_gthr_arg;

/*
** Parser functions
*/
t_token		*tokenize(char *input, char **env);
t_node_tree	*parse_tokens(t_token *tokens);
void		free_tokens(t_token *tokens);
char		*expand_vars(char *str, char **env, int status);
void		handle_quotes(char *str, int *i, char quote);

/*
** Executor functions
*/
int			execute_ast(t_shell *shell, t_node_tree *node);
int			execute_command(t_shell *shell, char **args);
int			execute_pipe_command(t_shell *shell, t_node_tree *node);
int			execute_simple_command(t_shell *shell, t_node_tree *node);
int			execute_external_command(t_shell *shell, char **args);
int			handle_redirections(t_node_tree *node, t_shell *shell);
int			handle_heredoc(t_node_tree *node, t_shell *shell);
int			setup_pipes(int pipefd[2]);
void		save_std_fds(t_shell *shell);
void		restore_std_fds(t_shell *shell);
void		handle_child_signals(void);
void		handle_parent_signals(void);
int			handle_redir_in(t_node_tree *node);
int			handle_redir_out(t_node_tree *node);
int			handle_append(t_node_tree *node);
void		restore_fds(int fds[2]);
void		close_fds(int fds[2]);
int			handle_redir_execution(t_shell *shell, t_node_tree *node);
int			handle_command_execution(t_shell *shell, t_node_tree *node);
int			handle_word_token_execution(t_node_tree *node);
int			handle_assignment_execution(t_node_tree *node);
int			handle_pipe_execution(t_shell *shell, t_node_tree *node);
int			save_original_fds(int original_fds[2]);
int			execute_redir_cmd_node(t_shell *shell, t_node_tree *redir_node);
int			read_heredoc_input(int pipe_write_fd, \
									const char *delimiter, char **env);
int			execute_redirection_chain(t_shell *shell, t_node_tree *node);

/*
** Builtin functions
*/
int			ft_echo(char **args);
int			ft_cd(char **args, char ***env);
int			ft_pwd(void);
int			ft_export(char **args, char ***env);
int			ft_exit(char **args, t_shell *shell);
int			ft_unset(char **args, t_shell *shell);

/*
** Utils functions
*/
void		init_shell(t_shell *shell, char **env);
void		cleanup_shell(t_shell *shell);
void		ft_free_strarray(char **array);
char		*find_command_path(char *cmd, char **env);
char		*ft_path_join(char *path, char *file);
char		*get_env_value(char **env, const char *name);
char		*get_envar(char **env, char *var);
size_t		ft_strnlen(char *s, char n);
void		print_env(char **env);
char		**sort_env(char **env);
char		*ft_strcat(char *dest, const char *src);
char		*ft_strcpy(char *dest, const char *src);
bool		is_builtin(const char *cmd);
bool		is_valid_identifier(const char *identifier);
int			update_env(char ***env, char *var, char *value);
void		set_current_exit_status(int status);
int			get_current_exit_status(void);

/*
** Token manipulation functions
*/
void		print_token_lst(t_token *lst);
t_token		*get_lastone_nodeof_rank(t_token *lst, t_ranking this_ranking);
t_token		*get_prev_node(t_token *node, t_token *lst);
t_token		*untie_node(t_token *node, t_token *lst);
t_token		*last_one_of_lst(t_token_type who, t_token *lst);
long long	get_new_token_id(void);
t_token		*create_and_init_token(char *input, int start, int token_len);
void		perform_quote_concatenation(t_token *token_list);
t_token		*finalize_list(t_token *first, t_token *last);
int			get_unquoted_token_len(char *input);

/*
** AST / Tree manipulation functions
*/
char		*domane_expantion(char **env, char *input);
void		expand_token_list(t_token *token_list, char **env);
void		left_child(t_shell *shell, t_node_tree *left, int pipefd[2]);
void		right_child(t_shell *shell, t_node_tree *right, int pipefd[2]);
int			fork_left(t_shell *shell, t_node_tree *node, int pipefd[2]);
int			fork_right(t_shell *shell, t_node_tree *node, int pipefd[2]);

/*
** Expansion functions
*/
int			exp_var_init(t_exp_vars *v, const char *input, char **env);
int			append_char(char **buf, size_t *len, size_t *cap, char c);
void		handle_exit_status(t_exp_vars *v);
void		handle_variable(t_exp_vars *v);
int			handle_dollar_expansion(t_exp_vars *v);
int			append_normal_char(t_exp_vars *v);
char		*get_env_value_exp(const char *name, char **env);
int			is_valid_var_char(char c, int pos);
char		*expand_variables(const char *input, char **env);
void		expand_token_list_no_assignments(t_token *token_list, char **env);

/*
** Assignment functions
*/
int			is_valid_assignment(const char *token);
void		apply_variable_assignment(t_shell *shell, t_token *token_list);
void		process_variable_assignments(t_shell *shell, t_token *token_list);
int			get_token_len(char *input);

/*
** Command list functions
*/
long long	count_commands_in_path(char **env);
char		**get_path_list(char **env);
char		**command_list_malloc(char **env);
int			populate_command_list(char **list, char **env);
char		**init_command_list(char **env);
char		**gather_arg_helper3(t_gthr_arg *cu);

/*
** Error Handling
*/
bool		has_parser_error(t_token *token);
void		st_prsr_err(const char *message, const char *token_value);
void		*ft_realloc(void *ptr, size_t old_size, size_t new_size);
int			process_heredoc_line(char *line, int pipe_write_fd, \
	char **env, bool expand);
char		*gather_filename(t_token *redir_token, t_token *end_token);

#endif /* MINISHELL_H */