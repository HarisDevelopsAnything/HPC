#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

int dict_size = 10;
char* dict[] = {"Hello", "World", "Unix", "Linux", "BSD", "Git", "Docker", "Mepco", "HPC", "Lab"};

// Shared job queue
static int queue_capacity = 0;
static int *q_idx = NULL;
static char **q_word = NULL;
static int q_head = 0, q_tail = 0, q_count = 0;
static pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t q_not_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t q_not_full = PTHREAD_COND_INITIALIZER;
static bool done_adding = false;

int *results = NULL;
int n_global = 0;

void enqueue_job(int idx, char *word){
	pthread_mutex_lock(&q_mutex);
	while(q_count == queue_capacity){
		pthread_cond_wait(&q_not_full, &q_mutex);
	}
	q_idx[q_tail] = idx;
	q_word[q_tail] = word;
	q_tail = (q_tail + 1) % queue_capacity;
	q_count++;
	pthread_cond_signal(&q_not_empty);
	pthread_mutex_unlock(&q_mutex);
}

int dequeue_job(int *out_idx, char **out_word){
	pthread_mutex_lock(&q_mutex);
	while(q_count == 0 && !done_adding){
		pthread_cond_wait(&q_not_empty, &q_mutex);
	}
	if(q_count == 0 && done_adding){
		pthread_mutex_unlock(&q_mutex);
		return 0; // no more jobs
	}
	*out_idx = q_idx[q_head];
	*out_word = q_word[q_head];
	q_head = (q_head + 1) % queue_capacity;
	q_count--;
	pthread_cond_signal(&q_not_full);
	pthread_mutex_unlock(&q_mutex);
	return 1;
}

void* worker_thread(void *arg){
	free(arg);
	while(1){
		int idx;
		char *word;
		if(!dequeue_job(&idx, &word)) break;

		int ok = 0;
		for(int i=0;i<dict_size;i++){
			if(strcmp(word, dict[i]) == 0){
				ok = 1;
				break;
			}
		}
		results[idx] = ok;
	}
	return NULL;
}

int main() {
	int n;
	printf("Enter the number of words: ");
	if(scanf("%d", &n) != 1) return 1;
	n_global = n;
	char *words[n];
	printf("Enter the words: ");
	for(int i=0;i<n;i++){
		words[i] = malloc(64);
		if(words[i] == NULL) return 1;
		scanf("%63s", words[i]);
	}

	int num_threads = 4;
	printf("Enter number of worker threads: ");
	if(scanf("%d", &num_threads) != 1) num_threads = 4;

	// initialize queue and results
	queue_capacity = (n > 0) ? n : 1;
	q_idx = malloc(sizeof(int) * queue_capacity);
	q_word = malloc(sizeof(char*) * queue_capacity);
	results = malloc(sizeof(int) * n);
	for(int i=0;i<n;i++) results[i] = 0;

	pthread_t threads[num_threads];
	for(int i=0;i<num_threads;i++){
		int *id = malloc(sizeof(int));
		*id = i;
		pthread_create(&threads[i], NULL, worker_thread, id);
	}

	// enqueue all jobs
	for(int i=0;i<n;i++){
		enqueue_job(i, words[i]);
	}

	// signal no more jobs
	pthread_mutex_lock(&q_mutex);
	done_adding = true;
	pthread_cond_broadcast(&q_not_empty);
	pthread_mutex_unlock(&q_mutex);

	for(int i=0;i<num_threads;i++){
		pthread_join(threads[i], NULL);
	}

	// print results
	for(int i=0;i<n;i++){
		printf("%s : %s\n", words[i], results[i] ? "correct" : "incorrect");
	}

	// cleanup
	for(int i=0;i<n;i++) free(words[i]);
	free(q_idx);
	free(q_word);
	free(results);
	return 0;
}

