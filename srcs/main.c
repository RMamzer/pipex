/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/15 17:42:24 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

	// should i check for envp ==NULL?


void error_exit(char *msg)
{
ft_putendl_fd(msg, STDERR_FILENO);
/*
NEED TO FREE
-t_data
- cmd_ags (ft_split)
*/
exit (EXIT_FAILURE);
}

void	check_access(char *file, t_data *data)
{
	if (access(file, F_OK | R_OK) == -1)
		error_exit ("Access fail ");
}



 void child_process_cmd1(t_data *data)
 {
	pid_t	pid_cmd1;
	int 	infile_fd;

	pid_cmd1 = fork();
	if (pid_cmd1 == -1)
	{
		// close fd's
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Fork error for cmd1");
	}
	if(pid_cmd1 == 0)
	{
		check_access(data->argv[1], data);
		infile_fd = open(data->argv[1], O_RDONLY);
		if (infile_fd < 0)
		// close fd's
			error_exit("Error opening infile");
		if (dup2(infile_fd, STDIN_FILENO) == -1)
		//close fd's
			error_exit(" 1st dup2 for cmd1 broke");
		close(infile_fd);
		if (dup2(data->pipefd[WRITE_END], STDOUT_FILENO) == -1)
		//close fd's
			error_exit("2nd dup2 for cmd1 broke");
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		data->cmd_args = ft_split(data->argv[2], " ");
		if (!data->cmd_args)
			// split failed
			error_exit("split returned NULL");
		data->cmd_path = check_path(data->argv[2]);

	}
 }

void	find_path(t_data *data, char **envp)
{
	if (!data->envp ||!*(data->envp))
		//is this correct
		error_exit("no path");
	while (())
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
	find_path(data, data->envp);
	if (pipe(data->pipefd) == -1)
		error_exit("./pipex: pipe broke");

	child_process_cmd1(data);
	return (0);
}





/*
Program structure:
- receive
*/

