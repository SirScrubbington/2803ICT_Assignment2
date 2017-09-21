int trial_division(unsigned long n, unsigned long * result){
	unsigned long p;
	int i;
	for(p=1;p<n;p++){
		if (n%p==0){
			if(i < 1024){
				result[i]=p;
				i++;
			}
		}
	}
	return i;
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
