/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/14 16:47:10 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void error_exit(char *msg)
{
ft_putendl_fd(msg, STDERR_FILENO);

exit (EXIT_FAILURE);
}



int main (int argc, char **argv, char **envp)
{
	int		pipefd[2];
	pid_t	pid_cmd1;
	pid_t	pid_cmd2;

	if (argc != 5)
		error_exit("Usage: ./pipex file1 cmd1 cmd2 file2");

	if (pipe(pipefd) == -1)
		error_exit("./pipex: pipe broke");

		pid_cmd1 = fork();

	if (pid_cmd1 == -1)
		error_exit("./pipex: fork 1 failed")

	return (0);
}





/*
Program structure:
- receive
*/
