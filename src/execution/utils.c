/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: averin <averin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:03:13 by averin            #+#    #+#             */
/*   Updated: 2024/01/23 12:12:46 by averin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution.h"


/**
 * @brief Initialize exec with default values
 * 
 * @param exec pointer to exec to init
 */
void	init_exec(t_exec *exec)
{
	exec->pathname = NULL;
	exec->infile = -1;
	exec->outfile = -1;
	exec->pipes[0] = -1;
	exec->pipes[1] = -1;
}

/**
 * Init the exec structure
 * @param exec structure to init
 * @param cmd cmd to init from
 * @param path the path
 * @return SUCCESS 
*/
int	fill_exec(t_exec *exec, t_cmd cmd, char **path)
{
	exec->args = cmd.args;
	if (exec->pathname)
		free(exec->pathname);
	if (ft_strchr(exec->args[0], '/'))
		exec->pathname = find_relative_exec(exec->args[0]);
	else
		exec->pathname = find_path_exec(exec->args[0], path);
	if (!exec->pathname)
		return (C_GEN);
	return (C_SUCCESS);
}

/**
 * Find an element and return his value
 * @param cmd cmd in wich search the element
 * @param type type of the element to search
 * @return element's value or NULL if not found
*/
void	*find_element(t_cmd cmd, t_type type)
{
	size_t	i;

	i = -1;
	while (cmd.elements[++i])
	{
		if (cmd.elements[i]->type == type)
			return (cmd.elements[i]->value);
	}
	return (NULL);
}
