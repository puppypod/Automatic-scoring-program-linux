#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "ssu_score.h"
#include "blank.h"

extern struct ssu_scoreTable score_table[QNUM];
extern char id_table[SNUM][10]; // SNUM == 100임. 최대 100개까지만 가능하기 때문

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char errorDir[BUFLEN];
char threadFiles[ARGNUM][FILELEN];
char cIDs[ARGNUM][FILELEN];

int eOption = false;
int tOption = false;
int mOption = false; // <수정> cOption -> mOption
int iOption = false; // <수정> pOption -> iOption

void ssu_score(int argc, char* argv[])
{
	char saved_path[BUFLEN];
	int i;
	char *cpy;

	for (i = 0; i < argc; i++) { // argv에 -h가 존재한다면 print_usage() 함수를 호출
		if (!strcmp(argv[i], "-h")) {
			print_usage();
			return; // h옵션 출력 후 종료 
		}
	}

	memset(saved_path, 0, BUFLEN); // saved_path 0으로 초기화­
	if (argc >= 3 && strcmp(argv[1], "-i") != 0) { // <수정> 옵션 i로 변경 
		strcpy(stuDir, argv[1]); //STD_DIR 디렉토리 복사
		strcpy(ansDir, argv[2]); //ANS_DIR 디렉토리 복사
	}

	if (!check_option(argc, argv)) // 옵션을 체크하는 함수 호출
		exit(1);

	if (!eOption && !tOption && !mOption && iOption) { // i옵션일 경우 dp_iOption() 함수 호출
		do_iOption(cIDs); // do_cOption() -> do_iOption() 함수로 수정
		return;
	}

	getcwd(saved_path, BUFLEN); // 현재 작업중인 디렉토리 저장

	if (chdir(stuDir) < 0) { // stuDir로 디렉토리 변경
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN); // stuDir로 현재 작업중인 디렉토리 저장 <학생 답지 디렉토리 저장>

	chdir(saved_path); // 현재 작업 디렉토리 변경 <원래 디렉토리로 변경>
	if (chdir(ansDir) < 0) { // ansDir로 디렉토리 변경
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN); // ansDir로 현재 디렉토리 저장 <표준 답지 디렉토리 저장>
	cpy = (char*)malloc(sizeof(char)*BUFLEN);	
	strcpy(cpy,ansDir);

	chdir(saved_path); // 원래 디렉토리로 작업 공간 변경

	set_scoreTable(ansDir); // 점수 초기화 테이블
	set_idTable(stuDir); // 학생 id 초기화 테이블
	
	if(mOption){
		do_mOption(cpy);
	}
	printf("grading student's test papers..\n");
	score_students();

	if (iOption) // i옵션일 경우 do_iOption 함수 호출
		do_iOption(cIDs);

	return;
}

int check_option(int argc, char* argv[])
{
	int i, j;
	int c;

	while ((c = getopt(argc, argv, "e:thmi")) != -1) // <수정> argv에 옵션 인자가 존재하는지 확인 (c옵션과 p옵션이 없기 때문)
	{
		switch (c) {
		case 'e':
			eOption = true;
			strcpy(errorDir, optarg); // errorDir에 error 복사

			if (access(errorDir, F_OK) < 0) // 파일 존재여부 체크
				mkdir(errorDir, 0755);  // errorDir 디렉토리 생성
			else {
				rmdirs(errorDir); // errorDir 디렉토리 삭제
				mkdir(errorDir, 0755);
			}
			break;
		case 't':
			tOption = true;
			i = optind;
			j = 0;

			while (i < argc && argv[i][0] != '-') {

				if (j >= ARGNUM) // ARGNUM = 5임. 인자가 5개를 넘으면 안되기 때문
					printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
				else
					strcpy(threadFiles[j], argv[i]); // t옵션 뒤에오는 인자들은 threadFiles에 순차적으로 복사
				i++;
				j++;
			}
			break;
		case 'm': // <수정> p옵션은 없기 때문임
			mOption = true;
			break;
		case 'i': // <수정> c옵션은 없기 때문임
			iOption = true;
			i = optind;
			j = 0;

			while (i < argc && argv[i][0] != '-') { // '-'은 다음 옵션을 읽어들이는 것을 방지하기 위한 작업이다.

				if (j >= ARGNUM) // 인자가 5개 넘는지 확인
					printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
				else
					strcpy(cIDs[j], argv[i]); //clDs에 i옵션 뒤에 나오는 인자들 순차적으로 복사
				i++;
				j++;
			}
			break;
		case '?':
			printf("Unkown option %c\n", optopt);
			return false;
		}
	}

	return true; // 옵션없이 실행하는 경우
}

