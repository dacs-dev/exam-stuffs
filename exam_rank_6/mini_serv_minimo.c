#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

/* ===== COPIADO TAL CUAL DEL main.c QUE TE DAN ===== */

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

/* ===== A PARTIR DE AQUI LO ESCRIBES TU ===== */

int		ids[1024];
char	*msgs[1024];
int		count = 0, max_fd = 0, sockfd;
fd_set	afds, rfds, wfds;
char	buf_read[1001], buf_write[64];

void fatal(void)
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

void send_all(int except, char *s)
{
	for (int i = 0; i <= max_fd; i++)
		if (FD_ISSET(i, &wfds) && i != except && i != sockfd)
			send(i, s, strlen(s), 0);
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}

	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fatal();
	max_fd = sockfd;
	FD_ZERO(&afds);
	FD_SET(sockfd, &afds);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	servaddr.sin_port = htons(atoi(av[1]));
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
		fatal();
	if (listen(sockfd, 128) != 0)
		fatal();

	while (1)
	{
		rfds = wfds = afds;
		if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
			fatal();

		for (int fd = 0; fd <= max_fd; fd++)
		{
			if (!FD_ISSET(fd, &rfds))
				continue;

			if (fd == sockfd)
			{
				int cfd = accept(sockfd, NULL, NULL);
				if (cfd < 0)
					continue;
				if (cfd > max_fd)
					max_fd = cfd;
				ids[cfd] = count++;
				msgs[cfd] = NULL;
				FD_SET(cfd, &afds);
				sprintf(buf_write, "server: client %d just arrived\n", ids[cfd]);
				send_all(cfd, buf_write);
				break;
			}

			int r = recv(fd, buf_read, 1000, 0);
			if (r <= 0)
			{
				sprintf(buf_write, "server: client %d just left\n", ids[fd]);
				send_all(fd, buf_write);
				free(msgs[fd]);
				msgs[fd] = NULL;
				FD_CLR(fd, &afds);
				close(fd);
				break;
			}
			buf_read[r] = 0;
			msgs[fd] = str_join(msgs[fd], buf_read);
			if (msgs[fd] == 0)
				fatal();

			char *msg;
			int ret;
			while ((ret = extract_message(&msgs[fd], &msg)) > 0)
			{
				sprintf(buf_write, "client %d: ", ids[fd]);
				send_all(fd, buf_write);
				send_all(fd, msg);
				free(msg);
			}
			if (ret < 0)
				fatal();
		}
	}
	return 0;
}
