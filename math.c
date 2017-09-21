
void trial_division(unsigned long n, char * shmpt, int threadno){
	unsigned long p;
	for(p=1;p<n;p++){
		if (n%p==0){
			strcpy(shmpt,"Thread ");
			strcat(shmpt,(char*)itoa(threadno));
			strcat(shmpt," Factor: ");
			strcat(shmpt,(char*)ultoa(p));
			printf("%s\n",shmpt);
		}
	}
}

unsigned int _rotl(unsigned long value, int shift) {
    if ((shift &= sizeof(value)*8 - 1) == 0)
      return value;
    return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

unsigned int _rotr(unsigned long value, int shift) {
    if ((shift &= sizeof(value)*8 - 1) == 0)
      return value;
    return (value >> shift) | (value << (sizeof(value)*8 - shift));
}

void test_rotation(){
	unsigned long n;
	int i,j;
	for(i=0;i<10;i++){
		n = i*10000000;
		for(j=0;j<i;j++){
			printf("%i,%lu\n",i,n);
			n = _rotr(n,1);
		}
	}
}