void do_mOption(char *cpy)//m옵션 추가
{
	char textnum[30];
	int isTrue;
	int num = sizeof(score_table)/sizeof(score_table[0]);
	int i;
	int searchnum;
	double newscore;
	char filename[FILELEN];
	char name[FILELEN];
	while(1){
		do{ // 존재하지 않는 텍스트 번호를 검색할 시 재입력
			printf("Input question's number to modify >> ");
			scanf("%s",textnum);
			if(!strncmp(textnum,"no",2)){ // no 입력 시 문제 점수 수정을 종료함
				break;
			}
			isTrue = false;
			for(i = 0; i < num ; i++){
				if(score_table[i].score == 0){// 초기화 하지 않은 메모리를 읽어 세그멘테이션 에러를 유도하지 않기 위한 조건문
					break;
				}
				memset(name,0,sizeof(name));
				memcpy(name,score_table[i].qname,strlen(score_table[i].qname)-strlen(strrchr(score_table[i].qname,'.')));	
				if(!strcmp(name,textnum)){ // 사용자가 입력한 값이 실존하는 문제 번호이어야만 프로그램이 정상 동작함
					isTrue = true;
					searchnum = i;
					break;
				}
			}
			
			if(!isTrue){ // 존재하지 않는 문제 번호를 입력하면 재입력 요구
				printf("%s is not found\n",textnum);	
			}
		}while(!isTrue);
		if(!strncmp(textnum ,"no",2)){
			break;
		}
			
		printf("Current score : %lf\n",score_table[searchnum].score);
		printf("New score : "); // 새로운 점수 부여
		scanf("%lf",&newscore);
		score_table[searchnum].score = newscore;

		sprintf(filename, "%s/%s",cpy,"score_table.csv");
		write_scoreTable(filename); // 수정한 점수를 score_table.csv에 다시 쓰게 됨
	}
	return;
}

