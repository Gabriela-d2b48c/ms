/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ex_redir_io.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gabriela <gabriela@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 13:48:02 by gabriela          #+#    #+#             */
/*   Updated: 2023/10/04 13:55:23 by gabriela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_setup_input_redir(int prev_fd, t_cmd *current, int *in_fd)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (current->input_redir)
	{
		*in_fd = open(current->input_redir, O_RDONLY | O_CLOEXEC, 0777);
		dup2(*in_fd, STDIN_FILENO);
		close(*in_fd);
	}
}

void	ft_setup_output_redir(t_cmd *current, t_ms *ms, int *out_fd)
{
	if (current->next)
	{
		dup2(ms->pipe_fd[1], STDOUT_FILENO);
		close(ms->pipe_fd[1]);
	}
	else if (current->output_redir)
	{
		*out_fd = open(current->output_redir, O_WRONLY | O_CREAT
				| O_TRUNC | O_CLOEXEC, 0777);
		dup2(*out_fd, STDOUT_FILENO);
		close(*out_fd);
	}
	else if (current->append_output_redir)
	{
		*out_fd = open(current->append_output_redir, O_WRONLY | O_CREAT
				| O_APPEND | O_CLOEXEC, 0777);
		dup2(*out_fd, STDOUT_FILENO);
		close(*out_fd);
	}
}

int	ft_handle_heredoc_builtin(t_ms *ms, t_cmd *current, int *prev_fd)
{
	int	heredoc_fd[2];

	if (current->here_doc)
	{
		ft_heredoc(current->here_doc, heredoc_fd);
		*prev_fd = heredoc_fd[0];
	}
	if (!ft_check_if_piped(current, *prev_fd) && ft_check_builtin(current->cmd))
	{
		ft_execute_builtin_directly(current, ms);
		return (1);
	}
	return (0);
}

void	ft_heredoc(char *delimiter, int heredoc_fd[2])
{
	char	*line;

	pipe(heredoc_fd);
	while (1)
	{
		write(1, "heredoc> ", 9);
		line = readline(NULL);
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(heredoc_fd[1], line, strlen(line));
		write(heredoc_fd[1], "\n", 1);
		free(line);
	}
	close(heredoc_fd[1]);
}
