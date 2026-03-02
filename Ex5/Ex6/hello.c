#include <stdio.h>
#include <pthread.h>

void* threadFn(void* argv){
	printf("Hello from thread %s\n", (char*)argv);
}

int main() {
	int i=0;
	pthread_t thread_id;
	for(i=0;i<4;i++){
		pthread_create(&thread_id, NULL, threadFn , (void*));
		pthread_join(thread_id, NULL);
	}
}