void do_iOption(char(*ids)[FILELEN]) // <수정> c옵션 기능 -> i옵션 기능
{ 
	FILE* fp;
	FILE* fp2;
	FILE* fp3;
	int i = 0; // 틀린 문제를 찾기위한 변수
	int cnt=0; 
	char tmp[BUFLEN];
	char tmp2[BUFLEN];
	char tmp3[BUFLEN];
	char* p;
	char* p2;
	char* p3;
	char* saved[100];
	int wrongnum[5];

	if ((fp = fopen("score.csv", "r")) == NULL) { //socre.csv 파일을 읽을 수 없을 경우(없을 경우) 리턴 <점수 비교를 위한 파일 오픈>
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}
	if ((fp2 = fopen("score.csv", "r")) == NULL) { //socre.csv 파일을 읽을 수 없을 경우(없을 경우) 리턴 <각 학번마다 틀린 갯수를 파악하기 위한 파일 오픈>
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}
	if ((fp3 = fopen("score.csv", "r")) == NULL) { //socre.csv 파일을 읽을 수 없을 경우(없을 경우) 리턴 <모든 문제의 '번호.파일'을 saved에 저장하기 위한 파일 오픈 >
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}
	fscanf(fp3, "%s'n",tmp3); //score.csv -> tmp3는 문제 번호를 저장하기 위한 변수
	p3 = strtok(tmp3,","); // p3의 값(문제 번호)은 saved에 저장
	saved[i++] = p3;
	while((p3 = strtok(NULL,","))!=NULL){
		saved[i++] = p3;
	}
	fclose(fp3); // 더 이상 fp3는 사용하지 않음으로 파일 닫음

	fscanf(fp2, "%s'n",tmp2); //score.csv -> tmp2는 문제 번호를 저장하기 위한 변수
	while (fscanf(fp2, "%s\n", tmp2) != EOF)
	{
		p2 = strtok(tmp2, ",");

		if (!is_exist(ids, tmp2))//가변 인자로 받지 않은 것은 아래 코드를 실행하지 않음.
			continue;
	
		i=0;
		while ((p2 = strtok(NULL, ",")) != NULL){ // NULL값을 반환하기 직전의 값이 sum값임.
			if(atoi(p2) == 0){
				i++;
			}
		}
		wrongnum[cnt++]=i; // 필터링된 학번이 틀린 문제의 갯수를 저장하게 됨. 이 과정을 수행하게 된 이유는 마지막 틀린 문제에 ','를 쓰지 않기 위함임. ex) 1-2.txt, 1-3.txt, 가 아닌 1-1.txt, 1-3.txt 가 출력 되야함(명세서 출력 내용과 동일하게 하기 위한 과정)
	}
	fclose(fp2); //문제 번호를 모두 저장하였으므로 fp2 파일을 닫음

	cnt=0;
	fscanf(fp, "%s\n", tmp); //score.csv -> tmp는 점수를 비교하기 위한 변수
	while (fscanf(fp, "%s\n", tmp) != EOF)
	{
		p = strtok(tmp, ",");

		if (!is_exist(ids, tmp))//가변 인자로 받지 않은 것은 아래 코드를 실행하지 않음.
			continue;
	
		i=0;
		printf("%s's wrong answer : \n", tmp);//tmp에는 score.csv에 있는 학번이 들어가있음.
		while ((p = strtok(NULL, ",")) != NULL){ // NULL값을 반환하기 직전의 값이 sum값임.
			if(atoi(p) == 0){
				wrongnum[cnt]--;
				if(wrongnum[cnt]!=0){
					printf("%s, ",saved[i]);
				}
				else{
					printf("%s",saved[i]);
				}
			}
			i++;
		}
		cnt++;
		printf("\n");
	}
	fclose(fp);
}

int is_exist(char(*src)[FILELEN], char* target)//인자가 5개를 넘어가지 않아야하며 타겟이 존재해야 true리턴
{
	int i = 0;

	while (1)
	{
		if (i >= ARGNUM)
			return false;
		else if (!strcmp(src[i], ""))
			return false;
		else if (!strcmp(src[i++], target))
			return true;
	}
	return false;
}

void set_scoreTable(char* ansDir)
{
	char filename[FILELEN];

	// filename에 ANS_DIR/score_table.csv 를 저장
	sprintf(filename, "%s/%s", ansDir, "score_table.csv"); 

	if (access(filename, F_OK) == 0) // ansDir/score_table.csv 파일이 있는지 확인
		read_scoreTable(filename); // 파일이 존재한다면 읽기
	else { // 파일이 존재하지 않을 때
		make_scoreTable(ansDir);
		write_scoreTable(filename);
	}
}

void read_scoreTable(char* path)//score_table.csv파일 읽기
{
	FILE* fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;

	if ((fp = fopen(path, "r")) == NULL) {
		fprintf(stderr, "file open error for %s\n", path);
		return;
	}

	while (fscanf(fp, "%[^,],%s\n", qname, score) != EOF) {//파일이름과 점수 세팅
		strcpy(score_table[idx].qname, qname);
		score_table[idx++].score = atof(score); // 문자열을 float 현식의 값으로 계산하여 반환
	}

	fclose(fp);
}

