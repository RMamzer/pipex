/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/24 13:26:08 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

 void	*free_arrays(char **splitted)
{
	size_t	i;

	i = 0;
	while (splitted[i])
		free(splitted[i++]);
	free(splitted);
	return (NULL);
}
void	error_manual(char *msg, t_data *data, int exit_code)
{
	ft_putstr_fd("pipex: ", STDERR_FILENO);
	ft_putendl_fd(msg, STDERR_FILENO);
		if (data->cmd_args)
			free_arrays(data->cmd_args);
	if (data->cmd_path)
			free_arrays(data->cmd_path);
	exit (exit_code);
}

void error_exit(char *msg, t_data *data, int exit_code)
{
	ft_putstr_fd("pipex: ", STDERR_FILENO);
	perror(msg);
	if (data->cmd_args)
			free_arrays(data->cmd_args);
	if (data->cmd_path)
			free_arrays(data->cmd_path);
	exit (exit_code);
}

void close_fds_and_exit(t_data *data, int opened_file, char *msg, int exit_code)
{
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	if (opened_file >= 0)
		close (opened_file);
	error_exit(msg, data, exit_code);
}


char	*path_strjoin(char const *s1, char const *s2)
{
	char	*joinedstr;
	size_t	str1_l;
	size_t	str2_l;

	str1_l = ft_strlen(s1);
	str2_l = ft_strlen(s2);
	if (!s1 && !s2)
		return (NULL);
	joinedstr = malloc(str1_l + str2_l + 2);
	if (!joinedstr)
		return (NULL);
	ft_memcpy(joinedstr, s1, str1_l);
	ft_memcpy(joinedstr + str1_l, "/", 1);
	ft_memcpy(joinedstr + str1_l + 1, s2, str2_l);
	joinedstr[str1_l + 1 + str2_l] = '\0';
	return (joinedstr);
}

// void	execute_cmd(t_data *data, char *path)
// {
// 	if (access(path, F_OK| X_OK) == 0)
// 		{
// 			execve(path, data->cmd_args, data->envp);
// 			free(path);
// 			data->exit_status = CMD_NOT_EXEC;
// 			return;
// 		}
// 	if (errno == EACCES)
// 		data->exit_status = CMD_NOT_EXEC; //<---exit 126
// 	else if (data->exit_status != CMD_NOT_EXEC)
// 		data->exit_status = CMD_NOT_FOUND;//<--- exit 127
// 	if (errno !=ENOENT)
// 		error_exit("", data, data->exit_status); //<----- what is this?
// }



void	execute_cmd(t_data *data, char *path)
{
	if (access(path, F_OK)== 0)
	{
		if (access (path, X_OK) == 0)
		{
			execve(path, data->cmd_args, data->envp);
			free(path);
			error_exit("Failed execution", data, CMD_NOT_EXEC);
		}
		free(path);
		error_exit("Permission denied:", data, CMD_NOT_EXEC);
	}
	free (path);
}

void	execute_path_cmd(t_data *data, char *path)
{
	if (access(path, F_OK) != 0)
		error_exit("No such file or directory", data, CMD_NOT_FOUND);
	if (access(path, X_OK) != 0)
		error_exit("Permission denied", data, CMD_NOT_EXEC);
	execve (path, data->cmd_args, data->envp);
	error_exit("Failed execution", data, CMD_NOT_EXEC);
}


void	check_cmd(t_data *data, char	**cmd_path)
{
	char	*joined_path;

	if (!data->cmd_args[0])
		error_manual("Command not found", data, CMD_NOT_FOUND);
	if (ft_strchr(data->cmd_args[0], '/'))
		execute_path_cmd(data, data->cmd_args[0]);
	else if (!cmd_path)
		error_manual("command not found", data, CMD_NOT_FOUND);
	else
	{
		while(*cmd_path)
			{
				joined_path = path_strjoin(*cmd_path, data->cmd_args[0]);
				if (!joined_path)
					error_manual("strjoin malloc failed", data, EXIT_FAILURE);
				execute_cmd(data, joined_path);
				cmd_path++;
			}
	}
	error_manual("command not found", data, CMD_NOT_FOUND);
}

