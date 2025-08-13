#include "minishell.h"

void	pwd_builtin(void)
{
	char	*buf;

	buf = getcwd(NULL, 0);
	add_to_gc(buf);
	if (buf == NULL)
	{
		perror("pwd");
		return ;
	}
	printf("%s\n", buf);
	// free(buf); // GC
}