void make_scoreTable(char* ansDir)//score_table.csv 파일 만들기
{
	int type, num;
	double score, bscore, pscore;
	struct dirent* dirp, * c_dirp;
	DIR* dp;
	char tmp[BUFLEN];
	int idx = 0;
	int i;

	num = get_create_type(); // 점수 테이블을 타입 1로 할지, 타입 2로 할지 결정해주는 함수 호출

	if (num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore); // 빈칸 문제 점수
		printf("Input value of program question : ");
		scanf("%lf", &pscore); // 프로그램 문제 점수
	}

	if ((dp = opendir(ansDir)) == NULL) { // ansDir의 스트림 포인터를 반환하며 이는 해당 디렉토리의 첫번째 요소의 위치를 가리킴
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}

	while ((dirp = readdir(dp)) != NULL) // <수정> ANS_DIR 내부에 서브 디렉토리가 더 이상 존재하지 않아 코드 수정
	{
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) // 상위 디렉토리와 자신의 디렉토리일 경우 반복문으로 다시 돌아감
			continue;

		if ((type = get_file_type(dirp->d_name)) < 0) // file의 타입을 확인 할수 없다면 반복문으로 다시 돌아감
			continue;

		strcpy(score_table[idx++].qname, dirp->d_name); // score_table[].qname에 파일 이름 저장
		/*sprintf(tmp, "%s/%s", ansDir, dirp->d_name); // tmp¿¡ ansDir/d_name ÆÄÀÏ °æ·Î ÀúÀå

		if ((c_dp = opendir(tmp)) == NULL) {
			fprintf(stderr, "open dir error for %s\n", tmp);
			return;
		}

		while ((c_dirp = readdir(c_dp)) != NULL)
		{
			if (!strcmp(c_dirp->d_name, ".") || !strcmp(c_dirp->d_name, ".."))
				continue;

			if ((type = get_file_type(c_dirp->d_name)) < 0)
				continue;

			strcpy(score_table[idx++].qname, c_dirp->d_name);
		}*/

		//closedir(c_dp);
		//<수정> 서브 디렉토리가 존재하지 않아 필요없는 코드
	}

	closedir(dp);
	sort_scoreTable(idx); //스코어 테이블 정렬

	for (i = 0; i < idx; i++)
	{
		type = get_file_type(score_table[i].qname); //파일의 타입 확인

		if (num == 1) // 빈칸 문제 점수 통일  & 프로그램 문제 점수 통일
		{
			if (type == TEXTFILE)
				score = bscore;
			else if (type == CFILE)
				score = pscore;
		}
		else if (num == 2) // 각 문제마다 점수 부여
		{
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		score_table[i].score = score;
	}
}

void write_scoreTable(char* filename) // score_table.csv파일에 쓰기, filename은 ANS_DIR에 존재하는 .csv파일임
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]); // num = 총 문제 개수 저장

	if ((fd = creat(filename, 0666)) < 0) {
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	for (i = 0; i < num; i++)
	{
		if (score_table[i].score == 0)
			break;

		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		write(fd, tmp, strlen(tmp)); // 문제 번호와 문제 번호에 할달된 점수 쓰기
	}

	close(fd);
}


void set_idTable(char* stuDir)//id(학번) 세팅
{
	struct stat statbuf;
	struct dirent* dirp;
	DIR* dp;
	char tmp[BUFLEN];
	int num = 0;

	if ((dp = opendir(stuDir)) == NULL) { // STD_DIR 디렉토리 오픈
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while ((dirp = readdir(dp)) != NULL) {
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) // 상위 디렉토리와 자기자신 디렉토리는 확인할 필요 없으므로 반복문으로 회귀
			continue;

		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		stat(tmp, &statbuf);

		if (S_ISDIR(statbuf.st_mode)) // 디렉토리라는 것이 판단되면 id_table에 학번 저장
			strcpy(id_table[num++], dirp->d_name);
		else
			continue;
	}

	sort_idTable(num); //학번 디렉토리 정렬
}

void sort_idTable(int size) //학생 아이디 테이블 순차적으로 정렬
{
	int i, j;
	char tmp[10];

	for (i = 0; i < size - 1; i++) {
		for (j = 0; j < size - 1 - i; j++) { // tmp를 통해 순서를 정렬
			if (strcmp(id_table[j], id_table[j + 1]) > 0) {
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j + 1]);
				strcpy(id_table[j + 1], tmp);
			}
		}
	}
}

