#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_score.h"

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
	struct timeval begin_t, end_t;
	gettimeofday(&begin_t, NULL);

	ssu_score(argc, argv);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);//프로그램을 실행하는데 걸리는 시간 측정해주는 함수

	exit(0);
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)//프로그램을 실행하는데 걸리는 시간 측정해주는 함수
{
	end_t->tv_sec -= begin_t->tv_sec;//초 단위의 끝 - 시작 시간 차이를 할당

	if(end_t->tv_usec < begin_t->tv_usec){//만약 마이크로초 단위 시간이 시작 시작이 더 크다면
		end_t->tv_sec--;
		end_t->tv_usec += SECOND_TO_MICRO;
	}

	end_t->tv_usec -= begin_t->tv_usec;//마이크로 초 단위 구하기
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}
