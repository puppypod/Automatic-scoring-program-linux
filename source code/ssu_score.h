#ifndef MAIN_H_
#define MAIN_H_

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef STDOUT
	#define STDOUT 1 //표준 출력 파일디스크립터 번호
#endif
#ifndef STDERR
	#define STDERR 2 //표준 에러 파일디스크립터 번호
#endif
#ifndef TEXTFILE
	#define TEXTFILE 3 //텍스트 파일 리터럴 상수
#endif
#ifndef CFILE
	#define CFILE 4 //C파일 리터럴 상수
#endif
#ifndef OVER
	#define OVER 5 //제한 시간 5초
#endif
#ifndef WARNING
	#define WARNING -0.1 // warning 시에 점수 감점
#endif
#ifndef ERROR
	#define ERROR 0 //에러 리터럴 상수
#endif

#define FILELEN 64
#define BUFLEN 1024
#define SNUM 100
#define QNUM 100
#define ARGNUM 5 //-i, -m 뒤에 오는 인자의 개수를 5개로 한정

struct ssu_scoreTable{
	char qname[FILELEN];//파일 이름
	double score;//점수
};

void ssu_score(int argc, char *argv[]);
int check_option(int argc, char *argv[]);
void print_usage();

void score_students();
double score_student(int fd, char *id);
void write_first_row(int fd);

char *get_answer(int fd, char *result);
int score_blank(char *id, char *filename);
double score_program(char *id, char *filename);
double compile_program(char *id, char *filename);
int execute_program(char *id, char *filname);
pid_t inBackground(char *name);
double check_error_warning(char *filename);
int compare_resultfile(char *file1, char *file2);

void do_iOption(char (*ids)[FILELEN]);//c옵션 -> i옵션으로 변경
void do_mOption(char *cpy);//m옵션 추가
int is_exist(char (*src)[FILELEN], char *target);

int is_thread(char *qname);
void redirection(char *command, int newfd, int oldfd);
int get_file_type(char *filename);
void rmdirs(const char *path);
void to_lower_case(char *c);

void set_scoreTable(char *ansDir);
void read_scoreTable(char *path);
void make_scoreTable(char *ansDir);
void write_scoreTable(char *filename);
void set_idTable(char *stuDir);
int get_create_type();

void sort_idTable(int size);
void sort_scoreTable(int size);
void get_qname_number(char *qname, int *num1, int *num2);

#endif
