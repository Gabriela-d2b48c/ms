/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ex_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabriela <gabriela@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 09:13:58 by gabriela          #+#    #+#             */
/*   Updated: 2023/10/04 13:49:33 by gabriela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"



void	ft_child_process(int prev_fd, t_cmd *current, t_ms *ms)
{
	int	in_fd;
	int	out_fd;

	in_fd = -1;
	out_fd = -1;
	ft_setup_input_redir(prev_fd, current, &in_fd);
	ft_setup_output_redir(current, ms, &out_fd);
	if (ft_check_builtin(current->cmd))
		ft_execute_forked_builtin(current, ms);
	if (execve(current->cmd, current->arg, environ) == -1)
		ft_command_not_found(current);
	exit(0);
}

/* Close read end of previous pipe. If there is next cmd,
store read end of current pipe in prev_fd. If there is no
next command, close write end of current pipe.
Wait for child process to finish. */
void	ft_parent_process(int *prev_fd, int pid, t_ms *ms, t_cmd *current)
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (current->next)
		*prev_fd = ms->pipe_fd[0];
	close(ms->pipe_fd[1]);
	waitpid(pid, &ms->exit_status, 0);
}

/* Check if there is a pipe with previous command. */
int	ft_check_if_piped(t_cmd *current, int prev_fd)
{
	int	is_piped;

	if ((current->next != NULL) || (prev_fd != -1))
		is_piped = 1;
	else
		is_piped = 0;
	return (is_piped);
}

void	ft_execute_cmd(t_ms *ms, t_cmd *current)
{
	int	prev_fd;
	int	pid;

	prev_fd = -1;
	while (current)
	{
		if (ft_handle_heredoc_builtin(ms, current, &prev_fd))
		{
			current = current->next;
			continue ;
		}
		if (current->next && pipe(ms->pipe_fd) == -1)
			ft_error();
		pid = fork();
		if (pid == -1)
			ft_error();
		if (pid == 0)
			ft_child_process(prev_fd, current, ms);
		else
			ft_parent_process(&prev_fd, pid, ms, current);
		current = current->next;
	}
}
