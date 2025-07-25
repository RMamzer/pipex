/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:57 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/25 14:26:22 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "libft.h"

# include <unistd.h> // For fork, pipe, dup2, execve, access, close, read, write
# include <sys/wait.h> // For wait, waitpid
# include <fcntl.h> // For open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
# include <stdlib.h> // For exit, malloc, free
# include <stdio.h> // For perror
# include <stdbool.h> //do i need it?
# include <errno.h> // do i need it?

# define READ_END 0
# define WRITE_END 1

# define NO_OPENED_FILE -1

# define CMD_NOT_FOUND 127
# define CMD_NOT_EXEC 126

typedef struct s_data
{
	int		pipefd[2];
	char	**argv;
	char	**envp;
	char	**cmd_args;
	char	**cmd_path;
}	t_data;

//main
int		main(int argc, char **argv, char **envp);
void	init_parent(t_data *data, char **argv, char **envp);
void	wait_children(pid_t pid1, pid_t pid2, t_data *data);
void	child_process_cmd1(t_data *data);
void	child_process_cmd2(t_data *data);

//cmd_execution
void	execute_cmd(t_data *data, char *path);
void	execute_path_cmd(t_data *data, char *path);
void	check_cmd(t_data *data, char	**cmd_path, char *args_str);

//error_and_free
void	*free_arrays(char **splitted);
void	free_struct(t_data *data);
void	error_exit(char *msg, t_data *data, int exit_code);
void	error_manual(char *msg, char *msg_2, t_data *data, int exit_code);
void	close_fds_and_exit(t_data *data, int file, char *msg, int exit_code);

//utils
void	find_path(t_data *data, char **envp);
char	*pipex_strjoin(char const *s1, char const *s2, char const *s3);
#endif