void sort_scoreTable(int size) //스코어 테이블을 정렬하는 함수
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	for (i = 0; i < size - 1; i++) {
		for (j = 0; j < size - 1 - i; j++) {

			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j + 1].qname, &num2_1, &num2_2);


			if ((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))) { // tmp를 통해 순서를 정렬

				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j + 1], sizeof(score_table[0]));
				memcpy(&score_table[j + 1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

void get_qname_number(char* qname, int* num1, int* num2)//1-1.txt와 1-2.txt 같은 파일들의 숫자를 얻어 오기 위한 함수
{
	char* p;
	char dup[FILELEN];

	strncpy(dup, qname, strlen(qname));
	*num1 = atoi(strtok(dup, "-."));//1-1.txt와 같은 파일을 대상으로 함

	p = strtok(NULL, "-.");
	if (p == NULL)
		* num2 = 0;
	else
		*num2 = atoi(p);//p를 정수로 반환
}

int get_create_type() // csv 파일이 존재하지 않을 때 호출
{
	int num;

	while (1)
	{
		printf("score_table.csv file doesn't exist!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		if (num != 1 && num != 2)
			printf("not correct number!\n");
		else
			break;
	}

	return num;
}

void score_students()//학생들의 점수를 출력 및 얻어오는 함수
{
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]); // 학번(id) 갯수

	if ((fd = creat("score.csv", 0666)) < 0) {
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd); // 학생들의 점수가 쓰여질 .csv파일에 첫번째줄 만들기
	

	for (num = 0; num < size; num++)
	{
		if (!strcmp(id_table[num], ""))
			break;

		sprintf(tmp, "%s,", id_table[num]);
		write(fd, tmp, strlen(tmp));

		score += score_student(fd, id_table[num]);
	}

	printf("Total average : %.2f\n", score / num);//옵션 flag 삭제 -> 명세서에서 요구하는 출력형태로 바꾸기 위해 조건문 삭제

	close(fd);
}

double score_student(int fd, char* id) // 각 학번마다 점수 세팅
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]); // 총 문제 갯수
	for (i = 0; i < size; i++)
	{
		if (score_table[i].score == 0)
			break;

		// 예시 > STD_DIR/20200001/1-1.txt
		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname);
		

		if (access(tmp, F_OK) < 0){ //파일 존재여부
			result = false; 
		}
		else
		{
			if ((type = get_file_type(score_table[i].qname)) < 0){ // 파일 타입 가져오기
				continue;
			}

			if (type == TEXTFILE){
				result = score_blank(id, score_table[i].qname); // .txt파일일 경우
			}
			else if (type == CFILE)
				result = score_program(id, score_table[i].qname);// .c파일일 경우
		}

		if (result == false)//result 값이 false라면 0점 처리
			write(fd, "0,", 2);
		else {
			if (result == true) { // 참이라면 감점없이 점수 채점
				score += score_table[i].score;
				sprintf(tmp, "%.2f,", score_table[i].score);
			}
			else if (result < 0) {//값이 음수라면 감점
				score = score + score_table[i].score + result;
				sprintf(tmp, "%.2f,", score_table[i].score + result);
			}
			write(fd, tmp, strlen(tmp));// 점수 세팅
		}
	}

	//명세서에서 요구하는 출력을 하기 위해 조건문 삭제
	printf("%s is finished. score : %.2f\n", id, score);

	sprintf(tmp, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));

	return score;
}

void write_first_row(int fd) // .csv파일 첫번째 줄 만들기(문제 번호 + 합산 점수)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]); // 총 문제 갯수

	write(fd, ",", 1); // 1행 1열은 공백이기 때문

	for (i = 0; i < size; i++) {
		if (score_table[i].score == 0)
			break;

		sprintf(tmp, "%s,", score_table[i].qname);
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4);
}

char* get_answer(int fd, char* result)//학생 및 표준 답지의 내용물을 얻어오는 함수
{
	char c;
	int idx = 0;

	memset(result, 0, BUFLEN);
	while (read(fd, &c, 1) > 0)
	{
		if (c == ':') // 콜론(:)일 경우 반복문 종료, 표준 답지가 이 조건문에 걸림
			break;

		result[idx++] = c; // result에 문자 한개 씩 답지 저장
	}
	if (result[strlen(result) - 1] == '\n') //개행이 있다면 \0으로 change
		result[strlen(result) - 1] = '\0';

	return result;
}

