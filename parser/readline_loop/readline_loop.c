/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline_loop.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 18:31:39 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/16 18:45:26 by jbrol-ca         ###   ########.fr       */
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

static char	*display_prompt_and_get_input(void)
{
	char	*input_str;
	char	*prompt;

	prompt = build_prompt_string();
	if (!prompt)
		prompt = "$ ";
	ft_putstr_fd("💥", 1);
	konopwd(true, "pwd");
	input_str = readline(prompt);
	if (prompt != NULL && strcmp(prompt, "$ ") != 0)
	{
		free(prompt);
		prompt = NULL;
	}
	return (input_str);
}

static int	handle_received_input(char *input_str, t_shell *shell)
{
	if (!input_str)
	{
		handle_eof(shell);
		return (2);
	}
	if (input_str[0] == '\0')
	{
		free(input_str);
		input_str = NULL;
		return (1);
	}
	add_history(input_str);
	input_handler(shell, input_str);
	free(input_str);
	input_str = NULL;
	return (0);
}

void	readline_loop(t_shell *shell)
{
	char	*input_str;
	int		input_status;

	while (1)
	{
		setup_interactive_signals();
		input_str = display_prompt_and_get_input();
		input_status = handle_received_input(input_str, shell);
		if (input_status == 1)
			continue ;
		else if (input_status == 2)
			break ;
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
