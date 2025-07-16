/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 18:09:57 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/16 13:06:24 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

//check if used
# include "libft.h"

# include <unistd.h> // For fork, pipe, dup2, execve, access, close, read, write
# include <sys/wait.h> // For wait, waitpid
# include <fcntl.h> // For open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
# include <stdlib.h> // For exit, malloc, free
 # include <stdio.h> // For perror
// # include <string.h> // For strerror (though perror is often enough)

# define READ_END 0
# define WRITE_END 1

typedef struct s_data
{
	int		pipefd[2];
	char	**argv; // i guess i need it?
	char	**envp; // do i need it?
	char	**cmd_args;
	char 	**cmd_path;
}	t_data;

#endif
