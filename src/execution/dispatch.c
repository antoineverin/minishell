/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: averin <averin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 10:41:06 by averin            #+#    #+#             */
/*   Updated: 2024/02/02 13:08:38 by averin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution.h"

/**
 * Open outfile and set corresponding fd in exec
 * @param cmd Command in wich search infile
 * @param exec Where to store infile fd
 * @return `C_SUCCESS` or `C_GEN` when error
*/
static int	init_outfile(t_cmd *cmd, t_exec *exec)
{
	t_outfile	*element;

	element = (t_outfile *) find_element(*cmd, T_OUTFILE);
	if (element != NULL)
	{
		if (element->outtype == OT_TRUNCATE)
			exec->outfile = open(element->filename,
					O_WRONLY | O_TRUNC | O_CREAT, 0644);
		else if (element->outtype == OT_APPEND)
			exec->outfile = open(element->filename,
					O_WRONLY | O_APPEND | O_CREAT, 0644);
		if (exec->outfile == -1)
			return (perror(element->filename), C_GEN);
	}
	return (C_SUCCESS);
}

/**
 * Open or create infile and set corresponding fd in exec
 * @param cmd Command in wich search infile
 * @param exec Where to store infile fd
 * @return `C_SUCCESS` or `C_GEN` when error
*/
static int	init_infile(t_cmd *cmd, t_exec *exec)
{
	t_infile	*element;

	element = (t_infile *) find_element(*cmd, T_INFILE);
	if (element != NULL)
	{
		if (element->intype == IT_INFILE)
			exec->infile = open(element->filename, O_RDONLY);
		else if (element->intype == IT_HERE_DOC)
			exec->infile = here_doc(element->filename);
		if (exec->infile == -1)
			return (perror(element->filename), C_GEN);
		else if (exec->infile == -2)
			return (C_BAD_USE);
	}
	return (C_SUCCESS);
}

/**
 * If the command have a pipe create it and store it in exec and close if
 * another pipe is already open
 * @param cmd Where search for pipe
 * @param exec Where store thie pipe
 * @return `C_SUCCESS` or `C_GEN` when error
*/
static int	init_pipe(t_cmd *cmd, t_exec *exec)
{
	if (find_element(*cmd, T_PIPE))
	{
		if (pipe(exec->pipes) == -1)
			return (perror("pipe"), C_GEN);
		exec->outfile = exec->pipes[1];
	}
	return (C_SUCCESS);
}

/**
 * @brief Call redirections function and manage errors
 *
 * @param cmd command to execute
 * @param exec current execution
 * @return int `C_SUCCESS` or an exit code
 */
static int	prepare_exec(t_cmd *cmd, t_exec *exec, char **path)
{
	int	err;

	if (init_pipe(cmd, exec) == C_GEN)
		return (124);
	err = fill_exec(exec, *cmd, path);
	if (err != C_SUCCESS)
		return (err);
	err = init_infile(cmd, exec);
	if (err == C_BAD_USE)
		return (130);
	else if (err == C_GEN || init_outfile(cmd, exec) == C_GEN)
		return (125);
	return (C_SUCCESS);
}

/**
 * Execute a command
 * @param data data of the program
 * @return exit code
*/
int	dispatch_cmd(t_data *data)
{
	t_exec	exec;
	int		pid;
	int		err;

	pid = -1;
	init_exec(&exec, data);
	while (exec.target)
	{
		err = prepare_exec(exec.target, &exec, data->path);
		if (err != C_SUCCESS)
			return (free(exec.pathname), err);
		if (exec.is_builtin)
			pid = exec_builtin(&exec);
		else
			do_exec(&exec, data->envp, &pid);
		exec.target = find_element(*(exec.target), T_PIPE);
		if (exec.target)
			exec.infile = exec.pipes[0];
	}
	free(exec.pathname);
	if (exec.is_builtin)
		return (pid);
	return (wait_children(pid));
}
