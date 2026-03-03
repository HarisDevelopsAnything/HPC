#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

typedef struct {
	int a;
	int b;
	char op; /* '+', '-', '*', '/' */
	int id;
} job_t;

/* queue */
static job_t *queue = NULL;
static int q_size = 0;
static int q_head = 0;
static int q_tail = 0;
static int q_count = 0;

sem_t q_full;  /* number of items available */
sem_t q_empty; /* number of free slots */
sem_t write_sem; /* protect file writes */
pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;

static int total_jobs = 0;

void enqueue_job(job_t j){
	sem_wait(&q_empty);
	pthread_mutex_lock(&q_mutex);
	queue[q_tail] = j;
	q_tail = (q_tail + 1) % q_size;
	q_count++;
	pthread_mutex_unlock(&q_mutex);
	sem_post(&q_full);
}

job_t dequeue_job(){
	job_t j = {0,0,'?',-1};
	sem_wait(&q_full);
	pthread_mutex_lock(&q_mutex);
	j = queue[q_head];
	q_head = (q_head + 1) % q_size;
	q_count--;
	pthread_mutex_unlock(&q_mutex);
	sem_post(&q_empty);
	return j;
}

void* producer(void* arg){
	int n = *(int*)arg;
	for(int i=0;i<n;i++){
		job_t j;
		j.id = i+1;
		j.a = rand() % 101; /* 0..100 */
		j.b = rand() % 101; /* 0..100 */
		int op = rand() % 4;
		switch(op){
			case 0: j.op = '+'; break;
			case 1: j.op = '-'; break;
			case 2: j.op = '*'; break;
			case 3: j.op = '/';
					if(j.b==0) j.b = 1; /* avoid division by zero */
					break;
		}
		enqueue_job(j);
	}
	return NULL;
}

void* consumer(void* arg){
	int to_process = *(int*)arg;
	int processed = 0;
	while(processed < to_process){
		job_t j = dequeue_job();
		long result = 0;
		switch(j.op){
			case '+': result = (long)j.a + j.b; break;
			case '-': result = (long)j.a - j.b; break;
			case '*': result = (long)j.a * j.b; break;
			case '/': result = (long)j.a / j.b; break;
			default: break;
		}
		sem_wait(&write_sem);
		FILE *f = fopen("results.txt", "a");
		if(f){
			fprintf(f, "Job %d: %d %c %d = %ld\n", j.id, j.a, j.op, j.b, result);
			fclose(f);
		}
		sem_post(&write_sem);
		processed++;
	}
	return NULL;
}

int main(){
	printf("Enter the number of operations: ");
	int n;
	if(scanf("%d", &n)!=1 || n<=0) return 1;
	total_jobs = n;

	srand((unsigned)time(NULL));

	/* queue size: choose a small buffer, e.g., 8 or min(n,8) */
	q_size = (n < 8) ? n : 8;
	queue = calloc(q_size, sizeof(job_t));
	if(!queue) return 2;

	sem_init(&q_full, 0, 0);
	sem_init(&q_empty, 0, q_size);
	sem_init(&write_sem, 0, 1);

	pthread_t prod, cons;

	if(remove("results.txt")==0){ /* clear previous results if existed */ }

	pthread_create(&prod, NULL, producer, &n);
	pthread_create(&cons, NULL, consumer, &n);

	pthread_join(prod, NULL);
	pthread_join(cons, NULL);

	sem_destroy(&q_full);
	sem_destroy(&q_empty);
	sem_destroy(&write_sem);
	free(queue);

	printf("Completed %d operations. Results in results.txt\n", n);
	return 0;
}
