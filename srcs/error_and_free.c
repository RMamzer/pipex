/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_and_free.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 13:24:26 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/25 14:34:57 by rmamzer          ###   ########.fr       */
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

void	free_struct(t_data *data)
{
	if (data->cmd_args)
		free_arrays(data->cmd_args);
	if (data->cmd_path)
		free_arrays(data->cmd_path);
}

void	error_exit(char *msg, t_data *data, int exit_code)
{
	char	*error_msg;

	error_msg = ft_strjoin("pipex: ", msg);
	if (!error_msg)
	{
		perror("pipex: error message maloc failed");
		free_struct(data);
		exit(EXIT_FAILURE);
	}
	perror(error_msg);
	free(error_msg);
	free_struct(data);
	exit (exit_code);
}

void	error_manual(char *msg, char *msg_2, t_data *data, int exit_code)
{
	char	*error_msg;

	error_msg = pipex_strjoin("pipex: ", msg, msg_2);
	if (!error_msg)
	{
		perror("pipex: error message maloc failed");
		free_struct(data);
		exit(EXIT_FAILURE);
	}
	ft_putendl_fd(error_msg, STDERR_FILENO);
	free(error_msg);
	free_struct(data);
	exit (exit_code);
}

void	close_fds_and_exit(t_data *data, int file, char *msg, int exit_code)
{
	close(data->pipefd[READ_END]);
	close(data->pipefd[WRITE_END]);
	if (file >= 0)
		close (file);
	error_exit(msg, data, exit_code);
}
