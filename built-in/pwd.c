#include "minishell.h"

void	pwd_builtin(void)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf == NULL)
	{
		perror("pwd");
		return ;
	}
	printf("%s\n", buf);
	free(buf);
}


