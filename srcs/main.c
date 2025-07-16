/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:21 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/16 12:47:50 by rmamzer          ###   ########.fr       */
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

void error_exit(char *msg, t_data *data)
{
	ft_putendl_fd(msg, STDERR_FILENO);
	/*
	NEED TO FREE
	-t_data
	- cmd_ags (ft_split)
	- cmd_path (ft_split)
	*/
	if(data)
	{
		if (data->cmd_args)
			free_arrays(data->cmd_args);
		if(data->cmd_path)
			free_arrays(data->cmd_path);
		free(data);
	}
	exit (EXIT_FAILURE);
}

void	check_access(char *file, t_data *data)
{
	if (access(file, F_OK | R_OK) == -1)
		error_exit ("Access fail", data);
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
		error_exit("Fork error for cmd1", data);
	}
	if(pid_cmd1 == 0)
	{
		check_access(data->argv[1], data);
		infile_fd = open(data->argv[1], O_RDONLY);
		if (infile_fd < 0)
		// close fd's
			error_exit("Error opening infile", data);
		if (dup2(infile_fd, STDIN_FILENO) == -1)
		//close fd's
			error_exit(" 1st dup2 for cmd1 broke", data);
		close(infile_fd);
		if (dup2(data->pipefd[WRITE_END], STDOUT_FILENO) == -1)
		//close fd's
			error_exit("2nd dup2 for cmd1 broke", data);
		close(data->pipefd[READ_END]);
		close(data->pipefd[WRITE_END]);
		data->cmd_args = ft_split(data->argv[2], ' ');
		if (!data->cmd_args)
			// split failed
			error_exit("split returned NULL", data);

	}
 }

void	find_path(t_data *data, char **envp)
{
 	if (!envp ||!*envp)
 		//is this correct
 		error_exit("no path", data);
 	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			{
				data->envp = ft_split(*envp, ':');
				if (!data->envp)
					error_exit("envp split broke", data);
				return;
			}
		envp++;
	}
}


int main (int argc, char **argv, char **envp)
{

	t_data *data;
	if (argc != 5)
		error_exit("Usage: ./pipex file1 cmd1 cmd2 file2", NULL);



	data = calloc(1, sizeof(data));
	if (!data)
		error_exit("Calloc broke", data);
	data->argv = argv;
	find_path(data, envp);
	if (pipe(data->pipefd) == -1)
		error_exit("./pipex: pipe broke", data);
	// print_envp(envp);
	// print_envp(data->envp);
	 child_process_cmd1(data);
	return (0);
}





/*
Program structure:
- receive
*/

