#include "minishell.h"


void	custom_pwd(void)
{
	int status;
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