void wait_children(pid_t pid1, pid_t pid2, t_data *data)
{
	int	status;
	pid_t	term_pid;
	int children_rem;

	children_rem = 2;
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	while (children_rem > 0)
	{
		term_pid = waitpid(-1, &status, 0);
		if (term_pid == -1)
			error_exit("waitpd failed", data, EXIT_FAILURE);
		if (term_pid == pid1 || term_pid == pid2)
		{
			children_rem--;
			if (term_pid == pid2 && (WIFEXITED(status)))
				data->exit_status = WEXITSTATUS(status);
		}
	}
	exit(data->exit_status);
}

void	find_path(t_data *data, char **envp)
{
	 	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			{
				data->cmd_path = ft_split(*envp + 5 , ':');
				if (!data->cmd_path)
					error_manual("failed to split the path to command", data, EXIT_FAILURE);
				return;
			}
		envp++;
	}
}

void	child_process_cmd1(t_data *data)
{
	int 	infile_fd;

	infile_fd = open(data->argv[1], O_RDONLY);
	if (infile_fd < 0)
		close_fds_and_exit (data, NO_OPENED_FILE, "Failed to open infile", EXIT_FAILURE);
	if (dup2(infile_fd, STDIN_FILENO) == -1)
		close_fds_and_exit(data, infile_fd, "Failed to dup2 infile->stdin", EXIT_FAILURE);
	if (dup2(data->pipefd[WRITE_END], STDOUT_FILENO) == -1)
		close_fds_and_exit(data, infile_fd,"Failed to dup2 stdout->pipe[write_end]", EXIT_FAILURE);
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	close(infile_fd);
	find_path(data, data->envp);
	if (!data->argv[2][0])
		error_manual("permission denied", data, CMD_NOT_EXEC);
	data->cmd_args = ft_split(data->argv[2], ' ');
	if (!data->cmd_args)
		error_manual("Failed malloc in splitting arguments", data, EXIT_FAILURE);
	check_cmd(data, data->cmd_path);
}

void	child_process_cmd2(t_data *data)
{
	int outfile_fd;

	outfile_fd = open (data->argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile_fd < 0)
		close_fds_and_exit (data, NO_OPENED_FILE, "Failed open the outfile", EXIT_FAILURE);
	if (dup2(outfile_fd, STDOUT_FILENO) == -1)
		close_fds_and_exit (data, outfile_fd, "Failed to dup2 outfile->stdout", EXIT_FAILURE);
	if (dup2(data->pipefd[READ_END], STDIN_FILENO) == -1)
		close_fds_and_exit (data, outfile_fd, "Failed to dup2 pipe[read_end]->stdin", EXIT_FAILURE);
	close(data->pipefd[WRITE_END]);
	close(data->pipefd[READ_END]);
	close(outfile_fd);
	find_path(data, data->envp);
	if (!data->argv[3][0])
		error_manual("permission denied", data, CMD_NOT_EXEC);
	data->cmd_args = ft_split(data->argv[3], ' ');
	if (!data->cmd_args)
		error_manual("Failed malloc in splitting arguments", data, EXIT_FAILURE);
	check_cmd(data,data->cmd_path);
}


void	init_parent(t_data *data, char **argv, char **envp)
{
	data->argv = argv;
	data->envp = envp;
	data->cmd_args = NULL;
	data->cmd_path = NULL;
	data->exit_status = EXIT_FAILURE;
	if (pipe(data->pipefd) == -1)
		error_exit("Failed pipe", data, EXIT_FAILURE);
}


int	main(int argc, char **argv, char **envp)
{
	t_data data;
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
		close_fds_and_exit(&data, NO_OPENED_FILE, "Failed fork for cmd1", EXIT_FAILURE);
	if (pid_cmd1 == 0)
		child_process_cmd1(&data);
	pid_cmd2 = fork();
	if (pid_cmd2 == -1)
		close_fds_and_exit(&data, NO_OPENED_FILE, "Failed fork for cmd2", EXIT_FAILURE);
	if (pid_cmd2 == 0)
		child_process_cmd2(&data);
	wait_children(pid_cmd1, pid_cmd2, &data);
}

/*
1. close fd_s
2.
*/