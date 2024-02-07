/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abasdere <abasdere@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 09:38:04 by averin            #+#    #+#             */
/*   Updated: 2024/02/07 14:41:03 by abasdere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"

int	cmd_unset(t_exec *exec)
{
	size_t	i;
	char	*el;

	i = 0;
	if (exec->is_pipe || exec->args[0] == NULL || exec->args[1] == NULL)
		return (C_SUCCESS);
	while (exec->args[++i])
	{
		el = ft_getenv(*(exec->data), exec->args[i]);
		if (!el)
			continue ;
		if (ft_unenv(exec->data, exec->args[i]))
			return (free(el), C_MEM);
		free(el);
		if (!ft_strncmp(exec->args[i], "PATH\0", ft_strlen(exec->args[i]) + 1))
			if (get_path(exec->data))
				return (C_MEM);
	}
	return (C_SUCCESS);
}
