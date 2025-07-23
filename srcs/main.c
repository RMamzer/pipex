/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/23 12:14:31 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"


// handle -t ?????????????????????????????? YESSSSS

 void	*free_arrays(char **splitted)
{
	size_t	i;

	i = 0;
	while (splitted[i])
		free(splitted[i++]);
	free(splitted);
	return (NULL);
}


void error_exit(char *msg, t_data *data, int exit_code)
{
	perror(msg);
	if (data->cmd_args)
			free_arrays(data->cmd_args);
	if (data->cmd_path)
			free_arrays(data->cmd_path);
	exit (exit_code);
}

void close_fds_and_exit(t_data *data, int opened_file, char *msg, int code)
{
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	if (opened_file >= 0)
		close (opened_file);
	error_exit(msg, data, code);
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

void	execute_cmd(t_data *data, char *path, bool path_in_args)
{
	if (access(path, F_OK| X_OK) == 0)
		{
			if(execve(path, data->cmd_args, data->envp) == -1)
			{
				if (path_in_args == NO_PATH_IN_ARGS)
					free(path);
				if (errno == ENOEXEC)
					data->exit_status = CMD_NOT_EXEC;
				else
					data->exit_status = EXIT_FAILURE;
				return; 
			}
		}
	if (errno == EACCES)
		data->exit_status = CMD_NOT_EXEC; //<---exit 126
	else if (data->exit_status != 126 || data->exit_status != EXIT_FAILURE)
		data->exit_status = CMD_NOT_FOUND; //<--- exit 127
}

// what uf no envp/cmd_path variables at all?
void	check_cmd(t_data *data, char	**cmd_path)
{
	char	*joined_path;

	if (!data->cmd_args[0])
		error_exit("Command not found", data, CMD_NOT_FOUND);
	if (ft_strchr(data->cmd_args[0], '/'))
		execute_cmd(data, data->cmd_args[0], PATH_IN_ARGS);
	else
	{
		while(*cmd_path)
			{
				joined_path = path_strjoin(*cmd_path, data->cmd_args[0]);
				if (!joined_path)
					error_exit("strjoin malloc failed", data, EXIT_FAILURE);
				execute_cmd(data, joined_path, NO_PATH_IN_ARGS);
				free (joined_path);
				cmd_path++;
			}
	}
	error_exit("Cmd was not found", data, data->exit_status);
}

void wait_children(pid_t pid1, pid_t pid2, t_data *data)
{
	int status1;
	int status2;

	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	if (WIFEXITED(status2))
		exit(WEXITSTATUS(status2));
	else
		exit(EXIT_FAILURE);
}

void	find_path(t_data *data, char **envp)
{
	 	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			{
				data->cmd_path = ft_split(*envp + 5 , ':');
				if (!data->cmd_path)
					error_exit("failed to split the path to command", data, EXIT_FAILURE);
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
	data->cmd_args = ft_split(data->argv[2], ' ');
	if (!data->cmd_args)
		error_exit("Failed malloc in splitting arguments", data, EXIT_FAILURE);
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
	data->cmd_args = ft_split(data->argv[3], ' ');
	if (!data->cmd_args)
		error_exit("Failed malloc in splitting arguments", data, EXIT_FAILURE);
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
		error_exit("./pipex: pipe broke", data, EXIT_FAILURE);
}


int	main(int argc, char **argv, char **envp)
{
	t_data data;
	pid_t	pid_cmd1;
	pid_t	pid_cmd2;

	if (argc != 5)
	{
		ft_printf("Usage: ./pipex file1 cmd1 cmd2 file2");
		exit (EXIT_FAILURE);
	}
	init_parent(&data, argv, envp);
	pid_cmd1 = fork();
	if (pid_cmd1 == -1)
		close_fds_and_exit(&data,NO_OPENED_FILE, "Fork error for cmd1", EXIT_FAILURE);
	if (pid_cmd1 == 0)
		child_process_cmd1(&data);
	pid_cmd2 = fork();
	if (pid_cmd2 == -1)
		close_fds_and_exit(&data, NO_OPENED_FILE, "Fork error for cmd2", EXIT_FAILURE);
	if (pid_cmd2 == 0)
		child_process_cmd2(&data);
	wait_children(pid_cmd1, pid_cmd2, &data);
}