int score_blank(char* id, char* filename)//빈칸 문제 점수를 얻어오는 함수
{
	char tokens[TOKEN_CNT][MINLEN];
	node* std_root = NULL, * ans_root = NULL;
	int idx, start;
	char tmp[BUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans; 
	int result = true;
	int has_semicolon = false;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));// .txt 제거

	sprintf(tmp, "%s/%s/%s", stuDir, id, filename); // tmp == stuDir/id/filename (ex) STD_DIR/20200001/1-1.txt
	fd_std = open(tmp, O_RDONLY);
	strcpy(s_answer, get_answer(fd_std, s_answer));//get_answer은 파일의 정답지를 가져옴

	if (!strcmp(s_answer, "")) { // 빈칸으로 제출할 시 틀린 문제
		close(fd_std);
		return false;
	}

	if (!check_brackets(s_answer)) { // '('와 ')'의 갯수가 일치하지 않으면 틀린 문제
		close(fd_std);
		return false;
	}

	strcpy(s_answer, ltrim(rtrim(s_answer))); // 왼쪽, 오른쪽 공백 제거

	if (s_answer[strlen(s_answer) - 1] == ';') { // 세미콜론이 있는지 확인
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0'; // 세미콜론 자리에 \0으로 대체
	}

	// make_tokens()함수 기능의 예시
	// ex) creat(fname, O_RDONLY)
	// tokens[0] = "creat", tokens[1] = "(", tokens[2] = "fname", tokens[3] = ",", tokens[4] = "O_RDONLY", tokens[5] = ")" 로 나뉘어 짐
	if (!make_tokens(s_answer, tokens)) { //학생 답안지를 토큰으로 만듬 
		close(fd_std);
		return false;
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0);

	sprintf(tmp, "%s/%s", ansDir, filename); // <수정> ANS_DIR에는 서브 디렉토리가 존재하지 않음
	fd_ans = open(tmp, O_RDONLY);

	while (1) //복수 정답이 있을 수 있기 때문에 반복문을 돌림
	{
		ans_root = NULL;
		result = true;

		for (idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		strcpy(a_answer, get_answer(fd_ans, a_answer)); // get_answer은 답지 가져오는 함수

		if (!strcmp(a_answer, "")) // 답지가 비어 있다면 반복문 종료
			break;

		strcpy(a_answer, ltrim(rtrim(a_answer)));//좌우측 공백 제거

		if (has_semicolon == false) {//세미콜론이 없었다면 
			if (a_answer[strlen(a_answer) - 1] == ';')//끝자리에 세미콜론이 없었다면
				continue;
		}

		else if (has_semicolon == true)//세미콜론이 있었다면
		{
			if (a_answer[strlen(a_answer) - 1] != ';')//끝자리에 세미콜론이 없다면
				continue;
			else//끝자리에 세미콜론이 있다면
				a_answer[strlen(a_answer) - 1] = '\0'; //세미콜론 자리에 \0 대체
		}

		if (!make_tokens(a_answer, tokens)) //토큰이 만들어 지지 않으면
			continue;

		idx = 0;
		ans_root = make_tree(ans_root, tokens, &idx, 0);//토큰의 값을 토대로 노드를 통해 트리를 구성

		compare_tree(std_root, ans_root, &result);//학생루트, 답지루트를 비교

		if (result == true) {//비교를 통해 true라면 점수부여
			close(fd_std);
			close(fd_ans);

			if (std_root != NULL)
				free_node(std_root);// 학생 노드 해제
			if (ans_root != NULL)
				free_node(ans_root);// 답지 노드 해제
			return true;

		}
	}

	close(fd_std);
	close(fd_ans);

	if (std_root != NULL)
		free_node(std_root);//노드 해제
	if (ans_root != NULL)
		free_node(ans_root);//노드 해제

	return false;//위 반복문에서 true를 반환하지 못했으므로 false 리턴
}

double score_program(char* id, char* filename)//프로그램 파일 채점,.c파일인 경우 호출
{
	double compile;
	int result;

	compile = compile_program(id, filename);//컴파일 결과 compile에 저장

	if (compile == ERROR || compile == false)//에러 또는 false라면 0점임
		return false;

	result = execute_program(id, filename);//프로그램 실행

	if (!result)//result가 false라면 false리턴
		return false;

	if (compile < 0)//compile 값을 리턴. 이 값은 채점 점수에 영향을 미침 (ex)warning 2회 -> 0.2 감점
		return compile;

	return true;//어떠한 오류도 없을 시에 true 리턴
}

