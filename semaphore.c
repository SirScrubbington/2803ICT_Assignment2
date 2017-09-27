#define Mutex pthread_mutex_t
#define Cond pthread_cond_t



typedef struct
{
int value;		//- the semaphore’s value
int wakeups;		//- the number of pending signals to avoid thread starvation
Mutex*mutex;		//- used to protect value and wakeups
Cond*cond;		//- for waiting on the semaphore
}	Semaphore;

typedef struct
{
	Semaphore*s;
	int n;
} testargs;

Semaphore* make_semaphore(int);
Cond*make_cond();
Mutex*make_mutex();
void sem_wait(Semaphore*);
void sem_signal(Semaphore*);

Semaphore*make_semaphore(int value){
	Semaphore*semaphore = ( Semaphore*)malloc(sizeof(Semaphore));
	semaphore->value = value;
	semaphore->wakeups = 0;
	semaphore->mutex = make_mutex();
	semaphore->cond = make_cond();
	return semaphore;
}

Cond*make_cond(){
	Cond* cond = (Cond*)malloc(sizeof(Cond));
	pthread_cond_init(cond, NULL);
	return cond;
}

Mutex*make_mutex(){
	Mutex * mutex = (Mutex *)malloc(sizeof(Mutex));
	pthread_mutex_init(mutex, NULL);
	return mutex;
}

void sem_wait(Semaphore* s){
	pthread_mutex_lock(s->mutex);
	while(s->value<=0)
		pthread_cond_wait(s->cond,s->mutex);
	s->value++;
	pthread_mutex_unlock(s->mutex);
}

void sem_signal(Semaphore* s){
	pthread_mutex_lock(s->mutex);
	s->value--;
	s->wakeups++;
	pthread_cond_signal(s->cond);
	pthread_mutex_unlock(s->mutex);
}

// struct fact_args * arg = (struct fact_args*)args;

void * test_thread_1(void* args){
	testargs * arg = (testargs*)args;
	for(;;){
		while(arg->n<100){
			arg->n++;
			printf("%i\n",arg->n);
		}
		sem_signal(arg->s);
		sem_wait(arg->s);
	}
}
void * test_thread_2(void* args){
	testargs * arg = (testargs*)args;
	for(;;){
		sem_wait(arg->s);
		while(arg->n>0){
			arg->n--;
			printf("%i\n",arg->n);
		}
		sem_signal(arg->s);
	}
}

void test_sem(void){
	Semaphore*s = make_semaphore(0);
	pthread_t th[2];
	int id[2];
	testargs t;
	t.n=0;
	t.s=s;
	id[0]=pthread_create(&th[0],NULL,test_thread_1,&t);
	id[1]=pthread_create(&th[1],NULL,test_thread_2,&t);
}