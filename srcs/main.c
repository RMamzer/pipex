/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/15 15:00:49 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void error_exit(char *msg)
{
ft_putendl_fd(msg, STDERR_FILENO);
exit (EXIT_FAILURE);
}


 void child_process_cmd1(t_data *data)
 {
	pid_t	pid_cmd1;
	int 	infile_fd;

	pid_cmd1 = fork();
	if (pid_cmd1 == -1)
	{
		close(data->pipefd[0]);
		close(data->pipefd[1]);
		error_exit("Fork error for cmd1");
	}
	if(pid_cmd1 == 0)
	{
		infile_fd = open(data->argv[1], O_RDONLY);
		if (infile_fd < 0)
			error_exit("Error opening infile");
	}

 }



int main (int argc, char **argv, char **envp)
{

	t_data *data;
	if (argc != 5)
		error_exit("Usage: ./pipex file1 cmd1 cmd2 file2");



	data = calloc(1, sizeof(data));
	if (!data)
		error_exit("Calloc broke");
	data->argv = argv;
	data->envp = envp;
	if (pipe(data->pipefd) == -1)
		error_exit("./pipex: pipe broke");

	child_process_cmd1(data);
	return (0);
}





/*
Program structure:
- receive
*/

