#include "minishell.h"

int status;

void	custom_pwd(void)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	if (buf == NULL)
	{
		perror("Error");
		status = 1;
		return ;
	}
	status = 0;
	free(buf);
}


