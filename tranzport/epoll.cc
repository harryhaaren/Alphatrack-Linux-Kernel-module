typedef union epoll_data {
	void *ptr;
	int   fd;
	__uint32_t u32;
	__uint64_t u64;
} epoll_data_t;

struct epoll_event {
	__uint32_t events; /* Epoll events */
	epoll_data_t data; /* User data variable */
};

void mainloop(int queue)
{
  struct epoll_event list[5];
  int                events;
  int                i;
  struct foo         data;

  int epoll_fd = epoll_create(5);
  if (epoll_fd == -1) {
    // epoll not available

  }

  timerfd readtimer;
  timerfd writetimer;
  
  list[0].events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT ; // read fd
  list[0].events = list[0].events & ^EPOLLOUT;   // read fd
  list[1].events = list[0].events & ^EPOLLIN;  // write fd
  list[2].events = list[0].events;  // read timer fd
  list[3].events = list[0].events;  // writer timer fd
  list[4].events = list[0].events;  // tempo fd
  
  list[3].data.fd = readtimer.getfd();
  list[4].data.fd = writetimer.getfd();

 while(1)
  {
    events = epoll_wait(queue,list,10,TIMEOUT);
    if (events < 0)
      continue;	/* error, but we ignore for now */
    for (i = 0 ; i < events ; i++)
    {
      data = list[i].data.ptr;
      (*data->fn)(&list[i]);	/* call our function */
    }
  }
}