int is_thread(char* qname)//스레드 체크, qname은 -lpthread를 적용시킨 문제번호에 해당함
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);

	for (i = 0; i < size; i++) {
		if (!strcmp(threadFiles[i], qname))
			return true;
	}
	return false;
}

double compile_program(char* id, char* filename)//프로그램 문제 컴파일
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	memset(qname, 0, sizeof(qname));
	//10.c -> 10 변경함
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	isthread = is_thread(qname);//-t를 통해 lpthread를 추가시켜 줄 것인지에 대한 판단여부

	sprintf(tmp_f, "%s/%s", ansDir, filename);//tmp_f에는 정답지 파일 경로가 들어감
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);//tmp_e에는 정답지 파일 경로에 .exe파일을 넣어줄 것임

	if (tOption && isthread)//t옵션과 isthread가 참이어여만 동작
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);//에러 텍스트파일이 저장된 변수
	fd = creat(tmp_e, 0666);//에러 텍스트 파일 생성

	redirection(command, fd, STDERR);//에러 텍스트 메시지를 fd가 가리키는 파일에 넣기
	size = lseek(fd, 0, SEEK_END); //파일 사이즈 얻기
	close(fd);
	unlink(tmp_e);//answer 에러 파일이기에 지워줌

	if (size > 0)//사이즈가 0을 넘는다면 에러가 출력됐다는 것이므로 false를 리턴
		return false;

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	if (tOption && isthread)//t옵션과 isthread 값이 참이라면 -lpthread 추가
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else //기본 명령 옵션
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);//에러 텍스트 파일 만들기

	redirection(command, fd, STDERR);//에러 텍스트 파일에 에러 출력문 쓰기
	size = lseek(fd, 0, SEEK_END);//에러 텍스트 파일의 크기 얻기
	close(fd);

	if (size > 0) {//사이즈가 0을 넘는다면 에러 또는 warning이 발생했다는 뜻임
		if (eOption)//eOption이 참이라면 error 디렉토리를 생성해야함
		{
			sprintf(tmp_e, "%s/%s", errorDir, id);//(ex)error/20200001
			if (access(tmp_e, F_OK) < 0)//파일이 존재하지 않는다면 디렉토리 만들기
				mkdir(tmp_e, 0755);

			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			rename(tmp_f, tmp_e);//tmp_f에서 tmp_e로 rename 지정

			result = check_error_warning(tmp_e);//warning check 함수
		}
		else {//거짓이라면 error 디렉토리는 생성하지 않아도 됨
			result = check_error_warning(tmp_f);
			unlink(tmp_f);
		}

		return result;
	}

	unlink(tmp_f);
	return true;
}

double check_error_warning(char* filename)//warning체크를 통해 감점 요인 파악
{
	FILE* fp;
	char tmp[BUFLEN];
	double warning = 0;

	if ((fp = fopen(filename, "r")) == NULL) {//fopen에러 처리
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	while (fscanf(fp, "%s", tmp) > 0) { // fp -> tmp
		if (!strcmp(tmp, "error:"))//error: 구문을 만난다면 ERROR 반환
			return ERROR;
		else if (!strcmp(tmp, "warning:"))//warning: 구문을 만난다면 -0.1점 누적
			warning += WARNING;
	}

	return warning;//감점 점수 반환
}

int execute_program(char* id, char* filename)//프로그램 실행을 통해 .stdout파일, .exe파일, .stdexe 파일 만들기
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));//,c를 제거하여 문제 번호만 qname에 넣어줌

	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666);

	sprintf(tmp, "%s/%s.exe", ansDir, qname);
	redirection(tmp, fd, STDOUT);//.stdout에 file descriptor(STDOUT) 내용 넣기
	close(fd);

	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666);

	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname);

	start = time(NULL);// 시간 측정 변수
	redirection(tmp, fd, STDOUT);//학생이 제출한 프로그램, .stdexe파일 만들기

	sprintf(tmp, "%s.stdexe", qname);
	while ((pid = inBackground(tmp)) > 0) {//
		end = time(NULL);//프로그램이 끝나는 시점을 구하기위한 변수

		if (difftime(end, start) > OVER) {//제한시간 5초를 넘어갔을 때 종료시키고 false를 리턴하는 부분
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname);//결과 파일을 비교해주는 함수
}

