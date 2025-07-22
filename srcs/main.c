/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/22 16:17:19 by rmamzer          ###   ########.fr       */
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


void error_exit(char *msg, t_data *data)
{
	perror(msg);
	if (data->cmd_args)
			free_arrays(data->cmd_args);
	if (data->cmd_path)
			free_arrays(data->cmd_path);
	exit (data->exit_status);
}

void close_fds_and_exit(t_data *data, int opened_file, char *msg)
{
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	if (opened_file >= 0)
		close (opened_file);
	error_exit(msg, data);
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
	if (access(path, X_OK)== 0)
		{
			if(execve(path, data->cmd_args, data->envp) == -1)
			{
				if (path_in_args == NO_PATH_IN_ARGS)
					free(path);
				data->exit_status = 126;
				error_exit("Cmd1 execution failed", data);
			}
		}
}


// what uf no envp/cmd_path variables at all?
void	check_cmd(t_data *data, char	**cmd_path)
{
	char	*joined_path;

	if (!data->cmd_args[0])
	{
		data->exit_status = 127;
		error_exit("Cmd NULL:", data);
	}  //<------ i need different exit message? No perror
	if (ft_strchr(data->cmd_args[0], '/'))
		execute_cmd(data, data->cmd_args[0], PATH_IN_ARGS);
	else
	{
	while(*cmd_path)
		{
			joined_path = path_strjoin(*cmd_path, data->cmd_args[0]);
			if (!joined_path)
				error_exit("strjoin malloc broke", data);
			execute_cmd(data, joined_path, NO_PATH_IN_ARGS);
			free (joined_path);
			cmd_path++;
		}
	}
	data->exit_status = 127;
	error_exit("Cmd was not found", data);
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

void	find_path(t_data *data,char **envp)
{
	 	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			{
				data->cmd_path = ft_split(*envp + 5 , ':');
				if (!data->cmd_path)
					error_exit("path split broke", data);
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
		close_fds_and_exit (data, NO_OPENED_FILE, "Failed open the infile");
	if (dup2(infile_fd, STDIN_FILENO) == -1)
		close_fds_and_exit(data, infile_fd, "Failed dup2 stdin cmd1");
	if (dup2(data->pipefd[WRITE_END], STDOUT_FILENO) == -1)
		close_fds_and_exit(data, infile_fd,"Failed dup2 stdout cmd1");
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	close(infile_fd);
	find_path(data, data->envp);
	data->cmd_args = ft_split(data->argv[2], ' ');
	if (!data->cmd_args)
		error_exit("Failed malloc in args ft_split", data);
	check_cmd(data, data->cmd_path);
}

void	child_process_cmd2(t_data *data)
{
	int outfile_fd;

	outfile_fd = open (data->argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile_fd < 0)
		close_fds_and_exit (data, NO_OPENED_FILE, "Failed open the outfile");
	if (dup2(outfile_fd, STDOUT_FILENO) == -1)
		close_fds_and_exit (data, outfile_fd, "Failed dup2 outfile fd to stdout");
	if (dup2(data->pipefd[READ_END], STDIN_FILENO) == -1)
		close_fds_and_exit (data, outfile_fd, "Failed dup2 read_end to stdin");
	close(data->pipefd[WRITE_END]);
	close(data->pipefd[READ_END]);
	close(outfile_fd);
	find_path(data, data->envp);
	data->cmd_args = ft_split(data->argv[3], ' ');
	if (!data->cmd_args)
		error_exit("failed malloc in args ft_split cmd2", data);
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
		error_exit("./pipex: pipe broke", data);
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
		close_fds_and_exit(&data,NO_OPENED_FILE, "Fork error for cmd1");
	if (pid_cmd1 == 0)
		child_process_cmd1(&data);
	pid_cmd2 = fork();
	if (pid_cmd2 == -1)
		close_fds_and_exit(&data, NO_OPENED_FILE, "Fork error for cmd2");
	if (pid_cmd2 == 0)
		child_process_cmd2(&data);
	wait_children(pid_cmd1, pid_cmd2, &data);
}

