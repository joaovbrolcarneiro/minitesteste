/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline_loop.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:31:39 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 23:08:37 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"

static size_t	calculate_prompt_length(const char **title_parts)
{
	size_t	len;
	int		i;

	len = 0;
	i = 0;
	while (title_parts[i])
	{
		len += ft_strlen(title_parts[i]);
		i++;
	}
	return (len);
}

static char	*build_prompt_string(void)
{
	static const char	*title_parts[] = {
		TITLE_P1, TITLE_P2, TITLE_P3, TITLE_P4, TITLE_P5,
		TITLE_P6, TITLE_P7, TITLE_P8, TITLE_P9, TITLE_P10,
		TITLE_P11, TITLE_P12, TITLE_P13, TITLE_P14, TITLE_P15,
		TITLE_P16, TITLE_P17, TITLE_P18, TITLE_P19, TITLE_P20,
		TITLE_P21, TITLE_P22, TITLE_P23, TITLE_P24, TITLE_P25,
		NULL
	};
	size_t				total_len;
	char				*prompt_buffer;
	int					i;

	total_len = calculate_prompt_length(title_parts);
	prompt_buffer = (char *)malloc(sizeof(char) * (total_len + 1));
	if (!prompt_buffer)
		return (NULL);
	*prompt_buffer = '\0';
	i = 0;
	while (title_parts[i])
	{
		ft_strcat(prompt_buffer, title_parts[i]);
		i++;
	}
	return (prompt_buffer);
}

static void	handle_eof(t_shell *shell)
{
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	cleanup_shell(shell);
	exit(get_current_exit_status());
}

static void	setup_interactive_signals(void)
{
	signal(SIGINT, handle_ctrl_c);
	signal(SIGQUIT, SIG_IGN);
}

void	readline_loop(t_shell *shell)
{
	char	*input;
	char	*prompt;

	while (1)
	{
		setup_interactive_signals();
		prompt = build_prompt_string();
		if (!prompt)
			prompt = "$ ";
		ft_putstr_fd("💥", 1);
		konopwd(true, "pwd");
		input = readline(prompt);
		if (prompt != NULL && strcmp(prompt, "$ ") != 0)
			free(prompt);
		if (!input)
			handle_eof(shell);
		if (*input)
			add_history(input);
		if (!ft_strlen(input))
			free(input);
		if (!ft_strlen(input))
			continue ;
		input_handler(shell, input);
		free(input);
	}
}

/* FUNCAO ANTIGA
void readline_loop(char **env)
{
    while (1)
    {
		minigarbege_colector();
		ft_printf("💥"ULI);
		konopwd(true, "pwd");
		ft_printf(RST);
    	char *input = readline(TITLE);
		if(!input)//Ctrl+D
			exit(0);
		if(*input)
			add_history(input);
		is_minishell_exit(input);
		if(!ft_strlen(input))
			continue;
		input_handler(env, input);
	}
}*/
