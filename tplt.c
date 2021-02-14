#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <inttypes.h>
#include <curl/curl.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
static volatile int execute = 1;
static volatile int tc_modify = 0;
extern int errno;
void print_usage() {
	printf("Usage: tplt <url> -s SOURCE_INTERFACE -c GET_COUNT -l LOSS -L (0-99) LATENCY (ms)\n");
}
size_t discard(void *ptr, size_t size, size_t nmemb, void *data) {
	return size * nmemb;
}
void sig_handler(int val) {
	execute = 0;
}
void cleanup(char* si) {
	if(tc_modify == 0) {
		char clear_buf[256];
		printf("Clearing latency and loss from interface %s\n",si);
		snprintf(clear_buf,sizeof(clear_buf),"%s%s%s","tc qdisc del dev ",si," root");
		if(system(clear_buf) != 0) {
			printf("Failed to delete tc rules\n");
			exit(EXIT_FAILURE);
		};
	};
}
int main(int argc, char *argv[]) {
	char* url;
	char* si;
	uint32_t get_count;
	float loss;
	uint32_t latency;
	uint32_t diff_time;
	int option = 0;
	int return_code;
	signal(SIGINT, sig_handler);
	while((option = getopt(argc, argv, "s:c:l:L:")) != -1) {
		switch(option) {
			case 's':
				si = optarg;
				break;
			case 'c':
				get_count = atoi(optarg);
				break;
			case 'l':
				loss = strtof(optarg,NULL);
				break;
			case 'L':
				latency = atoi(optarg);
				break;
			case 'H':
				print_usage();
				exit(EXIT_SUCCESS);
			default: print_usage();
				 exit(EXIT_FAILURE);
		}
	}
	url = argv[argc - 1];
	struct hostent *he;
	//Check if url is legit, also populate resolver cache to reduce latency
	if(gethostbyname(url) == NULL) {
		printf("Failed to resolve URL %s\n",url);
		exit(EXIT_FAILURE);
	};
	printf("Attempting to perform %u HTTP GET requests against %s with a packet loss of %.2f percent and added latency of %u milliseconds\n", get_count, url, loss, latency);
	char add_buf[256];
	if(loss > 0.0) {
		snprintf(add_buf,sizeof(add_buf),"%s%s%s%f%s%d%s","tc qdisc add dev ",si," root netem loss ",loss,"% delay ",latency,"ms");
	} else if (latency > 0) {
		snprintf(add_buf,sizeof(add_buf),"%s%s%s%d%s","tc qdisc add dev ",si," root netem delay ",latency,"ms");
	} else {
		tc_modify = 1;
	}
	if(tc_modify == 0) {
		if(system(add_buf) != 0) {
			printf("Failed to apply tc rules\n");
			exit(EXIT_FAILURE);
		}; 
	}
	struct timespec start, end;
	CURL *curl;

	uint32_t count = 0;
	CURLcode rcode;
	curl = curl_easy_init();
	
        //Set options
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_INTERFACE, si);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10000);
	
        //Take first time measurement
	clock_gettime(CLOCK_MONOTONIC_RAW,&start);
	while(count < get_count && execute == 1) {
		rcode = curl_easy_perform(curl);
		if(rcode != CURLE_OK) {
			fprintf(stderr, "ERROR: %s\n", curl_easy_strerror(rcode));
			cleanup(si);
			exit(EXIT_FAILURE);
		}
		count++;
		//curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &return_code);
	};
	
        //Take final time measurement
	clock_gettime(CLOCK_MONOTONIC_RAW,&end);	
	diff_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
	printf("Total time to perform %d HTTP GET Requests: %u milliseconds\n",get_count,(diff_time/1000));	
	printf("Average response time: %d ms\n",(diff_time/1000)/get_count);
        //Cleanup
        curl_easy_cleanup(curl);
	cleanup(si);
}