pid_t inBackground(char* name)//채점 프로그램이 실행되는 동안 정답파일을 실행하는 프로그램이 백그라운드로 실행되는 것을 캐치해주는 함수,name == .stdexe파일임
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;

	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);//background.txt 생성

	sprintf(command, "ps | grep %s", name);//백그라운드로 실행되는 프로그램을 캐치할 수 있음
	redirection(command, fd, STDOUT);

	lseek(fd, 0, SEEK_SET);//오프셋 변경
	read(fd, tmp, sizeof(tmp));//fd의 내용을 tmp에 저장

	if (!strcmp(tmp, "")) {//tmp가 비어있다면
		unlink("background.txt");
		close(fd);
		return 0;
	}

	pid = atoi(strtok(tmp, " "));
	close(fd);

	unlink("background.txt");
	return pid;
}

int compare_resultfile(char* file1, char* file2)//학생 .stdout과 답지 .stdout을 비교하는 함수
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while (1)
	{
		while ((len1 = read(fd1, &c1, 1)) > 0) {//문자 1개씩 읽기
			if (c1 == ' ')
				continue;
			else
				break;
		}
		while ((len2 = read(fd2, &c2, 1)) > 0) {//문자 1개씩 읽기
			if (c2 == ' ')
				continue;
			else
				break;
		}

		if (len1 == 0 && len2 == 0)//len1과 len2의 값이 모두 0이면 true
			break;

		to_lower_case(&c1);//소문자로 변경해주는 함수
		to_lower_case(&c2);//소문자로 변경해주는 함수

		if (c1 != c2) {//서로 다른 문자가 들어오면 false
			close(fd1);
			close(fd2);
			return false;
		}
	}
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char* command, int new, int old)
{
	int saved;

	saved = dup(old); //old descriptor 복사
	dup2(new, old); //old -> new 디스크립터 복사 

	system(command);//(ex) - old가 2디스크립터였다면 new에 error 텍스트 메시지를 넣어줄 수 있음

	dup2(saved, old); // old -> saved로 원상 복귀
	close(saved);// 초기 상태로 돌려놓음
}

int get_file_type(char* filename)
{
	char* extension = strrchr(filename, '.'); // .txt 인지 .c인지 확인

	if (!strcmp(extension, ".txt"))
		return TEXTFILE;
	else if (!strcmp(extension, ".c"))
		return CFILE;
	else
		return -1;
}

void rmdirs(const char* path)//기존 error디렉토리 폴더를 삭제해주는 함수
{
	struct dirent* dirp;
	struct stat statbuf;
	DIR* dp;
	char tmp[50];

	if ((dp = opendir(path)) == NULL)
		return;

	while ((dirp = readdir(dp)) != NULL)
	{
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))//부모 디렉토리와 자기자신 디렉토리는 반복문 비교에서 제외 됨
			continue;

		sprintf(tmp, "%s/%s", path, dirp->d_name);//error디렉토리 내에 있는 파일 tmp에 할당

		if (lstat(tmp, &statbuf) == -1)
			continue;

		if (S_ISDIR(statbuf.st_mode))//tmp가 디렉토리라면
			rmdirs(tmp);//삭제
		else
			unlink(tmp);//링크 풀기
	}

	closedir(dp);
	rmdir(path);//재귀호출을 통해 재검색
}

void to_lower_case(char* c)//소문자로 바꾸어 주는 함수
{
	if (*c >= 'A' && *c <= 'Z')
		* c = *c + 32;
}

void print_usage() // h옵션 내용을 명세서에 맞게 수정
{
	printf("Usage : ssu_score <STD_DIR> <ANS_DIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -m                        modify question's score\n");
	printf(" -e <DIRNAME>              print error on 'DIRNAME/ID/qname_error.txt' file \n");
	printf(" -t <QNAMES>               compile QNAME.c with -lpthread option\n");
	printf(" -i <IDS>                  print ID's wrong questions\n");
	printf(" -h                        print usage\n");
}
