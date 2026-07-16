#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

#include<sys/select.h>
#include<arpa/inet.h>
#include<netinet/ip.h>




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

int count = 0;
int max_fd = 0;
int sockfd = -1;
int ids[65536];
char *msgs[65536];
char *to_send[65536];
int to_send_len[65536];
int to_send_off[65536];
int to_send_cap[65536];

fd_set read_fds, write_fds, all_fds;

char buf_read[1001], buf_write[42];

void fatal_error()
{
    write(2, "Fatal error\n", 12);
    exit(1);
}

int create_socket()
{
    max_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (max_fd < 0)
        fatal_error();
    FD_SET(max_fd, &all_fds);
    return max_fd;
}

// Appends str to fd's pending output buffer. Capacity doubles instead of
// growing to an exact fit, so realloc only actually reallocates O(log n)
// times as the backlog grows -- appending stays O(len(str)) amortized
// instead of O(len(backlog)) per call. Actual sending happens in
// flush_client(), so a client that isn't writable yet never loses data.
void queue_msg(int fd, char *str)
{
    int addlen;
    int needed;
    int newcap;
    char *newbuf;

    addlen = strlen(str);
    needed = to_send_len[fd] + addlen + 1;
    if (needed > to_send_cap[fd])
    {
        newcap = to_send_cap[fd] * 2;
        if (newcap < needed)
            newcap = needed;
        newbuf = realloc(to_send[fd], newcap);
        if (newbuf == 0)
            fatal_error();
        to_send[fd] = newbuf;
        to_send_cap[fd] = newcap;
    }
    strcpy(to_send[fd] + to_send_len[fd], str);
    to_send_len[fd] += addlen;
}

// Sends as much of fd's pending buffer as the socket accepts right now and
// remembers how much was sent via to_send_off, so a partial send never needs
// to shift the remaining bytes down. Buffer is freed once fully drained.
void flush_client(int fd)
{
    int sent;
    int remaining;

    if (to_send[fd] == 0 || !FD_ISSET(fd, &write_fds))
        return;
    remaining = to_send_len[fd] - to_send_off[fd];
    sent = send(fd, to_send[fd] + to_send_off[fd], remaining, 0);
    if (sent <= 0)
        return;
    to_send_off[fd] += sent;
    if (to_send_off[fd] == to_send_len[fd])
    {
        free(to_send[fd]);
        to_send[fd] = 0;
        to_send_len[fd] = 0;
        to_send_off[fd] = 0;
        to_send_cap[fd] = 0;
    }
}

void notify_other(int author, char *str)
{
    for (int fd = 0; fd <= max_fd; fd++)
    {
        if (FD_ISSET(fd, &all_fds) && fd != author && fd != sockfd)
        {
            queue_msg(fd, str);
        }
    }
}

void register_client(int fd)
{
    if(fd > max_fd)
        max_fd = fd;

    ids[fd] = count++;
    msgs[fd] = NULL;
    to_send[fd] = NULL;
    to_send_len[fd] = 0;
    to_send_off[fd] = 0;
    to_send_cap[fd] = 0;
    FD_SET(fd, &all_fds);

    sprintf(buf_write, "server: client %d just arrived\n", ids[fd]);
    notify_other(fd, buf_write);
}

void remove_client(int fd)
{
    sprintf(buf_write, "server: client %d just left\n", ids[fd]);
    notify_other(fd, buf_write);

    free(msgs[fd]);
    free(to_send[fd]);
    to_send[fd] = NULL;
    to_send_len[fd] = 0;
    to_send_off[fd] = 0;
    to_send_cap[fd] = 0;
    FD_CLR(fd, &all_fds);
    close(fd);
}

void send_msg(int fd)
{
    char *msg;
    int ret;

    ret = extract_message(&(msgs[fd]), &msg);
    while (ret > 0)
    {
        sprintf(buf_write, "client %d: ", ids[fd]);
        notify_other(fd, buf_write);

        notify_other(fd, msg);
        free(msg);
        ret = extract_message(&(msgs[fd]), &msg);
    }
    if (ret < 0)
        fatal_error();
}

int main(int argc, char **argv) {

	if (argc != 2)
    {
		write(2, "Wrong number of arguments\n", 26);
		exit (1);
	}

	FD_ZERO(&all_fds);
	sockfd = create_socket();

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
		fatal_error();
	}
	if (listen(sockfd, 128) != 0)
    {
		fatal_error();
	}

	while (1)
    {

		read_fds = write_fds = all_fds;
		if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
        {
			fatal_error();
		}

		for (int fd = 0; fd <= max_fd; fd++)
        {
			if (!FD_ISSET(fd, &read_fds))
				continue;

			if (fd == sockfd){
				socklen_t addr_len = sizeof(servaddr);
				int client_fd = accept(sockfd, (struct sockaddr*)&servaddr, &addr_len);
				if (client_fd >= 0 )
                {
					register_client(client_fd);
					break;
				}
			}
			else
            {
				int read_bytes = recv(fd, buf_read, 1000, 0);
				if (read_bytes <= 0)
                {
					remove_client(fd);
					break;
				}
				buf_read[read_bytes] = '\0';
				char *joined = str_join(msgs[fd], buf_read);
				if (joined == 0)
					fatal_error();
				msgs[fd] = joined;
				send_msg(fd);
			}
		}

		for (int fd = 0; fd <= max_fd; fd++)
			if (fd != sockfd)
				flush_client(fd);
	}
	return 0;
}
