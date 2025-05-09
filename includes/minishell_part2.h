/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_part2.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 21:06:10 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/09 20:46:43 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_PART2_H
# define MINISHELL_PART2_H

# include "minishell.h"
# include <sys/stat.h> // Include necessary standard headers

// --- Defines ---
// Note: Ensure defines are constants as required by Norminette.
# define QUOTE_SET "!@#&£$% \f\n\r\t\v-+'"
# define RST "\001\e[0m\002"
# define ULI "\001\e[4m\002"
# define BLN "\001\e[5m\002"
# define BLK "\001\e[1;30m\002"
# define PNK "\001\e[38;2;255;0;127m\002"
# define YLW "\001\e[38;2;255;150;0m\002"
# define RDK "\001\e[38;2;220;20;60m\002"
# define GRN "\001\e[1;32m\002"
# define BLU "\001\e[1;34m\002"
# define PRP "\001\e[1;35m\002"
# define CYN "\001\e[1;36m\002"
# define WHT "\001\e[1;37m\002"
# define CLR "\001\e[0;39m\002"
# define STAR "\001⭐\002"
# define YEN "\001¥\002"
# define BWN "\001\e[38;2;105;50;0m\002"

// --- TITLE Components (Replaces original TITLE define) ---
# define TITLE_P1 BWN
# define TITLE_P2 "``"
# define TITLE_P3 PNK
# define TITLE_P4 "K"
# define TITLE_P5 YLW
# define TITLE_P6 "o"
# define TITLE_P7 PNK
# define TITLE_P8 "N"
# define TITLE_P9 YLW
# define TITLE_P10 "o"
# define TITLE_P11 PNK
# define TITLE_P12 "S"
# define TITLE_P13 YLW
# define TITLE_P14 "u"
# define TITLE_P15 PNK
# define TITLE_P16 "B"
# define TITLE_P17 YLW
# define TITLE_P18 "a"
# define TITLE_P19 PNK
# define TITLE_P20 "sh!"
# define TITLE_P21 BWN
# define TITLE_P22 "\""
# define TITLE_P23 GRN
# define TITLE_P24 "$-> "
# define TITLE_P25 RST

typedef struct s_token_vec
{
	t_token	*first;
	t_token	*so_far;
}	t_token_vec;

typedef struct s_flt_vrs
{
	t_token		*cu_tken;
	t_token		*last_highest_found;
	t_ranking	max_rank_found;
}	t_flt_vrs;

typedef struct s_obj_ygg
{
	t_node_tree	*y;
	t_token		*left_child_token;
	t_token		*right_child_token;
}	t_obj_ygg;

typedef struct s_quote_params {
	t_token	*token;
	char	**env;
	int		*is_unclosed_flag;
	bool	is_literal_operator;
}	t_quote_params;
// --- Function Prototypes ---

// General Utils
int			ischarset(const char *set, char c);
t_token		*get_prev_node(t_token *node, t_token *lst);
size_t		ft_strsetlen(char *s, const char *set);
size_t		ft_strnlen(char *s, char n);
long long	get_token_id(void);
char		*get_envar(char **env, char *var);
void		print_env(char **env);
char		**get_path_list(char **env);
bool		search_list(char *search, char **env);
char		*domane_expantion(char **env, char *input);
int			quote_handler_counter(char *input, char **env);
char		*quote_handler_cpy(int count, char *input, char **env);
char		*is_quote_opened(char *input, int *is_unclosed);
bool		handler_quote_operator(char *input);
char		*quote_handler(t_token *token, char **env, int *is_unclosed);
bool		merge_to_token_exception(t_token *token);
void		quote_handler_call_loop(t_token *token, char **env);
bool		proximity_exception(char *input, int i);
t_token		*split_input(char *input, int i);
t_token		*typealize(t_token *token, char **env);
t_token		*new_pipe(t_token *token);
t_token		*new_redir(t_token *token);
t_token		*new_cmd(t_token *token);
t_token		*new_word(t_token *token);
t_token		*new_eof(t_token *token);
t_token		*new_token_super(t_token *token);
bool		is_redir_super(char *input);
bool		is_word_super(char *input);
bool		is_pipe_super(char *input);
bool		is_cmd_super(char *input, char **env);
bool		is_eof(char *input);
void		typealize_call_loop(t_token *token, char **env);
void		merg_tok_excep_cll_revloop(t_token *token);
char		**gather_arguments(t_token *cmd_token,
				t_token *segment_end_token); // Line break for length
t_token		*get_lastone_nodeof_rank(t_token *lst, t_ranking this_ranking);
t_token		*untie_node(t_token *node, t_token *lst);
t_token		*rm_node_lst(t_token *token, t_token *first);
void		add_node_lst(t_token *dst_lst, t_token *token);
t_token		*last_token(t_token *token);
t_token		*remap_lst(t_token *token);
t_token		*find_right_token(t_token *token, t_token *eof);
t_token		*find_left_token(t_token *token, t_token *first);
t_node_tree	*new_yggnode(t_token *token);
t_node_tree	*mke_yggdrasil(t_token *t, t_token *f, t_token *e,
				t_node_tree *y); // Line break for length
bool		token_err(t_token *token_lst);
t_node_tree	*init_yggdrasil(t_token *token_lst);
void		print_yggdrasil(t_node_tree *yggnode, int num_tabs, char *leg);
void		join_and_split(t_token *priv, t_token *arg_token);
t_token		*redir_handler_file(t_token *token, t_token *first);
t_token		*cmd_handler_args(t_token *token, t_token *first);
t_token		*handler_args_file(t_token *token, t_token *first);
void		handler_pipes(t_token_vec *token_vec);
void		handle_ctrl_c(int sig);
t_token		*delegated_by_input_handler(char *input, char **env);
t_token		*input_handler(t_shell *shell, char *input);
void		readline_loop(t_shell *shell);
bool		is_executable(const char *path);
bool		is_regular_file(const char *path);
bool		is_valid_exc(const char *path);
bool		konopwd(bool cmd_exist, const char *input);
bool		is_too_long_input_exption(char *input);
void		parser_cmd_no_found(t_token *token, char **env);
t_gthr_arg	gthr_arg_vrs_init(t_gthr_arg *cu);

// Development / Debugging Utils


char		*print_type(t_token *lst);
void		print_token_lst(t_token *lst);
t_obj_ygg	make_yggdrasil_init(void);
bool		is_redirection(t_obj_ygg obj);
void		handle_parser_error(t_token *t);
bool		set_ygg_children(t_obj_ygg *obj, t_token *t, t_token *f,
				t_token *e); // Broken line for length
bool		has_left(t_obj_ygg obj);
bool		has_right(t_obj_ygg obj);
bool		flt_nrm(t_token *target_node, t_token *first_in_segment,
				t_flt_vrs *vrs); // Broken line for length
void		flt_nrm2(t_flt_vrs *vrs);
bool		gather_arg_helper(t_token *cmd_token, t_gthr_arg *cu);
bool		gather_arg_helper2(t_token *cmd_token, t_gthr_arg *cu);
int			process_current_token_for_args(t_gthr_arg *cu);
bool		is_whitespace(char c);
int			handle_dollar_brace_expansion(t_exp_vars *v);
void		perform_quote_con_part2(t_token	*current, \
	t_token	*node_to_remove, char	*joined_value, char	*temp_value_ptr);
void	initialize_remaining_token_fields(t_token *token,
		const char *input,
		int start,
		int token_len);

// Deprecated / Not to use?
// void is_minishell_exit(char *input); // not to use

#endif // MINISHELL_PART2_H
