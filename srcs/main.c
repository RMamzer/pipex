/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/18 17:39:58 by rmamzer          ###   ########.fr       */
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
		if(data->cmd_path)
			free_arrays(data->cmd_path);
		if (data->cmd_args)
			free_arrays(data->cmd_args);
		free(data);
	}
}
void error_exit(char *msg, t_data *data, bool child)
{
	perror(msg);
	if (child == false)
		free_struct(data);
	else
	{
		if (data->cmd_args)
			free_arrays(data->cmd_args);
	}

	exit (EXIT_FAILURE);
}


// void	check_access(char *file, t_data *data)
// {
// 	if (access(file, F_OK | R_OK) == -1)
// 		error_exit ("Access fail", data). false;
// }


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
		error_exit("strjoin malloc broke", data, true);
	if (access(full_path, F_OK | X_OK)== 0)
	{
		if( execve(full_path, data->cmd_args, data->envp) == -1)
		{
			free(full_path);
			error_exit("Cmd1 execution failed", data, true);
		}
	}
	free(full_path);
}

// what uf no envp/cmd_path variables at all?
void	process_execution_of_cmd(t_data *data, char *cmd, char	**cmd_path)
{
	while(*cmd_path)
	{
		execute_cmd(data, *cmd_path);
		cmd_path++;
	}
		error_exit("Cmd was not found", data, true); //<-- need exit (127) num
}

void wait_cmd(pid_t pid1, pid_t pid2, t_data *data)
{
	int status1;
	int status2;

	waitpid(pid1, &status1, 0);

	waitpid(pid2, &status2, 0);

	free_struct(data);
	if (WIFEXITED(status2))
		exit(WEXITSTATUS(status2));
	else
		exit(EXIT_FAILURE);
}

void	child_process_cmd1(t_data *data)
{
	int 	infile_fd;

	infile_fd = open(data->argv[1], O_RDONLY);
	if (infile_fd < 0)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Failed open the infile", data, true);
	}
	if (dup2(infile_fd, STDIN_FILENO) == -1)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		close (infile_fd);
		error_exit("Failed dup2 stdin cmd1", data, true);
	}
	close(infile_fd); // <------would it be better to close fd together later? Setup fds as -1 first?
	if (dup2(data->pipefd[WRITE_END], STDOUT_FILENO) == -1)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Failed dup2 stdout cmd1", data, true);
	}
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	data->cmd_args = ft_split(data->argv[2], ' '); // <-----will need to free it
	if (!data->cmd_args)
		error_exit("Failed malloc in args ft_split", data, true);
	process_execution_of_cmd(data, data->argv[2], data->cmd_path);
}
void	child_process_cmd2(t_data *data)
{
	int outfile_fd;

	outfile_fd = open (data->argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile_fd < 0)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Failed open the outfile", data, true);
	}
	if (dup2(outfile_fd, STDOUT_FILENO) == -1)
	{
		close (outfile_fd);
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Failed dup2 outfile fd to stdout", data, true);
	}
	close(outfile_fd);
	if (dup2(data->pipefd[READ_END], STDIN_FILENO) == -1)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Failed dup2 read_end to stdin", data, true);
	}
	close(data->pipefd[WRITE_END]);
	close(data->pipefd[READ_END]);
	data->cmd_args = ft_split(data->argv[3], ' ');
	if (!data->cmd_args)
		error_exit("failed malloc in args ft_split cmd2", data, true);
	process_execution_of_cmd(data, data->argv[3], data->cmd_path);
}
void	find_path(t_data *data, char **envp)
{
 	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			{
				data->cmd_path = ft_split(*envp + 5 , ':');
				if (!data->cmd_path)
					error_exit("envp split broke", data, false);
				return;
			}
		envp++;
	}
}


int	main(int argc, char **argv, char **envp)
{
	t_data *data;
	pid_t	pid_cmd1;
	pid_t	pid_cmd2;

	if (argc != 5)
		error_exit("Usage: ./pipex file1 cmd1 cmd2 file2", NULL, false);
	data = calloc(1, sizeof(t_data));
	if (!data)
		error_exit("Calloc broke", data, false);
	data->argv = argv;
	data->envp = envp;
	find_path(data, envp);
	if (pipe(data->pipefd) == -1)
		error_exit("./pipex: pipe broke", data, false);
	pid_cmd1 = fork();
	if (pid_cmd1 == -1)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Fork error for cmd1", data, false);
	}
	if (pid_cmd1 == 0)
		child_process_cmd1(data);
	pid_cmd2 = fork();
	if (pid_cmd2 == -1)
	{
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		error_exit("Fork error for cmd1", data, false);
	}
	if (pid_cmd2 == 0)
		child_process_cmd2(data);
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	wait_cmd(pid_cmd1, pid_cmd2, data);
}
