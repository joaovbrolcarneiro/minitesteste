/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenlst.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbrol-ca <jbrol-ca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 18:10:25 by hde-barr          #+#    #+#             */
/*   Updated: 2025/05/05 23:01:19 by jbrol-ca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "minishell_part2.h"
#include <dirent.h>

static long long	count_dir_entries(DIR *dir)
{
	long long		count;
	struct dirent	*entry;

	count = 0;
	rewinddir(dir);
	while (1)
	{
		entry = readdir(dir);
		if (!entry)
			break ;
		if (ft_strcmp(entry->d_name, ".") != 0 \
		&& ft_strcmp(entry->d_name, "..") != 0)
			count++;
	}
	return (count);
}

/* Counts command names in PATH dirs */
long long	count_commands_in_path(char **env)
{
	long long	count;
	DIR			*dir;
	char		**path_list;
	int			i;

	count = 0;
	path_list = get_path_list(env);
	if (!path_list)
		return (-1);
	i = 0;
	while (path_list[i])
	{
		dir = opendir(path_list[i]);
		if (dir != NULL)
		{
			count += count_dir_entries(dir);
			closedir(dir);
		}
		i++;
	}
	return (count);
}

/* Populates list with entries from dir, updates current_index */
static int	populate_list_from_dir(char **list, DIR *dir, \
									long long *current_index)
{
	struct dirent	*d;

	rewinddir(dir);
	while (1)
	{
		d = readdir(dir);
		if (!d)
			break ;
		if (ft_strcmp(d->d_name, ".") == 0 || ft_strcmp(d->d_name, "..") == 0)
			continue ;
		list[*current_index] = ft_strdup(d->d_name);
		if (!list[*current_index])
		{
			perror("minishell: strdup command list");
			closedir(dir);
			return (1);
		}
		(*current_index)++;
	}
	closedir(dir);
	return (0);
}

/* Populates the command list array */
int	populate_command_list(char **list, char **env)
{
	char		**path_list;
	DIR			*dir;
	long long	current_index;
	int			path_idx;
	int			status;

	current_index = 0;
	path_list = get_path_list(env);
	if (!path_list)
		return (1);
	path_idx = 0;
	status = 0;
	while (path_list[path_idx] && status == 0)
	{
		dir = opendir(path_list[path_idx]);
		if (dir != NULL)
		{
			status = populate_list_from_dir(list, dir, &current_index);
		}
		path_idx++;
	}
	list[current_index] = NULL;
	if (status != 0)
		return (1);
	return (0);
}

/* Initializes the command list */
char	**init_command_list(char **env)
{
	char	**list;
	int		populate_status;

	list = command_list_malloc(env);
	if (!list)
		return (NULL);
	populate_status = populate_command_list(list, env);
	if (populate_status != 0)
	{
		return (NULL);
	}
	return (list);
}

/*t_token	*last_node(t_token *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
	{
		lst = lst->next;
	}
	return (lst);
}

t_token *token_lst(t_token *token)
{
	static t_token *lst;

	if (lst)
		last_node(lst)->next = token;
	else 
		lst = token;*/
/*	static t_token *lst[4096];
	static int i = 0;
	
	lst[i++] = token;
	lst[i] = NULL;*/
//	return(lst);
//}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envar1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hde-barr <hde-barr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:54:30 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/15 17:12:14 by hde-barr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
#include "../../includes/minishell.h"
#include "minishell_part2.h"

char **get_path_list(char **env)
{
	char *path;
	char **path_list;

	path = get_envar(env,"PATH");
	path_list = ft_split(path, ':');
	return (path_list);
}

char **command_list_malloc(char **list, char **env)
{
	long long i;
	DIR *dir;
	char **path_list;
	path_list = get_path_list(env);
	i = 0;
	while (*path_list)
	{
		dir = opendir(*path_list++);
		if(dir == NULL)
			continue;
		while (readdir(dir))
		{
			++i;
		}
		closedir(dir);
	}
	list = hb_malloc((sizeof(char *) * i) + 1) ;
	return(list);
}

char **init_command_list(long long count, char **env)
{
	char **list;
	char **path_list;
	DIR *dir;
	struct dirent *d;
	
	path_list = get_path_list(env);
	list = command_list_malloc(list, env);
	count = -1;
	while (*path_list)
	{
		dir = opendir(*path_list++);
		if(dir == NULL)
			continue;
		d = readdir(dir);
		while (d)
		{
			list[++count] = ft_strdup(d->d_name);
			d = readdir(dir);
		}
		closedir(dir);
	}
	list[count] = NULL;
	return(list);
}

bool search_list(char *search, char **env)
{
	char **list;
	int i = 0;
	long long count;

	if(is_valid_exc(search))
		return (true);
	if (!search || !env)
		return (false);
	list = init_command_list(count, env);
	while (*(++list))
	{
		if(ft_strncmp(search, *list, 4096) == 0)
			return(true);
	}
	return(false);
}
*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envar2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hde-barr <hde-barr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:00:25 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/02 16:30:14 by hde-barr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "../../includes/minishell.h"
//#include "minishell_part2.h"

/*void print_env(char **env)
{
	int i;

	i = -1;
	if(!env)
	{
		ft_printf("env not setted");
		return ;
	}
	while (env[++i])
	{
		ft_printf("%s\n", env[i]);
	}
}
*/

//char *domane_expantion(char **env, char *input)
//{
//	/*if (ft_strncmp(ft_strtrim(input, " "),"env", ft_strlen(input) + 3) == 0)
//		print_env(env);*/
//	if(get_envar(env, input) /*&& *input == '$'*/)
//		return (get_envar(env, input));
//	return ("");
//}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envar0.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hde-barr <hde-barr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:46 by hde-barr          #+#    #+#             */
/*   Updated: 2025/04/02 16:31:01 by hde-barr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
//
//#include "../../includes/minishell.h"
//#include "minishell_part2.h"
//