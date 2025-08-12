#include "minishell.h"

void	pwd_builtin(t_shell_state *shell)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf == NULL)
	{
		perror("pwd");
		shell->last_exit_status = 1;
		return ;
	}
	printf("%s\n", buf);
	free(buf);
	shell->last_exit_status = 0;
}


