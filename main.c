/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: okumurahyu <okumurahyu@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 15:09:25 by okumurahyu        #+#    #+#             */
/*   Updated: 2022/05/22 23:21:01 by okumurahyu       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopers.h"

typedef struct s_philo
{
	int				n;
	pthread_mutex_t *right_fork;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *fg_print;
	pthread_t		*pthread;
	struct s_philo	*next;
	struct s_philo	*prev;
	long			num_of_philo;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	long			eat_times;
	long			last_eat_time;
	int				*fg_exit;
}					t_philo;

/* typedef struct s_input
{
	int	philo_count;
	
}					t_input; */

/* typedef struct s_fork
{
	char			*name;
	t_list			*args;
	char			*stdin_str;
	int				fd_out;
	struct s_cmd	*prev;
	struct s_cmd	*next;
}					t_fork; */

void *func(void *p);
static void	init_philo(t_philo **philo);
static int	generate_first_philo(t_philo **philo);
static void from_input(t_philo *philo);
static int	set_flag(t_philo **philo);
static void	add_philo(t_philo *philo);
void	thinking(t_philo *philo);
void	eating(t_philo *philo);
void	sleeping(t_philo *philo);
void	wait_eating(t_philo *philo);
void	wait_time(long wait_time);
long	now_time();
//int n = 0;

int	main(void)
{
	int i;
	t_philo	*philo;

	init_philo(&philo);
	i = 0;
	while (i < philo->num_of_philo)
	{
		if (pthread_create(philo->pthread, NULL, &func, philo) == 0)
			philo->last_eat_time = now_time();//last_eat_time_update()
		else
			printf("err\n");
		philo = philo->next;
		i++;
	}
	philo = philo->next;
	i = 0;
	while (i < philo->num_of_philo)
	{
		pthread_join(*(philo->pthread), NULL);
		philo = philo->next;
		i++;
	}
	return (0);
	
}

static void	init_philo(t_philo **philo)
{
	//pthread_mutex_t	mutex;
	int	i;
	pthread_t		pthread;

	/* *philo = (t_philo *)malloc(sizeof(t_philo) * 1);
	(*philo)->next = *philo;
	(*philo)->prev = *philo;
	(*philo)->n = 1;
	(*philo)->pthread = &pthread;
	(*philo)->left_fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((*philo)->left_fork, NULL); */
	
	generate_first_philo(philo);
	
	from_input(*philo);
	set_flag(philo);
	i = 1;
	while (i < (*philo)->num_of_philo)
	{
		add_philo(*philo);
		i++;
	}
	i = 0;
	while (i < (*philo)->num_of_philo)
	{
		(*philo)->right_fork = (*philo)->prev->left_fork;
		(*philo) = (*philo)->next;
		i++;
	}
}

static int	generate_first_philo(t_philo **philo)
{
	pthread_t		pthread;

	*philo = (t_philo *)malloc(sizeof(t_philo) * 1);
	(*philo)->next = *philo;
	(*philo)->prev = *philo;
	(*philo)->n = 1;
	(*philo)->pthread = &pthread;
	(*philo)->left_fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((*philo)->left_fork, NULL);
	return (1);
}

static void	from_input(t_philo *philo)
{
	(philo)->num_of_philo = 5;
	(philo)->time_to_die = 10000;
	(philo)->time_to_eat = 200;
	(philo)->time_to_sleep = 200;
	(philo)->eat_times = 4;
}

static int	set_flag(t_philo **philo)
{
	(*philo)->fg_print = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	(*philo)->fg_exit = (int *)malloc(sizeof(int));
	*((*philo)->fg_exit) = 0;
	return (1);
}

static void	add_philo(t_philo *philo)
{
	pthread_mutex_t	mutex;
	pthread_t		pthread;
	t_philo			*new;
	
	new = (t_philo *)malloc(sizeof(t_philo) * 1);
	if (new == NULL)
		return ;
	new->n = philo->prev->n + 1;
	new->pthread = &pthread;
	new->left_fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	new->fg_print = philo->fg_print;
	pthread_mutex_init(new->left_fork, NULL);
	new->num_of_philo = philo->num_of_philo;
	new->time_to_die = philo->time_to_die;
	new->time_to_eat = philo->time_to_eat;
	new->time_to_sleep = philo->time_to_sleep;
	new->eat_times = philo->eat_times;
	new->fg_exit = philo->fg_exit;
	philo->prev->next = new;
	new->next = philo;
	new->prev = philo->prev;
	philo->prev = new;
}

void *func(void *p)
{
	t_philo	*philo;
	int		i;
	
	philo = (t_philo *)p;
	i = 0;
	while (i < philo->eat_times)//
	{
		thinking(philo);
		eating(philo);
		sleeping(philo);
		i++;
	}
	return (0);
}

void	print_doing(t_philo *philo, char *doing)
{
	pthread_mutex_lock(philo->fg_print);
	printf("%ld %d%s\n", now_time(), philo->n, doing);
	pthread_mutex_unlock(philo->fg_print);
}

void	thinking(t_philo *philo)
{
	print_doing(philo, THINK);
}

void	eating(t_philo *philo)
{
	if (philo->n % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		print_doing(philo, FORK);
		pthread_mutex_lock(philo->left_fork);
		print_doing(philo, FORK);
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		print_doing(philo, FORK);
		pthread_mutex_lock(philo->right_fork);
		print_doing(philo, FORK);
	}
	print_doing(philo, EAT);
	//wait_time(philo->time_to_eat);
	wait_eating(philo);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void	wait_eating(t_philo *philo)
{
	const long	start_time = now_time();

	while ((now_time() - start_time) < philo->time_to_eat)
		usleep(25);
	philo->last_eat_time = now_time();
}

void	wait_time(long wait_time)
{
	const long	start_time = now_time();

	while ((now_time() - start_time) < wait_time)
		usleep(25);
}

void	sleeping(t_philo *philo)
{
	print_doing(philo, SLEEP);
	wait_time(philo->time_to_sleep);
}

long	now_time()
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000);
}

/* void	check_dead(t_philo *philo)
{
	if (now_time() - philo->last_eat_time >= philo->time_to_die)
		*(philo->fg_exit) = 1;
	if (*(philo->fg_exit) == 1)
		pthread_detach
} */