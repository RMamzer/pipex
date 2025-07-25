/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/25 14:33:06 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	child_process_cmd1(t_data *data)
{
	int	infile_fd;

	infile_fd = open(data->argv[1], O_RDONLY);
	if (infile_fd < 0)
		close_fds_and_exit (data, NO_OPENED_FILE,
			"Failed to open infile", EXIT_FAILURE);
	if (dup2(data->pipefd[WRITE_END], STDOUT_FILENO) == -1)
		close_fds_and_exit(data, infile_fd,
			"Failed to dup2 stdout->pipe[write_end]", EXIT_FAILURE);
	if (dup2(infile_fd, STDIN_FILENO) == -1)
		close_fds_and_exit(data, infile_fd,
			"Failed to dup2 infile->stdin", EXIT_FAILURE);
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	close(infile_fd);
	find_path(data, data->envp);
	check_cmd(data, data->cmd_path, data->argv[2]);
}

void	child_process_cmd2(t_data *data)
{
	int	outfile_fd;

	outfile_fd = open (data->argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile_fd < 0)
		close_fds_and_exit (data, NO_OPENED_FILE,
			"Failed open the outfile", EXIT_FAILURE);
	if (dup2(outfile_fd, STDOUT_FILENO) == -1)
		close_fds_and_exit (data, outfile_fd,
			"Failed to dup2 outfile->stdout", EXIT_FAILURE);
	if (dup2(data->pipefd[READ_END], STDIN_FILENO) == -1)
		close_fds_and_exit (data, outfile_fd,
			"Failed to dup2 pipe[read_end]->stdin", EXIT_FAILURE);
	close(data->pipefd[WRITE_END]);
	close(data->pipefd[READ_END]);
	close(outfile_fd);
	find_path(data, data->envp);
	check_cmd(data, data->cmd_path, data->argv[3]);
}

void	wait_children(pid_t pid1, pid_t pid2, t_data *data)
{
	int		status;
	pid_t	term_pid;
	int		children_rem;
	int		exit_status;

	children_rem = 2;
	exit_status = EXIT_FAILURE;
	while (children_rem > 0)
	{
		term_pid = waitpid(-1, &status, 0);
		if (term_pid == -1)
			error_exit("waitpd failed", data, EXIT_FAILURE);
		if (term_pid == pid1 || term_pid == pid2)
		{
			children_rem--;
			if (term_pid == pid2 && (WIFEXITED(status)))
				exit_status = WEXITSTATUS(status);
		}
	}
	exit(exit_status);
}

void	init_parent(t_data *data, char **argv, char **envp)
{
	data->argv = argv;
	data->envp = envp;
	data->cmd_args = NULL;
	data->cmd_path = NULL;
	if (pipe(data->pipefd) == -1)
		error_exit("Failed pipe", data, EXIT_FAILURE);
}

int	main(int argc, char **argv, char **envp)
{
	t_data	data;
	pid_t	pid_cmd1;
	pid_t	pid_cmd2;

	if (argc != 5)
	{
		ft_putendl_fd("Usage: ./pipex file1 cmd1 cmd2 file2", STDERR_FILENO);
		exit (EXIT_FAILURE);
	}
	init_parent(&data, argv, envp);
	pid_cmd1 = fork();
	if (pid_cmd1 == -1)
		close_fds_and_exit(&data, NO_OPENED_FILE,
			"Failed fork for cmd1", EXIT_FAILURE);
	if (pid_cmd1 == 0)
		child_process_cmd1(&data);
	pid_cmd2 = fork();
	if (pid_cmd2 == -1)
		close_fds_and_exit(&data, NO_OPENED_FILE,
			"Failed fork for cmd2", EXIT_FAILURE);
	if (pid_cmd2 == 0)
		child_process_cmd2(&data);
	close(data.pipefd[READ_END]);
	close(data.pipefd[WRITE_END]);
	wait_children(pid_cmd1, pid_cmd2, &data);
}
