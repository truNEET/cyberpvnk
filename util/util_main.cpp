#include "util_main.h"

int util_shell(int function,char* parameter){ // Keeps most of the OS pre-processor code isolated from the rest of the program
	int return_value = 0;
	std::string command = "";
	switch(function){
	case UTIL_SHELL_DELETE:
		#ifdef __linux
			command = "rm -r " + (std::string)parameter;
		#elif _WIN32
			command = "del " + (char*)parameter;
		#endif
		return_value = system((char*)command.c_str());
		break;
	default:
		return_value = -1;
		break;
	}
	return return_value;
}

void s_sleep(long double s){
	ms_sleep(1000*s);
}

void ms_sleep(long double ms){
	#ifdef __linux
		usleep((unsigned int)(1000*ms));
	#elif USE_SDL
		SDL_Delay(ms);
	#elif _WIN32
		ms += .5;
		Sleep((int)ms); // I think this is correct
	#endif
}

static unsigned long int old_rand = 2;

unsigned long int gen_rand(unsigned int a){ // range, from 0 to a
	srand(time(NULL)*old_rand);
	unsigned long int return_value = rand()%a;
	if(return_value == 0){
		return_value++;
	}
	old_rand = return_value;
	return return_value;
}

short int term_if_true(bool a, char* details = NULL){
	if(__builtin_expect(a,true)){
		if(details != NULL) printf("'%s' term parameter failed\n",details);
		#ifndef NDEBUG
			assert(false);
		#else
			exit(0);
		#endif
		return TERMINATE;
	}
	return 0;
}

short int warn_if_true(bool a, char* details = NULL){
	if(__builtin_expect(a,true)){
		if(details != NULL) printf("'%s' warn parameter failed\n",details);
	}
	return 0;
}

bool double_cmp(long double a,long double b,long double degree_of_error){
	if(a-degree_of_error < b){
		if(a+degree_of_error > b){
			return true;
		}
	}
	return false;
}

long double get_time(){
	#ifdef _WIN32
		return SDL_GetTicks()/1000; // piece of crap timer
	#elif __linux // possibly OS X?
		timespec a;
		clock_gettime(CLOCK_MONOTONIC, &a);
		return a.tv_sec + ((long double)a.tv_nsec/(long double)1000000000.0);
	#endif
}

bool sign(long double *a){
	if(*a > 0){
		return true;
	}
	return false;
}

int encrypt(int *a, int size){
	int average = 0;
	for(int i = 0;i < size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

int encrypt(std::vector<int> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

int encrypt(std::vector<int*> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+*(a[i]))/2;
	}
	return average;
}

int encrypt(std::vector<double> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

int encrypt(std::vector<double*> a){
	const unsigned int a_size = a.size();
	long double average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+*(a[i]))/2;
	}
	return average;
}

int encrypt(std::vector<long double> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+a[i])/2;
	}
	return average;
}

int encrypt(std::vector<long double*> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		average = (average+*(a[i]))/2;
	}
	return average;
}

int encrypt(std::vector<std::string> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		const unsigned int a_i_size = a[i].size();
		for(unsigned int n = 0;n < a_i_size;n++){
			average = (average+a[i][n])/2;
		}
	}
	return average;
}

int encrypt(std::vector<std::string*> a){
	const unsigned int a_size = a.size();
	int average = 0;
	for(unsigned int i = 0;i < a_size;i++){
		const unsigned int a_i_size = a[i]->size();
		for(unsigned int n = 0;n < a_i_size;n++){
			average = (average+(*a[i])[n])/2;
		}
	}
	return average;
}

std::string wrap(char *start, std::string data, char *end){
	return (std::string)start + data + (std::string)end;
}

bool check_for_parameter(const std::string a, int argc_, char **argv_){
	for(int i = 0;i < argc_;i++){
		if(std::string(argv_[i]) == a){
			return true;
		}
	}
	return false;
}

void switcH_values(void *a, void *b){
	void *c = a;
	a = b;
	b = c;
}

void switch_values(void **a, void **b){
	void **c = a;
	a = b;
	b = c;
}
/*
// tgmath from stackexchange contributed this little bit:

// should I optimize for nearly sorted vectors?

int quick_sort_partition(std::vector<sort_t>& A, int p,int q){
	int x= A[p].value;
	int i=p;
	int j;
	for(j=p+1; j<q; j++){
		if(A[j].value <= x){ // unlikely()?
			i=i+1;
			std::swap(A[i],A[j]);
		}
	}
	std::swap(A[i],A[p]);
	return i;
}

void quick_sort(std::vector<sort_t>& A, int p,int q){
	int r;
	if(p < q){
		r=quick_sort_partition(A, p ,q);
		quick_sort(A,p,r);  
		quick_sort(A,r+1,q);
	}
}

// I hope this can run faster than std::sort().
void sorting_algorithm(std::vector<void*> *pointer, int mem_offset_bytes){
	const unsigned long int pointer_size = pointer->size();
	std::vector<sort_t> tmp_sort;
	for(unsigned long int i = 0;i < pointer_size;i++){
		sort_t sort_;
		sort_.pointer = (*pointer)[i];
		bool* bool_pointer = (bool*)(*pointer)[i];
		sort_.value = *(bool_pointer + mem_offset_bytes); // don't use void* since void* has no set size, bool has a size of one
		tmp_sort.push_back(sort_);
	}
	quick_sort(tmp_sort, 0, tmp_sort.size()+1);
	for(unsigned long int i = 0;i < pointer_size;i++){
		(*pointer)[i] = tmp_sort[i].pointer;
	}
}


// quick_vector_search(std::vector<void*>, int, int): searches the vector for a value (at the offset).
// this takes advantage of the quick sort algorithm and searches for the value pretty fast
// this only provides a speedup if the vector is sorted with quick sort
// this only works when a vector is of a pointer type (although a non-pointer type shouldn't be that hard to implement).

int quick_vector_search(std::vector<void*> vector_, int search_offset, int value){
	long int split_point = vector.size()/2;
	while(true){
		const int tmp_value = *(vector_[split_point]+search_offset);
		if(tmp_value > value){
			split_point *= -0.5*split_point;
		}else if(tmp_value < value){
			split_point *= 0.5*split_point;
		}else if(tmp_value == value){
			return split_point;
		}
		if(split_point <= 0 || split_point >= vector_size()-1){
			break;
		}
	}
	for(unsigned long int i = 0;i < vector_.size();i++){
		if(*(vector_[i]+search_offset) == value){
			return i;
		}
	}
}
*/
