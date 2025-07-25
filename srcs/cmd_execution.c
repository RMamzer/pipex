/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_execution.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 13:27:09 by rmamzer           #+#    #+#             */
/*   Updated: 2025/07/25 14:35:58 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	execute_cmd(t_data *data, char *path)
{
	if (access(path, F_OK) == 0)
	{
		if (access (path, X_OK) == 0)
		{
			execve(path, data->cmd_args, data->envp);
			free(path);
			error_exit(data->cmd_args[0], data, CMD_NOT_EXEC);
		}
		free(path);
		error_exit(data->cmd_args[0], data, CMD_NOT_EXEC);
	}
	free (path);
}

void	execute_path_cmd(t_data *data, char *path)
{
	if (access(path, F_OK) != 0)
		error_exit(path, data, CMD_NOT_FOUND);
	if (access(path, X_OK) != 0)
		error_exit(path, data, CMD_NOT_EXEC);
	execve (path, data->cmd_args, data->envp);
	error_exit(path, data, CMD_NOT_EXEC);
}

void	check_cmd(t_data *data, char	**cmd_path, char *args_str)
{
	char	*joined_path;

	data->cmd_args = ft_split(args_str, ' ');
	if (!data->cmd_args)
		error_exit("Failed malloc in splitting arguments", data, EXIT_FAILURE);
	if (!data->cmd_args[0])
		error_manual(args_str, ": command not found", data, CMD_NOT_FOUND);
	if (ft_strchr(data->cmd_args[0], '/'))
		execute_path_cmd(data, data->cmd_args[0]);
	if (!cmd_path)
		error_manual(data->cmd_args[0], ": command not found",
			data, CMD_NOT_FOUND);
	while (*cmd_path)
	{
		joined_path = pipex_strjoin(*cmd_path, "/", data->cmd_args[0]);
		if (!joined_path)
			error_exit("pipex_strjoin malloc failed", data, EXIT_FAILURE);
		execute_cmd(data, joined_path);
		cmd_path++;
	}
	error_manual(data->cmd_args[0], ": command not found", data, CMD_NOT_FOUND);
}
