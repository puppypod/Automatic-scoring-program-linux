# 숭실대학교 _ 리눅스 시스템 프로그래밍 개인프로젝트 P1 _ 자동채점프로그램

## 설계 및 구현

- ssu_score <STD_DIR> <ANS_DIR> [OPTION]
    1. <STD_DIR> : ( 채점 대상이 될 학생이 제출한 답안 디렉토리로 학생들 학번 서브디렉토리를 포함 최
대 개 100 )

    2. <ANS_DIR> : 채점 기준이 될 정답이 있는 디렉토리로 하나의 문제당 하나의 서브디렉토리를 포함하고 각 문제 서브디렉토리에는 빈칸 채우기 문제의 경우 정답 리스트 다수 ( 의 답이 있을 경우 콜론으로 구분 프로그램 문제 ), ( ( .c), ( .exe), 의 경우 정답 소스 프로그램 문제번호 정답 실행프로그램 문제번호정답 실행프로그래밍 실행결과 문제번호 파일 포함

        - 출제 시험 문제 형태 정답 파일 학생 답안 파일
            
            - 출제되는 시험 문제는 (1) (2) 빈칸 채우기 문제 와 프로그램 작성 문제 총 두 가지 종류이며 학생들이 출제되는 시험문제에 대한 답을 작성하여 자신의 학번 디렉토리 안에 모든 문제에 대한 답안 파일을 넣어 제출함

            -  빈칸 채우기 문제의 답안 파일 형태는 문제번호 이 .txt ( . 5.txt), 며 예 프로그램 작성 문제의 답안 파일 형태는 문제번호 에 .c ( . 10.c)임

            -  단 하나 , 의 빈칸 채우기 문제는 여러 개의 서브 문제로 구성될 수 있으며 각 학생들은 한 문제가 여러 개의 하위 문제들로 구성될 수 있음 예( . 1-1.txt, 1-2.txt)

            -  정답 파일도 학생 답안과 동일하게 하나의 디렉토리 안에 문제번호 와 문제번호 로 구성
            
            -  출제되는 문제 수는 최대 문제 100 까지 허용
        
**빈칸 채우기 문제**

![1](https://user-images.githubusercontent.com/47939832/112276375-0167f180-8cc4-11eb-996f-96702784a027.png)

![2](https://user-images.githubusercontent.com/47939832/112276377-02991e80-8cc4-11eb-993b-58f5ec31b54c.png)
    
**프로그램 문제**
 
 ![3](https://user-images.githubusercontent.com/47939832/112276378-02991e80-8cc4-11eb-8e59-a9f9b2c8d98c.png)
 
 ![4](https://user-images.githubusercontent.com/47939832/112276379-0331b500-8cc4-11eb-890e-5d6527f297e7.png)
    
**빈칸 문제 채점 방법**
    
    - 학생들이 제출한 답안 디렉토리의 파일들과 정답 파일 txt txt 의 파일을 비교하면서 채점하고 맞으면주어진 정답 파일에서 주어진 점수를 부여하고 틀리면 점 부여

    - 포인터 배열 캐, 스팅 연산처럼 다수의 답이 정답이 될 경우 정답파일에 ‘콜론(:)’으로 구분하며, 학생들이 작성한 답안 파일을 비교할 때 다수의 답을 모두 정답으로 채점 정답 파일에 새로 -> 운 답을 추가 및 삭제를 통해 학생들의 답안을 채점

   ![5](https://user-images.githubusercontent.com/47939832/112276908-8eab4600-8cc4-11eb-9bb1-004dfd87d5c8.png)
    
    - 연산자 사용으로 인해 다수의 답이 정답이 되는 경우 자동으로 동일한 연산식으로 자동으로 채점
    - 단, 피연산자들의 순서가 바뀌었을 때 결과 값이 달라지는 문제는 지원하지 않음
    - 연산자 우선순위를 기반으로 한 괄호도 정답으로 처리
    - 문자열 간 공백은 구분하지 않음 컴파일 시 에러가 나지 않는 경우 공 -> 백은 정답 처리
    
   ![6](https://user-images.githubusercontent.com/47939832/112276911-8f43dc80-8cc4-11eb-8ed1-f53b0af87c33.png)
    
**프로그램 문제 채점 방법**
    
    - 학생이 작성해서 답안으로 제출한 프로그램을 컴파일 후 실행파일은 학번 문제번호 “./STD/ / .stdexe“이름으로 저장 그리고 정답 프로그램의 실행 파일은 문제번호 이름으로 생성 ”./ANS/ .exe”
    - 각 문제에 대한 정답 프로그램인 문제번호 “./ANS/ .c” ” 의 실행 파일인 문제번호 를 실행시 ./ANS/ .exe”켜 실행결과를 문제번호 으로 “./ANS/ .stdout” 저장
    - 학생이 작성해서 답안으로 제출한 프로그램의 실행파일인 학번 문제번호 을 자 “./STD/ / .stdexe“ 동으로 실행시키고 실행결과를 학번 문제번호 자 “./STD/ / .stdout“ 동으로 저장
    - “./ANS/ .stdout 문제번호 를 학번 디렉토리를 ” “ 순회하면서 학번 문제번호 과 ./STD/ / .stdout” 비교하여 결과만 우선 채점
    - 학생들이 제출한 프로그램의 실행 결과 채점 시 실행결과의 대소문자와 공백을 구분하지 않음
    - 학생들이 제출한 프로그램의 실행 결과 채점 시 이 warning 0.1 , error 0 (#define 처리 가 하나라도 있는 경우 발생한 문제는 점 으로 정의 변경이 용이하게) 처리
    - 학생들이 제출한 프로그램의 실행이 초 이상 5 0 (#define 의 시간이 걸리는 프로그램은 점 으로 정의 –변경이 용이하게) 처리함
    - 학생들이 제출한 프로그램의 실행결과 파일의 크기에 상관없이 채점 가능해야 함
    
    
**점수 테이블 생성**
    
    - 점수 테이블 파일은 이름으로 생성해야 하 “./score_table.csv” , 며 문제번호와 점수를 저장
    - 점수 테이블 파일이 현“ ” 재 실행 위치 에 존재해야 하며 존재 , 하지 않은 경우 이름으로 점수 테이블 파일을 새로 생성하고, 사용자가 문제번호와 점수를 설정하고 이를 저장할 수 있어야 함
    
   ![7](https://user-images.githubusercontent.com/47939832/112277579-417ba400-8cc5-11eb-903f-7385a40494cf.png)
    
    
**채점 결과 테이블 생성**
    
    - 학생들의 성적 테이블은 프로그램이 ssu_score 종료하면 이름으로 자 “./score.csv” 동으로 생성되어야 하며, (1) (2) (3) 학번 문제번호당 채점 점수 총점을 저장하여 전체 학생들의 각 문제당 점수와 전체 점수를 알 수 있음

   ![8](https://user-images.githubusercontent.com/47939832/112277581-42143a80-8cc5-11eb-93c4-59f1ec297af1.png)
    
    
**옵션**
    
    - 옵션 없이 실행하는 경우
    
   ![9](https://user-images.githubusercontent.com/47939832/112277583-42acd100-8cc5-11eb-91ad-f2bcd0cd1920.png)
    
    - -m : 옵션 채점 전에 원하는 문제의 점수를 수정
    
   ![10](https://user-images.githubusercontent.com/47939832/112277584-42acd100-8cc5-11eb-9ca8-df0b2e73f0f2.png)
    
    - -e [DIRNAME] : DIRNAME/ / _error.txt 옵션 학번 문제번호 에 에러 메시지가 출력
    
   ![11](https://user-images.githubusercontent.com/47939832/112277590-43456780-8cc5-11eb-8de8-b4fd506392e1.png)
   
    - -t [QNAMES] : QNAME lpthread 옵션 을 문제 번호로 하는 문제는 컴파일 시 – 옵션 추가
    
   ![12](https://user-images.githubusercontent.com/47939832/112277593-43ddfe00-8cc5-11eb-8067-f92f5ec3efda.png)
   
   ![13](https://user-images.githubusercontent.com/47939832/112277597-43ddfe00-8cc5-11eb-909a-700c37a742ec.png)
   
    - -i [STUDENTIDS] : => 옵션 채점결과 파일이 있는 경우 해당 학생들의 틀린 문제 파일 출력 <STUDENTDIR> <TRUESETDIR>가 없어도 사용 가능
   
   ![14](https://user-images.githubusercontent.com/47939832/112277598-44769480-8cc5-11eb-8106-11e55456f8ce.png)
   
    - -h : 옵션 사용법 출력
    
   ![15](https://user-images.githubusercontent.com/47939832/112277603-44769480-8cc5-11eb-9547-162b6e46b860.png)

    
**기타**
         
    - 모든 옵션은 동시에 쓸 수 있음. -h ssu_score 옵션도 동시에 쓸 수 있으나 사용법을 출력 후 가 종료
    
    - 가변인자를 받는 옵션의 경우, 5 . 최대로 받을 수 있는 가변인자의 개수는 개로 제한 그 이상의 가변 인자를 받으면 실행과 같은 메시지를 출력하고 수행에는 반영하지 않도록 구현
    
   ![16](https://user-images.githubusercontent.com/47939832/112277605-450f2b00-8cc5-11eb-99a8-88664c94bcb1.png)
   
   ![17](https://user-images.githubusercontent.com/47939832/112277609-450f2b00-8cc5-11eb-999f-668ff1f0a3a1.png)
   
    - 존재하지 않는 디렉토리 혹은 파일을 대상으로 할 경우 에러 처리
    - 실행결과 및 에러 메시지를 파일에 출력 시 함수 system() 사용 금지
    - 점수 출력은 소수점 둘째자리까지 출력
    - popen(), fork(), exec() 계열 함수 사용 금지 
    - 게시판을 통해 시험문제 정답 그리고 학생 답 , . 샘플 제공함 게시판에 게시된 정답 및 학생답 이외의 경우는 고려하지 않음
    
## 소스코드 분석

- 설계
    
    ![21](https://user-images.githubusercontent.com/47939832/112281439-670aac80-8cc9-11eb-80a0-b3b2715dc328.png)
    
    ● main 함수 흐름도
        
    ![22](https://user-images.githubusercontent.com/47939832/112281447-683bd980-8cc9-11eb-8ebe-a251751815d0.png)
    
    ● -h 옵션이 들어온다면 프로그램 설명을 출력하고 종료한다.
    ● -h옵션이 없을 때 각 옵션들을 check 한다.
    ● 만약 I옵션만이 참이라면 I옵션을 수행해 주고 원하는 출력 후 종료한다.
    ● I옵션 만이 참이 아니라면, 점수 테이블과 학생 id 테이블을 세팅해 준다.
    ● m옵션의 여부를 묻는다. 만약 m옵션이 참이라면 m옵션 기능을 수행 후 다음 함수로 넘어간다.
    ● score_students() 함수가 본격적으로 시작된다.
    
    ![23](https://user-images.githubusercontent.com/47939832/112281450-696d0680-8cc9-11eb-8c94-5b8a04039d1e.png)
    
    ● 만약 score_table.csv 파일이 존재한다면 그 파일을 읽어들이는 작업만을 한다.
    ● 만약 score_table.csv 파일이 존재하지 않는다면 .csv파일을 생성하고 .csv 파일에 문제 번호 및 점수를 세팅하여 써준다.
    
    ![24](https://user-images.githubusercontent.com/47939832/112281456-6a9e3380-8cc9-11eb-9895-425b6d5f9bac.png)
    
    ● score_student() 함수는 학생들의 점수를 반환해주는 역할을 한다.
    ● 점수를 반환 받으면서 score.csv에 관련 정보를 적어주는 기능을 수행한다.
    
    ![25](https://user-images.githubusercontent.com/47939832/112281461-6b36ca00-8cc9-11eb-8753-d431a0ca9f90.png)
    
    ● 모든 문제는 .txt와 .c파일로 구성되어 있다.
    ● 때문에 .txt파일과 .c파일로 구분하여 채점을 진행해야만 한다.
    ● 만약 .txt 파일이라면 score_blank()로, .c파일이라면 score_program()으로 흐름이 넘어가야한다.
    ● score_program()함수는 컴파일 단계과, 실행 단계가 존재한다.
    ● 위 과정을 거치면서 .exe파일, .stdexe파일, stdout 파일 등이 생성되며 –t옵션과 –e옵션의 기능을 수행해주는 흐름 또한 존재한다.
    
    ![26](https://user-images.githubusercontent.com/47939832/112281467-6c67f700-8cc9-11eb-8b33-0191d669b2a6.png)
    
    ● score_blank()는 우선 학생 답지의 토큰을 나누어주고 토큰을 base로 노드 트리구조를 형성한다. 또한, 표준 답지의 토큰도 나눠지며 노드 트리구조를 형성한다.
    ● 학생 답지와 표준답지 모두 노드 트리구조가 형성됐다면 compare_tree()라는 함수가 동작하여 두 답지를 비교해주는 과정을 거치게 된다.
    ● 반복문이 종료될 때까지 일치하는 것이 없다면 flase, 일치 한다면 true를 수행한다.
    ● 그 외로 많은 return 값들이 있지만 전체 흐름도를 설명하기에 지엽적인 부분이라 흐름도에서 제외했다.
    
- 구현
    
    **<main.c>**
    
    **void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);**
    
    ▶ 프로그램이 실행되었던 시간을 측정하는 함수이다.
    
    ▶ 인자로는 struct timeval *값들이 들어가는데, ssu_score()함수를 호출하기 전과 후에 gettimeofday() 함수를 이용해 시작 시간과 끝나는 시간을 인자로 가지어 프로그램이 실행되는 시간을 측정한다.

    **<ssu_score.h>**

    **void ssu_score(int argc, char *argv[]);**

        ▶ main()함수에서 호출

        ▶ 채점 프로그램 함수

    **int check_option(int argc, char *argv[]);**

        ▶ i옵션,m옵션,h옵션,t옵션,e옵션의 여부를 판단하여 각 옵션 변수마다 true값 할당

        ▶ 반환 값으론 true or false값 리턴하여 프로그램을 종료, 또는 진행여부를 결정
    
    **void print_usage();**

        ▶ h옵션이 true일 경우 호출

        ▶ 프로그램 사용법에 대한 출력

    **void score_students();**

        ▶ 학생들의 점수를 얻어오고 총점 평균을 출력해줌

        ▶ score.csv 파일을 생성
    
    **double score_student(int fd, char *id);**
    
        ▶ 각 학번마다 점수를 세팅해 주는 함수

        ▶ 리턴값으로 각 학번마다의 채점 점수를 반환

        ▶ 각각의 id 점수를 출력해줌
    
    **void write_first_row(int fd);**
    
        ▶ score.csv 파일에 첫 번째줄을 채워주는 함수(문제번호 및 sum)

    **char *get_answer(int fd, char *result);**
    
        ▶ 학생 답지 또는 표준 답지의 텍스트 파일 내용물을 result값에 할당해주는 함수

        ▶ 반환 값으로 텍스트 내용물이 담긴 result가 반환됨
    
    **int score_blank(char *id, char *filename);**
    
        ▶ .txt 파일을 채점할 때 호출해주는 함수

        ▶ 학생 답지와 표준 답지를 각각 토큰으로 분할하고 노드 트리구조로 형성하고 트리구조 비교를 통해 정답여부 결정해 줌

        ▶ 리턴값으로 학생 채점 점수를 할당

    **double score_program(char *id, char *filename);**
    
        ▶ .c 파일을 채점할 때 호출해주는 함수

        ▶ .c파일을 컴파일 및 실행해주는 함수

        ▶ warning이 존재할 시 –0.1점씩 감점됨

        ▶ error일 경우 0점 부여

        ▶ 리턴 값으로 채점 점수가 리턴 됨

    **double compile_program(char *id, char *filename);**
    
        ▶ 프로그램 문제를 컴파일 해주는 함수

        ▶ 리턴값으론 true or false 리턴

        ▶ -t옵션 및 –e옵션을 수행해 주는 기능이 들어있음

    **int execute_program(char *id, char *filname);**
    
        ▶ .stdout파일, .exe파일, .stdexe파일을 만들어 주는 함수

        ▶ 프로그램이 진행되는 시간이 5초가 넘으면 프로그램을 강제 종료하여 0점 부여함

    **pid_t inBackground(char *name);**
    
        ▶ 채점 프로그램이 실행되는 동안 정답파일을 실행하는 프로그램이 백그라운드로 실행되는 것을 캐치해주는 함수

        ▶ 반환 값으로 proccess id 값인 pid가 리턴됨

    **double check_error_warning(char *filename);**
    
        ▶ warning체크를 통해 감점 요소를 파악해주는 함수

        ▶ 반환값으로 감점된 점수가 리턴됨

    **int compare_resultfile(char *file1, char *file2);**
    
        ▶ 학생 .stdout과 답지 .stdout을 비교하는 함수

        ▶ 반환값으로 true 또는 false가 반환되어 점수 채점 결과에 영향을 줌


    **void do_iOption(char (*ids)[FILELEN]);//c옵션 -> i옵션으로 변경**
    
        ▶ 학생들이 틀린 문제 파일을 출력해주는 함수

        ▶ 이 함수가 실행된 후에는 프로그램이 종료됨

    **void do_mOption(char *cpy);//m옵션 추가**
    
        ▶ 수정하고자 하는 점수를 다시 세팅해 주는 함수

        ▶ 이 함수가 수행된 후에는 .csv 파일들이 수정됨

    **int is_exist(char (*src)[FILELEN], char *target);**
    
        ▶ t옵션 I옵션 인자가 5개를 넘어가지 않아야하며 타켓이 존재해야 true값을 리턴


    **int is_thread(char *qname);**
    
        ▶ 스레드 체크해주는 함수이며, qname은 –lpthread를 적용시킨 문제번호에 해당

        ▶ 반환 값으로 true or false 리턴

    **void redirection(char *command, int newfd, int oldfd);**
    
        ▶ file descriptor 값을 통해(old, new) 원하는 입출력을 하기 위한 함수

        ▶ 원하는 작업을 마친 후에는 다시 원상복구 시킴

    **int get_file_type(char *filename);**
    
        ▶ .c 파일인지 .txt 파일인지 구별해주는 함수
    
    **void rmdirs(const char *path);**
    
        ▶ 기존 error디렉토리 폴더를 삭제해주는 함수
    
    **void to_lower_case(char *c);**
    
        ▶ 소문자로 바꾸어 주는 함수


    **void set_scoreTable(char *ansDir);**
    
        ▶ score_table.csv 파일을 read, write, make 해주는 기능이 있는 함수를 호출해줌
    
    **void read_scoreTable(char *path);**
    
        ▶ score_table.csv 파일이 존재할 때 호출하는 함수

        ▶ score_table.csv 파일을 읽어 파일 이름과 점수를 저장해줌

    **void make_scoreTable(char *ansDir);**
    
        ▶ score_table.csv파일이 존재하지 않을 때 호출해주는 함수

        ▶ 위 파일을 생성해줌

        ▶ 빈칸 문제 점수 및 프로그램 문제 점수를 세팅하는 함수

    **void write_scoreTable(char *filename);**
    
        ▶ score_table.csv 파일에 만들어진 파일 이름 및 문제 점수를 write 해주는 함수
    
    **void set_idTable(char *stuDir);**
    
        ▶ 학생 id 세팅해 주는 함수
    
    **int get_create_type();**
    
        ▶ .csv 파일이 존재하지 않을 때 호출해주는 함수

        ▶ 1옵션으로 할지 2옵션으로 할지 그 값을 리턴해줌


    **void sort_idTable(int size);**
    
        ▶ 학생들 id를 오름차순으로 정렬해주는 함수
    
    **void sort_scoreTable(int size);**
    
        ▶ 스코어 테이블을 정렬해주는 함수
    
    **void get_qname_number(char *qname, int *num1, int *num2);**
    
        ▶ 문제 번호를 정렬해줄 때 1-1.txt 또는 1-2.txt 라는 텍스트 파일이 올 때, 앞 숫자 뒷 숫자 등을 저장해 정렬의 소스가 될 수 있게 만들어줌

    **<blank.h>**
    
    **void compare_tree(node *root1,  node *root2, int *result);**
    
        ▶ 학생 답지와 표준 답지의 비교를 통해 정답여부를 알려주는 함수

        ▶ 리턴값은 없지만 result 값 자체가 점수 결정 여부의 요소가 됨

        ▶ 이 함수는 학생 답지와 표준 답지가 노드 트리가 형성된 이후에 호출됨

    **node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses);**
    
        ▶ str 토큰 분할 및 생성해주는 함수

        ▶ 문자와 operation을 각각 알맞게 분할하여 토큰 배열에 할당해 주는 함수

        ▶ 후에 트리구조를 형성하는데 결정적인 역할을 해줌

        ▶ root는 std_root, and_root

        ▶ tokens는 make_tokens()함수를 통해 만들어짐

        ▶ idx는 이 함수가 종료된 이후 사용되는 변수

        ▶ parentheses 초기값은 0이며 괄호를 의미

    **node *change_sibling(node *parent);**
    
        ▶ 형제 노드를 바꿔주는 기능을 수행
    
    node *create_node(char *name, int parentheses);
    
        ▶ 새로운 노드를 만들어주는 함수
        ▶ 반환값으로 새롭게 생성한 노드를 반환함
        
    int get_precedence(char *op);
    
        ▶ 연산자 상위 번호를 얻어오는 함수
        ▶ 반환값으로 number 또는 false가 리턴됨
        
    int is_operator(char *op);
    
        ▶ operator인지 판별하여 참 또는 거짓을 반환해주는 함수
        
    void print(node *cur);
    
        ▶ 노드 출력 테스트 함수
        
    node *get_operator(node *cur);
    
        ▶ operator 노드를 구해주는 함수
        ▶ 현재 노드의 부모값을 리턴해주는 함수
        
    node *get_root(node *cur);
    
        ▶ root 구하기
        ▶ 인자로 들어간 노드의 첫째 형제노드를 구한 후 그 노드의 부모 노드가 존재하면 재귀호출 함
        ▶ 즉, 상위 root를 구한다는 의미
        
    node *get_high_precedence_node(node *cur, node *new);
    
        ▶ 높은 상위 노드 구하는 함수
        ▶ 위 노드 값을 리턴함
        
    node *get_most_high_precedence_node(node *cur, node *new);
    
        ▶ 가장 높은 상위 노드(높은 상위 노드의 첫째 형제노드)를 구하는 함수
        ▶ 위 노드 값을 리턴함
        
    node *insert_node(node *old, node *new);
    
        ▶ 노드를 삽입하여 old 노드와 new 노드의 관계를 형성해주는 함수
        ▶ new 노드를 반환해줌
        
    node *get_last_child(node *cur);
    
        ▶ 마지막 자식 노드를 구하는 함수
        
    void free_node(node *cur);
    
        ▶ 노드를 해제해주는 함수
        
    int get_sibling_cnt(node *cur);
    
        ▶ 형제 노드 개수를 반환해주는 함수
        

    int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN]);
    
        ▶ 학생답지 및 표준답지의 .txt파일의 내용을 토큰으로 분할하는 함수
        ▶ 반환 값으로 true 또는 false를 해줌
        
    int is_typeStatement(char *str);
    
        ▶ type 상태 구하기
        ▶ return 0 -> 0점
        ▶ return 1 -> datatype이 존재하지 않을 경우
        ▶ return 2 -> gcc라는 명령어가 존재할 경우 또는 datatype이 존재하는 경우
        
    int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]);
    
        ▶ 타입 지정자 구하기1
        ▶ 리턴 값이 –1이라면 0점 부여
        
    int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]);
    
        ▶ 타입 지정자 구하기2
        ▶ 리턴값이 –1이라면 0점 부여
        
    int is_character(char c);
    
        ▶ ‘0’ ~ ‘9’, ‘A’ ~ ‘Z’, ‘a’ ~ ‘z’ 등의 character인지 판단해주는 함수
        ▶ true or false 리턴
        
    int all_star(char *str);
    
        ▶ 토큰 값이 모두 *인지 판단해주는 함수
        ▶ true or false 리턴
        
    int all_character(char *str);
    
        ▶ 토큰에 들어간 값이 모두 문자인지 판단하는 함수
        ▶ true or false 리턴
        
    int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]);
    
        ▶ 토큰을 깨끗하게 리셋해주는 함수
        ▶ true or false 리턴
        
    void clear_tokens(char tokens[TOKEN_CNT][MINLEN]);
    
        ▶ 토큰 모든 값을 0으로 초기화
        
    int get_token_cnt(char tokens[TOKEN_CNT][MINLEN]);
    
        ▶ 토큰의 개수를 반환해주는 함수
        
    char *rtrim(char *_str);
    
        ▶ 오른쪽 공백을 제거한 값을 리턴하는 함수
        
    char *ltrim(char *_str);
    
        ▶ 왼쪽 공백을 제거한 값을 리턴하는 함수
        
    void remove_space(char *str);
    
        ▶ 공백을 제거해주는 함수
        
    int check_brackets(char *str);
    
        ▶ ‘(’와 ‘)’의 개수가 서로 같은지 체크하는 함수
        ▶ 다르다면 false 같다면 true 리턴
        
    char* remove_extraspace(char *str);
    
        ▶ 공백제거 함수
        ▶ 제거한 값이 리턴됨
        
