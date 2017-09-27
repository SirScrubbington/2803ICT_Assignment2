// trial_division(uint32_t, char *,int,int*,double*): void
// Performs trial-division for a given integer when called by a thread in the threadpool.
void trial_division(uint32_t n, char * shmpt,int threadno, int * canwrite, double * progress, pthread_mutex_t * mutex){
	uint32_t p;
	for(p=1;p<n;p++){
		if (n%p==0){
			pthread_mutex_lock(mutex);
			*canwrite=1;
			char fact[MAX];
			snprintf(fact,MAX,"%lu",p);
			strcpy(shmpt,fact);
			pthread_mutex_unlock(mutex);
		}
		*progress = ((double)p / (double)n * 100);
		//printf("%f,",*progress);
	}
}

// lazy_rotr(uint32_t, int): uint32_t
// Rotates a given uint32_teger right by the given number of bits.
uint32_t lazy_rotr(uint32_t value, int shift){
	return ((value >> shift) | (value << (32 - shift)));
}

// test_rotation(void): void
// Simple function to test lazy_rotr.
void test_rotation(){
	int i,j;
	uint32_t n = 0xFFFFFFFF;
	for(i=0;i<32;i++){
		n = lazy_rotr(n,1);
		printf("%u\n",n);
	}
}
