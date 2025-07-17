/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/17 17:51:15 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

	// should i check for envp ==NULL?



void print_envp(char **envp)
{
    int i = 0;

    if (envp == NULL)
    {
        ft_printf("Error: envp array is NULL.\n");
        return;
    }

    printf("--- Environment Variables ---\n");
    while (envp[i] != NULL) // Loop until the NULL terminator is found
    {
        ft_printf("%s\n", envp[i]); // Print the current environment string
        i++; // Move to the next string
    }
    ft_printf("-----------------------------\n");
}


 void	*free_arrays(char **splitted)
{
	size_t	i;

	i = 0;
	while (splitted[i])
		free(splitted[i++]);
	free(splitted);
	return (NULL);
}
void	free_struct(t_data *data)
{
	if(data)
	{
	//	ft_printf("Im here :)");
		if (data->cmd_args)
			free_arrays(data->cmd_args);
		if(data->cmd_path)
			free_arrays(data->cmd_path);
		free(data);
	}
}
void error_exit(char *msg, t_data *data)
{
	ft_putendl_fd(msg, STDERR_FILENO);
	perror(msg);
	free_struct(data);
	exit (EXIT_FAILURE);
}


void	check_access(char *file, t_data *data)
{
	if (access(file, F_OK | R_OK) == -1)
		error_exit ("Access fail", data);
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

void	execute_cmd(t_data *data, char *path)
{
	char	*full_path;
	int	check;

	full_path = path_strjoin(path, data->cmd_args[0]);
	if (!full_path)
		error_exit("strjoin malloc broke", data);
	if (access(full_path, F_OK | X_OK))
	{
		execve(full_path, data->cmd_args, data->envp);
		data->status = -1;
	}
	free(full_path);
}

// what uf no envp/cmd_path variables at all?
void	process_execution_of_cmd(t_data *data, char *cmd, char	**cmd_path)
{
	while(*cmd_path)
	{
		execute_cmd(data, *cmd_path);
		if (data->status == -1 || data->status == 1)
			break;
		cmd_path++;
	}
	if (data->status == -1)
		error_exit("function didn't work", data);
	if (data->status == 0)
		error_exit("function was not found", data);
}

void wait_cmd(pid_t pid)
{
	int status;

	waitpid(pid, &status, 0);
}
void	child_process_cmd1(t_data *data)
{
	pid_t	pid_cmd1;
	int 	infile_fd;

	pid_cmd1 = fork();
	if (pid_cmd1 == -1)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Fork error for cmd1", data);
	}
	if(pid_cmd1 == 0)
	{
		infile_fd = open(data->argv[1], O_RDONLY);
		if (infile_fd < 0)
		{
			close(data->pipefd[READ_END]);
			close(data->pipefd[WRITE_END]);
			error_exit("Error opening infile", data);
		}
		if (dup2(infile_fd, STDIN_FILENO) == -1)
		{
			close(data->pipefd[READ_END]);
			close(data->pipefd[WRITE_END]);
			close (infile_fd);
			error_exit(" 1st dup2 for cmd1 broke", data);
		}
		close(infile_fd); // <------would it be better to close fd together later? Setup fds as -1 first?
		if (dup2(data->pipefd[WRITE_END], STDOUT_FILENO) == -1)
		{
			close(data->pipefd[READ_END]);
			close(data->pipefd[WRITE_END]);
			error_exit("2nd dup2 for cmd1 broke", data);
		}
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		data->cmd_args = ft_split(data->argv[2], ' ');
		if (!data->cmd_args)
			error_exit("split returned NULL", data);
		process_execution_of_cmd(data, data->argv[2], data->cmd_path);
	}
	 wait_cmd(pid_cmd1);
}

void	find_path(t_data *data, char **envp)
{
 	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			{
				data->cmd_path = ft_split(*envp + 5 , ':');
				if (!data->cmd_path)
					error_exit("envp split broke", data);
				return;
			}
		envp++;
	}
}


int	main(int argc, char **argv, char **envp)
{
	t_data *data;

	if (argc != 5)
		error_exit("Usage: ./pipex file1 cmd1 cmd2 file2", NULL);
	data = calloc(1, sizeof(t_data));
	if (!data)
		error_exit("Calloc broke", data);
	data->argv = argv;
	data->envp = envp;
	find_path(data, envp);
	if (pipe(data->pipefd) == -1)
		error_exit("./pipex: pipe broke", data);
	child_process_cmd1(data);
	free_struct (data);// final clean
}
