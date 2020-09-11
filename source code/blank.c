#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "blank.h"

//모든 데이터 타입 세팅
char datatype[DATATYPE_SIZE][MINLEN] = {"int", "char", "double", "float", "long"
			, "short", "ushort", "FILE", "DIR","pid"
			,"key_t", "ssize_t", "mode_t", "ino_t", "dev_t"
			, "nlink_t", "uid_t", "gid_t", "time_t", "blksize_t"
			, "blkcnt_t", "pid_t", "pthread_mutex_t", "pthread_cond_t", "pthread_t"
			, "void", "size_t", "unsigned", "sigset_t", "sigjmp_buf"
			, "rlim_t", "jmp_buf", "sig_atomic_t", "clock_t", "struct"};


//operator들의 우선순위 할당
operator_precedence operators[OPERATOR_CNT] = {
	{"(", 0}, {")", 0}
	,{"->", 1}	
	,{"*", 4}	,{"/", 3}	,{"%", 2}	
	,{"+", 6}	,{"-", 5}	
	,{"<", 7}	,{"<=", 7}	,{">", 7}	,{">=", 7}
	,{"==", 8}	,{"!=", 8}
	,{"&", 9}
	,{"^", 10}
	,{"|", 11}
	,{"&&", 12}
	,{"||", 13}
	,{"=", 14}	,{"+=", 14}	,{"-=", 14}	,{"&=", 14}	,{"|=", 14}
};

//root1 -> 학생답안지
//root2 -> 표준답안지
void compare_tree(node *root1,  node *root2, int *result)//학생 답지 & 표준 답지 비교
{
	node *tmp;
	int cnt1, cnt2;

	if(root1 == NULL || root2 == NULL){//학생 답지 또는 표준 답지 노드가 NULL이라면 false 리턴
		*result = false;
		return;
	}

	if(!strcmp(root1->name, "<") || !strcmp(root1->name, ">") || !strcmp(root1->name, "<=") || !strcmp(root1->name, ">=")){//학생 답지 name이 <,>,<=,>= 라면 아래 코드 실행
		if(strcmp(root1->name, root2->name) != 0){

			if(!strncmp(root2->name, "<", 1))//표준 답지가 < 라면
				strncpy(root2->name, ">", 1);//root2->name에 > 복사

			else if(!strncmp(root2->name, ">", 1))//표준 답지가 >라면
				strncpy(root2->name, "<", 1);//root2->name에 < 복사

			else if(!strncmp(root2->name, "<=", 2))//표준답지가 <=라면
				strncpy(root2->name, ">=", 2);//root2->name에 >= 복사

			else if(!strncmp(root2->name, ">=", 2))//표준답지가 >=라면
				strncpy(root2->name, "<=", 2);//root2->name에 <= 복사

			root2 = change_sibling(root2);//형제 노드 change
		}
	}

	if(strcmp(root1->name, root2->name) != 0){//학생답지의 이름과 표준답지의 이름이 다르다면 false, 0점 부여
		*result = false;
		return;
	}

	//학생답지와 표준답지의 자식 노드가 둘 중 한쪽에만 값이 있다면 flase임
	if((root1->child_head != NULL && root2->child_head == NULL)
		|| (root1->child_head == NULL && root2->child_head != NULL)){
		*result = false;
		return;
	}

	else if(root1->child_head != NULL){//학생 답지의 자식노드가 들어있다면
		if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
			*result = false;//이때, 학생 답지와 표준 답지의 형제 노드 개수가 다르다면 false부여
			return;
		}

		
		if(!strcmp(root1->name, "==") || !strcmp(root1->name, "!="))
		{//학생 답지가 == 이거나 != 라면
			compare_tree(root1->child_head, root2->child_head, result);//재귀 호출을 통해 답을 비교 

			if(*result == false)//false라면
			{
				*result = true;//result를 true로 바꾸어줌
				root2 = change_sibling(root2);//이때, 다시 형제노드를 바꾸어줌
				compare_tree(root1->child_head, root2->child_head, result);//다시 재귀 호출을 통해 답을 비교
			}
		}
		else if(!strcmp(root1->name, "+") || !strcmp(root1->name, "*")
				|| !strcmp(root1->name, "|") || !strcmp(root1->name, "&")
				|| !strcmp(root1->name, "||") || !strcmp(root1->name, "&&"))
		{//학생답지가 -> + 거나 * 거나 | 거나 & 거나 || 거나 && 라면

			//학생 답지와 표준 답지의 형제 노드개수가 다르다면 false
			if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
				*result = false;
				return;
			}

			tmp = root2->child_head;//표준 답지의 자식노드를 tmp에 저장

			while(tmp->prev != NULL)//tmp에서 이전 노드로 돌아가며 부모 노드와 다이렉트로 연결되어 있도록 세팅
				tmp = tmp->prev;

			while(tmp != NULL)//tmp가 값이 있다면 
			{
				compare_tree(root1->child_head, tmp, result);//재귀 호출을 통해 result값을 구함
			
				if(*result == true)//참이라면 점수 부여
					break;
				else{//거짓이라면 result에 다시 true를 부여하고 다음 형제 노드를 tmp에 저장
					if(tmp->next != NULL)
						*result = true;
					tmp = tmp->next;
				}
			}
		}
		else{
			compare_tree(root1->child_head, root2->child_head, result);//위 조건에 만족하는 사항이 없으면 재귀호출을 통해 두 답지 비교
		}
	}	


	if(root1->next != NULL){//학생답지 다음 노드가 있다면

		if(get_sibling_cnt(root1) != get_sibling_cnt(root2)){//형제 노드개수 파악
			*result = false;
			return;
		}

		if(*result == true)//result가 참이라면
		{
			tmp = get_operator(root1);//학생 답지의 operator를 구함
	
			if(!strcmp(tmp->name, "+") || !strcmp(tmp->name, "*")
					|| !strcmp(tmp->name, "|") || !strcmp(tmp->name, "&")
					|| !strcmp(tmp->name, "||") || !strcmp(tmp->name, "&&"))
			{//tmp가 + 거나 * 거나 | 거나 & 거나 || 거나 && 라면	

				tmp = root2;//tmp에 표준답지 노드 할당
	
				while(tmp->prev != NULL)//부모노드와 다이렉트로 연결되어 있는 형제 노드 구하기
					tmp = tmp->prev;

				while(tmp != NULL)//tmp가 값이 있다면
				{
					compare_tree(root1->next, tmp, result);//재귀 호출을 통해 답지 비교

					if(*result == true)//그럼에도 result가 참이라면 점수부여
						break;
					else{//거짓이라면
						if(tmp->next != NULL)//그러나 다음 형제 노드가 있을 때, 재검색이 가능하도록 result에 true 할당
							*result = true;
						tmp = tmp->next;// tmp에 다음 형제노드 저장
					}
				}
			}

			else//+,*,|,&,||,&& 연산자가 아니라면
				compare_tree(root1->next, root2->next, result);//재귀호출을 통해 두 답지 비교
		}
	}
}

int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN]) //str 토큰 분할 및 생성
{
	char *start, *end;
	char tmp[BUFLEN];
	char str2[BUFLEN];
	char *op = "(),;><=!|&^/+-*\""; 
	int row = 0;
	int i;
 	int isPointer;
	int lcount, rcount;
	int p_str;
	
	clear_tokens(tokens);

	start = str; // str은 텍스트 파일의 내용물<학생 답안지 || 표준 답안지>
	
	if(is_typeStatement(str) == 0)
		return false;	
	// is_typeStatement() -> 0 : 에러, 0점
	//                    -> 1 : datatype이 없는 경우
	//                    -> 2 : datatype이 있는 경우
	
	while(1)
	{
		if((end = strpbrk(start, op)) == NULL) // 연산자<(),;><=!|&^/+-*\">가 존재하지 않는다면 아래 코드들을 실행할 필요없어 반복문 종료
			break;

		if(start == end){ // start<텍스트 파일 내용>와 end<opearation>가 동일하다면

			if(!strncmp(start, "--", 2) || !strncmp(start, "++", 2)){ // -- 또는 ++ 이라면
				if(!strncmp(start, "++++", 4)||!strncmp(start,"----",4)) // ++++ 또는 ---- 라면 false를 반환<0점>
					return false;// 문법적인 에러이므로 false 반환, 0점 부여

				if(is_character(*ltrim(start + 2))){ // (++ 또는 --) 다음 '0 ~ 9', 'A ~ Z', 'a ~ z'라는 캐릭터가 온다면 
					if(row > 0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]))
						return false; // 문법적인 에러이므로 false 반환, 0점 부여

					end = strpbrk(start + 2, op);//++ or -- 다음 op찾아서 포인터 위치 반환
					if(end == NULL)
						end = &str[strlen(str)];
					while(start < end) {//start가 end를 넘지 않았다면 
						if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
							return false;// 문법적인 에러이므로 false 반환, 0점 부여
						else if(*start != ' ')
							strncat(tokens[row], start, 1);//start가 가리키는 위치가 공백이 아니라면 tokens[row]에 이어붙임
						start++;	
					}
				}
				
				else if(row>0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){//이전 토큰의 끝 값이 charater일 때
					if(strstr(tokens[row - 1], "++") != NULL || strstr(tokens[row - 1], "--") != NULL)	
						return false; // 문법적인 에러이므로 false 반환, 0점 부여

					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);//++ 또는 -- tmp에 저장
					strcat(tokens[row - 1], tmp);//이전 토큰에 이어붙이기, ex) 이전 토큰 값은 p++과 같이 될 수 있음
					start += 2;//다음 토큰 가리키기
					row--;// 이전 값에 이어 붙였으므로 row--를 해줌
				}
				else{//현재 토큰에 tmp 이어 붙이기
					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);
					strcat(tokens[row], tmp);
					start += 2;
				}
			}
			else if(!strncmp(start, "==", 2) || !strncmp(start, "!=", 2) || !strncmp(start, "<=", 2)
				|| !strncmp(start, ">=", 2) || !strncmp(start, "||", 2) || !strncmp(start, "&&", 2) 
				|| !strncmp(start, "&=", 2) || !strncmp(start, "^=", 2) || !strncmp(start, "!=", 2) 
				|| !strncmp(start, "|=", 2) || !strncmp(start, "+=", 2)	|| !strncmp(start, "-=", 2) 
				|| !strncmp(start, "*=", 2) || !strncmp(start, "/=", 2)){
				
				// 여기 부분에 왔다면 start는 ++,-- 는 아닌 것임
				// 이 부분은 비교연산문 

				strncpy(tokens[row], start, 2);//tokens[row]에 비교 연산문 저장
				start += 2;//비교 연산문 다음 부분
			}
			else if(!strncmp(start, "->", 2))//멤버 참조 연산자라면
			{
				end = strpbrk(start + 2, op);//end에는 ->를 건너띈 다음 op를 저장하게됨

				if(end == NULL) //-> 뒤에 다른 op가 없다면 
					end = &str[strlen(str)]; //str의 끝부분의 포인터 값(\0)을 end에 저장

				while(start < end){//start == end(\0)이 될때까지 무한반복 
					if(*start != ' ')//start 포인터 값이 공백이면 건너띄고 다음 포인터 값을 읽음
						strncat(tokens[row - 1], start, 1);// 토큰 이전 값에 start 포인터 값을 이어붙임
					start++;
				}
				row--;//이전 값에 이어 붙였으므로 row -1
			}
			else if(*end == '&')//end 포인터 값이 &를 가리킨다면
			{
				
				if(row == 0 || (strpbrk(tokens[row - 1], op) != NULL)){//row가 0이거나 현재 토큰 값인 row-1에 op가 있다면
					end = strpbrk(start + 1, op);//end에는 start에 &를 건너띈 op포인터 값을 넣음
					if(end == NULL)//end가 NULL포인터를 가리킨다면
						end = &str[strlen(str)];// NULL포인터 값을 할당
					
					strncat(tokens[row], start, 1);//start의 1 토큰을 tokens[row]에 할당
					start++;// +1 위치 이동

					while(start < end){
						if(*(start - 1) == ' ' && tokens[row][strlen(tokens[row]) - 1] != '&')
							return false;// 문법적인 에러이므로 false 반환, 0점 부여//
						else if(*start != ' ')
							strncat(tokens[row], start, 1);//tokens[row]에 start 1토큰 넣기
						start++; // +1 위치 이동
					}
				}
				
				else{//row>0 && 이전 토큰 값에 op가 없다면
					strncpy(tokens[row], start, 1);//현재 토큰에 & 이어붙이기
					start += 1;//다음 포인터 가리키기
				}
				
			}
		  	else if(*end == '*')//end가 가리키는 부분이 '*'라면
			{
				isPointer=0;

				if(row > 0)
				{
					
					for(i = 0; i < DATATYPE_SIZE; i++) {
						if(strstr(tokens[row - 1], datatype[i]) != NULL){//데이터타입이 있다면 실행 
							strcat(tokens[row - 1], "*");//이전 토큰에 * 붙이기 ex)int*
							start += 1;	
							isPointer = 1;
							break;
						}
					}
					if(isPointer == 1)
						continue;
					if(*(start+1) !=0)//데이터타입이 없었고 start+1 값이 0이 아니라면
						end = start + 1;//end에 start+1 포인터 값 넣기

					
					if(row>1 && !strcmp(tokens[row - 2], "*") && (all_star(tokens[row - 1]) == 1)){
						strncat(tokens[row - 1], start, end - start);//이전 토큰 값에 *이어 붙이기
						row--;
					}
					
					
					else if(is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]) == 1){ 
						strncat(tokens[row], start, end - start);//이전 토큰 값이 문자였다면 현재 토큰에 넣기   
					}

					
					else if(strpbrk(tokens[row - 1], op) != NULL){//이전 토큰에 연산자가 있다면		
						strncat(tokens[row] , start, end - start);//현재 토큰에 이어붙임 
					}
					else
						strncat(tokens[row], start, end - start);//나머지 경우 현재 토큰에 이어 붙인다.

					start += (end - start);
				}

			 	else if(row == 0)//row가 0인 경우
				{
					if((end = strpbrk(start + 1, op)) == NULL){// '*'다음 op가 없다면
						strncat(tokens[row], start, 1);//현재 토큰에 * 이어 붙이기
						start += 1;//+1 위치 이동
					}
					else{
						while(start < end){
							if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
								return false;// 문법적인 에러이므로 false 반환, 0점 부여
							else if(*start != ' ')
								strncat(tokens[row], start, 1);//end에 부여된 '*'다음 op전까지 모든 내용을 토큰에 이어 붙이기
							start++;	
						}
						if(all_star(tokens[row]))//만약 현재 토큰값이 모두 * 라면 row-1을 한다
							row--;
						
					}
				}
			}
			else if(*end == '(') //end가 가리키는 위치가 '('라면
			{
				lcount = 0;
				rcount = 0;
				if(row>0 && (strcmp(tokens[row - 1],"&") == 0 || strcmp(tokens[row - 1], "*") == 0)){//이전 토큰 값이 &거나 *라면
					while(*(end + lcount + 1) == '(')
						lcount++;
					start += lcount;

					end = strpbrk(start + 1, ")");

					if(end == NULL)
						return false;// 문법적인 에러이므로 false 반환, 0점 부여
					else{
						while(*(end + rcount +1) == ')')
							rcount++;
						end += rcount;

						if(lcount != rcount)
							return false;// 문법적인 에러이므로 false 반환, 0점 부여

						if( (row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) || row == 1){ 
							strncat(tokens[row - 1], start + 1, end - start - rcount - 1);//start+1는 ( 다음 문자를 가리키게 된다. 즉, 이전 토큰에 문자만을 이어붙이겠다는 의미다. 
//ex) *(p)  >>  *p
							row--;
							start = end + 1;// ')' 문자 다음을 가리킴
						}
						else{
							strncat(tokens[row], start, 1);
							start += 1;
						}
					}
						
				}
				else{//row<=0 이거나 이전 토큰 값이 &,*이 아니라면
					strncat(tokens[row], start, 1);//토큰에 '('를 이어붙임
					start += 1;
				}

			}
			else if(*end == '\"') //end가 가리키는 위치가 " 라면
			{
				end = strpbrk(start + 1, "\"");// end에는 다음 "를 가리키는 포인터로 초기화
				
				if(end == NULL)//"의 짝이 맞지 않음으로 문법적인 오류임
					return false;// 문법적인 에러이므로 false 반환, 0점 부여

				else{
					strncat(tokens[row], start, end - start + 1);//" ~ "을 tokens[row]에 토큰 저장
					start = end + 1;// " ~ " 다음 위치의 포인터 값을 반환
				}

			}

			else{// start값이 ++,--,비교 연산자,-> 가 아니고 end가 &,*,(," 가 아니라면
				
				if(row > 0 && !strcmp(tokens[row - 1], "++"))
					return false;// 문법적인 에러이므로 false 반환, 0점 부여

				
				if(row > 0 && !strcmp(tokens[row - 1], "--"))
					return false;// 문법적인 에러이므로 false 반환, 0점 부여
	
				strncat(tokens[row], start, 1);//token[row]에 start 값 부여
				start += 1;//+1 위치 이동
				
			
				if(!strcmp(tokens[row], "-") || !strcmp(tokens[row], "+") || !strcmp(tokens[row], "--") || !strcmp(tokens[row], "++")){
					// token[row]가 - 또는 + 또는 -- 또는 ++ 일 경우


				
					if(row == 0)
						row--;

					
					else if(!is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){//이전 토큰 값 끝 부분이 문자라면
					
						if(strstr(tokens[row - 1], "++") == NULL && strstr(tokens[row - 1], "--") == NULL)//이전 토큰 값에 ++ 또는 --가 없으면 row--
							row--;
					}
				}
			}
		}
		else{ 
		// start<텍스트 내용> 과 end<operation> 이 다르다면 ex) &p -> (x), creat() -> (o)
			// tokens 전 값이 모두 *이고, row의 값이 2 이상이고, 토큰 전전값의 끝 부분이 character가 아니라면
			if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))   
				row--;				
			// tokens의 전 값이 모두 *이고, row의 값이 1이라면
			if(all_star(tokens[row - 1]) && row == 1)   
				row--;	

			for(i = 0; i < end - start; i++){ // end - start == operation 전까지의 캐릭터들을 검색할수 있음
				if(i > 0 && *(start + i) == '.'){// . 라는 문자열이 있다면 공백전까지 tokens[row]에 이어 붙이기
					strncat(tokens[row], start + i, 1);//. 토큰 저장

					while( *(start + i +1) == ' ' && i< end - start )
						i++; 
				}
				else if(start[i] == ' '){
					while(start[i] == ' ') // 공백을 건너 뛰기 위한 과정
						i++;
					break; // 공백을 건너 뛰었다면 for문을 벗어남
				}
				else // 처음부터 공백이 아니라면 
					strncat(tokens[row], start + i, 1);//end(op) 전까지 이어 붙이기
			}

			if(start[0] == ' '){ // 첫번째 부분에 공백 건너뛰는 과정
				start += i;
				continue;
			}
			start += i; // end에서 가리킨 operation 전까지의 문자를 모두 tokens에 넣었으니 포인터 위치를 변경
			// ex) creat(fname, O_RDONLY) 일때, start는 ( 의 위치를 가리키게 됨
		}
			
		strcpy(tokens[row], ltrim(rtrim(tokens[row]))); // 토큰의 좌우 공백값들을 제거하여 다시 저장

		 // (row의 값이 1 이상 && 현재 tokens의 값의 끝부분이 character) 
		 // &&
		 // (tokens 전 값 typeStatement가 2 || tokens 전 값의 끝부분이 character || tokens 전 값의 끝부분이 .)
		 // 이라면
		 if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) //현재 토큰 위치가 0보다 크고 현재 토큰의 값 끝이 character라면
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.' ) ){

			if(row > 1 && strcmp(tokens[row - 2],"(") == 0)
			{
				if(strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1],"unsigned") != 0)
					return false;// 문법적인 에러이므로 false 반환, 0점 부여
			}
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;// 문법적인 에러이므로 false 반환, 0점 부여
			}
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;// 문법적인 에러이므로 false 반환, 0점 부여
			}
			
		}

		if((row == 0 && !strcmp(tokens[row], "gcc")) ){ // tokens 첫 부분이 gcc라면 토큰을 나눌 필요 없음
			clear_tokens(tokens); // 토큰 clear
			strcpy(tokens[0], str);	 // 텍스트 파일의 모든 부분을 tokens[0]에 복사
			return 1; // true 반환
		} 

		row++;
	}//while 문 종료 부분
	//**********************************************************반복문 종료**********************************************************//

	if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) //이전 토큰 값이 모두 *이고 이전전 토큰 끝값이 문자가 아니라면 
		row--;
	if(all_star(tokens[row - 1]) && row == 1) //row=1, 이전 토큰 값이 모두 * 라면  
		row--;	

	for(i = 0; i < strlen(start); i++)  //start 길이 만큼 반복문 실행 
	{
		if(start[i] == ' ') // 공백 문자를 발견한다면 진입 
		{
			while(start[i] == ' ')//공백 문자가 나오지 않을때까지 반복문 진행(공백 문자는 건너띈다는 의미임)
				i++;
			if(start[0]==' ') {//배열의 첫번째 부분이 공백 문자라면
				start += i; //공백 문자 개수(i개) 만큼 건너띄어 start 시작 위치 초기화
				i = 0;//이런 경우엔 반복문을 처음부터 다시 실행
			}
			else
				row++; //공백문자가 아니라면 row +1 증가
			
			i--;
		} 
		else
		{
			strncat(tokens[row], start + i, 1);//tokens[] row번째 방에 start+i의 1 토큰 붙이기
			if( start[i] == '.' && i<strlen(start)){
				while(start[i + 1] == ' ' && i < strlen(start))
					i++;

			}
		}
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));//tokens[row] 배열에 좌우 공백 제거

		if(!strcmp(tokens[row], "lpthread") && row > 0 && !strcmp(tokens[row - 1], "-")){ //-lpthread가 붙은 경우 진입
			strcat(tokens[row - 1], tokens[row]);
			memset(tokens[row], 0, sizeof(tokens[row]));
			row--;
		}
	 	else if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1])
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.') ){
			
			if(row > 1 && strcmp(tokens[row-2],"(") == 0)
			{
				if(strcmp(tokens[row-1], "struct") != 0 && strcmp(tokens[row-1], "unsigned") != 0)
					return false;// 문법적인 에러이므로 false 반환, 0점 부여
			}
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;// 문법적인 에러이므로 false 반환, 0점 부여
			}
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;// 문법적인 에러이므로 false 반환, 0점 부여
			}
		} 
	}// for 반복문 종료, 에러를 확인하며 마저 이어 붙이지 못한 토큰을 이어붙임


	if(row > 0)
	{

		
		if(strcmp(tokens[0], "#include") == 0 || strcmp(tokens[0], "include") == 0 || strcmp(tokens[0], "struct") == 0){ 
			clear_tokens(tokens);//모든 토큰 0으로 초기화 
			strcpy(tokens[0], remove_extraspace(str));//다시 세팅 
		}
	}

	if(is_typeStatement(tokens[0]) == 2 || strstr(tokens[0], "extern") != NULL){//tokens[0]의 char 포인터 값을 보내 datatype을 파악하고 만약 datatype이 존재한다면 2가 리턴 됨.
		for(i = 1; i < TOKEN_CNT; i++){
			if(strcmp(tokens[i],"") == 0) // 토큰이 비어있다면 더 이상의 비교는 필요 없음. 이 조건을 통해 세그멘테이션 에러를 피할 수 있음 
				break;		       

			if(i != TOKEN_CNT -1 )//토큰의 끝까지 오지 않았다면
				strcat(tokens[0], " ");//tokens[0]에 공백을 붙임
			strcat(tokens[0], tokens[i]);
			memset(tokens[i], 0, sizeof(tokens[i]));//tokens[i]의 값을 tokens[0]에 붙였으므로 0으로 초기화 시킴
		}
	}
	
	
	while((p_str = find_typeSpecifier(tokens)) != -1){ 
		if(!reset_tokens(p_str, tokens))
			return false;// 문법적인 에러이므로 false 반환, 0점 부여
	}

	
	while((p_str = find_typeSpecifier2(tokens)) != -1){  
		if(!reset_tokens(p_str, tokens))
			return false;// 문법적인 에러이므로 false 반환, 0점 부여
	}
	
	return true;
}

//root는 std_root,ans_root
//tokens는 make_tokens()함수를 통해 만들어짐
//idx는 이 함수가 종료된 이후 사용되는 변수
//parentheses 초기값은 0이며 괄호를 의미
node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses)//노드 트리구조 형성
{
	node *cur = root;
	node *new;
	node *saved_operator;
	node *operator;
	int fstart;
	int i;

	while(1)	
	{
		if(strcmp(tokens[*idx], "") == 0)//전체 반복문 종료 조건
			break;
	
		if(!strcmp(tokens[*idx], ")"))
			return get_root(cur);//재귀 호출 반환값

		else if(!strcmp(tokens[*idx], ","))//재귀 호출 반환값
			return get_root(cur);

		else if(!strcmp(tokens[*idx], "("))
		{
			
			if(*idx > 0 && !is_operator(tokens[*idx - 1]) && strcmp(tokens[*idx - 1], ",") != 0){ // 토큰 전값이 operator가 아니고 && 그 값이 , 가 아님 
				fstart = true;

				while(1)
				{
					*idx += 1;//다음 토큰을 비교하기 위해 +1

					if(!strcmp(tokens[*idx], ")"))//재귀 호출 종료 조건, 괄호를 닫았기 때문
						break;
					
					new = make_tree(NULL, tokens, idx, parentheses + 1);//괄호 +1
					
					if(new != NULL){//new노드가 값이 있다면
						if(fstart == true){//fstart가 true라면
							cur->child_head = new;//new노드를 현재 노드의 자식노드로 넣어줌
							new->parent = cur;//new노드의 부모노드는 당연히 cur가 됨
	
							fstart = false;
						}
						else{//fstart가 false라면 형제노드 만들기
							cur->next = new;
							new->prev = cur;
						}

						cur = new;//cur는 다음 노드 트리를 구성하기 위해 new노드 값을 받음
					}

					if(!strcmp(tokens[*idx], ")"))// ')' 문자가 나온다면 반복문 종료
						break;
				}
			}
			else{//위 조건 외의 경우 
				*idx += 1;//다음 idx
	
				new = make_tree(NULL, tokens, idx, parentheses + 1);//새로운 노드 형성, 괄호 추가

				if(cur == NULL)//cur의 노드 값이 없다면
					cur = new;//new노드를 cur에 넣어줌

				else if(!strcmp(new->name, cur->name)){//new->name과 cur->name이 같다면
					if(!strcmp(new->name, "|") || !strcmp(new->name, "||") 
						|| !strcmp(new->name, "&") || !strcmp(new->name, "&&"))
					{// | 또는 || 또는 & 또는 && 

						cur = get_last_child(cur);//마지막 자식노드 구하기

						if(new->child_head != NULL){//new의 자식 노드가 존재한다면
							new = new->child_head;//new의 자식노드를 new에 넣어줌

							new->parent->child_head = NULL;//new->parent->child_head는 당연히 NULL 값이 됨
							new->parent = NULL;//new의 부모 노드 또한 NULL
							
							// 형제 노드 결성
							new->prev = cur;
							cur->next = new;
						}
					}
					else if(!strcmp(new->name, "+") || !strcmp(new->name, "*"))
					{//new가 + 또는 * 라면

						i = 0;

						while(1)
						{
							if(!strcmp(tokens[*idx + i], ""))// 토큰의 끝 값으로 도달했다면 break
								break;

							if(is_operator(tokens[*idx + i]) && strcmp(tokens[*idx + i], ")") != 0)//operator 이며 그 operator는 ) 이 아닐때
								break;

							i++;
						}
						
						// 위에 반복문을 통해서 구한 i위치 토큰과 new의 name을 비교
						if(get_precedence(tokens[*idx + i]) < get_precedence(new->name))
						{
							cur = get_last_child(cur);//cur의 마지막 자식 노드 구하기
							//형제 노드 형성
							cur->next = new;
							new->prev = cur;
							cur = new;//cur는 new 노드가 됨
						}
						else
						{
							cur = get_last_child(cur);//cur의 마지막 자식 노드 구하기

							if(new->child_head != NULL){//new의 자식 노드가 값이 있다면
								new = new->child_head;//new에 new의 자식 노드 할당

								new->parent->child_head = NULL;
								new->parent = NULL;
								// 형제 노드 형성
								new->prev = cur;
								cur->next = new;
							}
						}
					}
					else{
						cur = get_last_child(cur);//cur의 마지막 자식 노드 구하기
						// 형제 노드 형성
						cur->next = new;
						new->prev = cur;
						cur = new;//cur에 new 노드 할당
					}
				}
	
				else//new->name과 cur->name이 다르다면
				{
					cur = get_last_child(cur);//cur의 마지막 자식 노드 구하기
					// 형제 노드 형성
					cur->next = new;
					new->prev = cur;
	
					cur = new;//new노드를 cur에 할당
				}
			}
		}// '('비교문 종료
		else if(is_operator(tokens[*idx]))//현재 토큰이 operator라면
		{
			if(!strcmp(tokens[*idx], "||") || !strcmp(tokens[*idx], "&&")
					|| !strcmp(tokens[*idx], "|") || !strcmp(tokens[*idx], "&") 
					|| !strcmp(tokens[*idx], "+") || !strcmp(tokens[*idx], "*"))
			{// || 또는 && 또는 | 또는 & 또는 + 또는 *
				if(is_operator(cur->name) == true && !strcmp(cur->name, tokens[*idx]))
					operator = cur;//cur가 연산자 && cur와 토큰이 같은 값이라면 operator에 cur 노드 할당
		
				else
				{
					new = create_node(tokens[*idx], parentheses);//새로운 노드 만들기
					operator = get_most_high_precedence_node(cur, new);//가장 큰 상위 노드 구하기

					if(operator->parent == NULL && operator->prev == NULL){//부모노드가 없고 이전 형제노드가 없다면

						if(get_precedence(operator->name) < get_precedence(new->name)){
							cur = insert_node(operator, new);//operator와 new노드의 관계 형성
						}

						else if(get_precedence(operator->name) > get_precedence(new->name))
						{
							if(operator->child_head != NULL){//자식 노드가 있다면
								operator = get_last_child(operator);//마지막 자식노드 구하기
								cur = insert_node(operator, new);//operator와 new노드의 관계 형성
							}
						}
						else
						{
							operator = cur;
	
							while(1)
							{
								if(is_operator(operator->name) == true && !strcmp(operator->name, tokens[*idx]))//operator 노드가 operator이며 토큰 값과 같은 값을 가진다면
									break;
						
								if(operator->prev != NULL)//operator 노드의 이전 노드가 값이 존재 한다면
									operator = operator->prev;//이전 형제노드를 operator에 할당
								else
									break;
							}

							if(strcmp(operator->name, tokens[*idx]) != 0)//값이 다르다면
								operator = operator->parent;//operator의 부모노드를 operator에 할당

							if(operator != NULL){//operator가 값이 존재한다면
								if(!strcmp(operator->name, tokens[*idx]))//operator와 토큰의 값이 같다면
									cur = operator;
							}
						}
					}

					else//부모노드 값이 있거나 이전 형제 노드가 있다면
						cur = insert_node(operator, new);//노드의 관계 형성
				}

			}
			else
			{
				new = create_node(tokens[*idx], parentheses);//새로운 노드 형성

				if(cur == NULL)
					cur = new;//cur노드 형성

				else
				{
					operator = get_most_high_precedence_node(cur, new);//상위노드 만들기

					if(operator->parentheses > new->parentheses)//괄호갯수 비교
						cur = insert_node(operator, new);//노드 관계 형성

					else if(operator->parent == NULL && operator->prev ==  NULL){//부모 노드와 이전 형제노드가 존재하지 않는다면 ->(최상위 노드)
					
						if(get_precedence(operator->name) > get_precedence(new->name))
						{
							if(operator->child_head != NULL){//자식 노드가 있다면
	
								operator = get_last_child(operator);//operator의 마지막 자식 노드 구하기
								cur = insert_node(operator, new);//노드 관계 형성
							}
						}
					
						else	
							cur = insert_node(operator, new);//노드 관계 형성
					}
	
					else
						cur = insert_node(operator, new);//노드 관계 형성
				}
			}
		}//operator 비교문 종료
		else 
		{
			new = create_node(tokens[*idx], parentheses);//새로운 노드 형성

			if(cur == NULL)
				cur = new;//node값을 cur에 세팅

			else if(cur->child_head == NULL){
				cur->child_head = new;//자식 노드가 비어있다면 자식노드에 new노드 할당
				new->parent = cur;//당연히 new의 부모노드는 cur가 됨

				cur = new;// cur에 new 노드 할당
			}
			else{

				cur = get_last_child(cur);//자식노드가 있으므로 가장 마지막 자식 노드를 cur에 할당
				//형제노드 형성
				cur->next = new;
				new->prev = cur;
				//cur는 new노드가 됨
				cur = new;
			}
		}

		*idx += 1;//토큰 방번호 증가
	}//전체 반복문 종료

	return get_root(cur);
}

node *change_sibling(node *parent)//표준답지가 인자로 들어옴, 노드를 change 해주는 작업
{//형제 노드를 바꿔주는 역할을 한다.
	node *tmp;
	
	tmp = parent->child_head;//부모 노드의 자식 노드를 tmp로 저장, parent에 연결되어 있던 자식노드끼리 바꿔 줘야하기 때문에 tmp에 임시 저장

	parent->child_head = parent->child_head->next; //parent가 본래 연결되어 있던 자식1을 자식2로 연결해주는 과정
	parent->child_head->parent = parent;//자식2의 부모를 지정해주는 과정
	parent->child_head->prev = NULL;//이제 자식2는 prev가 아님

	parent->child_head->next = tmp;//본래 자식1 -> 자식2였다면 이제 자식2 -> 자식1로 바꾸어줌
	parent->child_head->next->prev = parent->child_head;//이제 자식1은 prev값을 자식2로 가지게됨
	parent->child_head->next->next = NULL;//자식1은 이제 next값이 없음
	parent->child_head->next->parent = NULL;//이제 자식1은 다이렉트로 부모노드와 연결되어 있지 않기에 NULL값으로 지정		

	return parent;
}

node *create_node(char *name, int parentheses)//노드 생성, name은 토큰, parentheses는 괄호
{
	node *new;

	new = (node *)malloc(sizeof(node));//노드 동적할당
	new->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));//토큰의 크기만큼 node->name에 name 동적 할당 
	strcpy(new->name, name); // new->name에 name 복사

	//초기화 과정
	new->parentheses = parentheses;
	new->parent = NULL;
	new->child_head = NULL;
	new->prev = NULL;
	new->next = NULL;

	return new;
}

int get_precedence(char *op)//연산자 상위 번호 리턴
{
	int i;

	for(i = 2; i < OPERATOR_CNT; i++){//괄호 제외하고 나머지 검색
		if(!strcmp(operators[i].operator, op))// .operator는 char로 오퍼레이터를 의미
			return operators[i].precedence;// precedence는 우선 순위를 정해주는 상위 번호를 의미
	}
	return false;
}

int is_operator(char *op)//operator인지 판별해주는 함수
{
	int i;

	for(i = 0; i < OPERATOR_CNT; i++)
	{
		if(operators[i].operator == NULL)//operator를 모두 검색했다면 반복문 종료
			break;
		if(!strcmp(operators[i].operator, op)){//operator와 op가 값이 같다면
			return true;
		}
	}

	return false;
}

void print(node *cur)//노드를 출력하여 테스트
{
	if(cur->child_head != NULL){
		print(cur->child_head);
		printf("\n");
	}

	if(cur->next != NULL){
		print(cur->next);
		printf("\t");
	}
	printf("%s", cur->name);
}

node *get_operator(node *cur)//operator 구하기
{
	if(cur == NULL)//비어 있는 값이라면 본래 값 리턴
		return cur;

	if(cur->prev != NULL)//이전 형제 노드 값이 있다면
		while(cur->prev != NULL)//부모 노드와 다이렉트로 연결되어 있는 노드 연결
			cur = cur->prev;

	return cur->parent;//cur의 부모노드를 돌려줌
}

node *get_root(node *cur)//root 구하기
{
	if(cur == NULL)//값이 비어 있다면 자기 자신 리턴
		return cur;

	while(cur->prev != NULL)//맨 처음 형제노드 구하기
		cur = cur->prev;

	if(cur->parent != NULL)//부모노드가 존재한다면 get_root()함수 재호출
		cur = get_root(cur->parent);

	return cur;
}

node *get_high_precedence_node(node *cur, node *new)//높은 상위 노드 구하기
{
	//cur가 operator && cur가 new보다 상위번호가 낮다면
	if(is_operator(cur->name))
		if(get_precedence(cur->name) < get_precedence(new->name))
			return cur;//cur 노드 반환

	if(cur->prev != NULL){//cur 이전 형제 노드가 값이 있다면
		while(cur->prev != NULL){
			cur = cur->prev;//cur에 이전 형제노드를 할당
			
			return get_high_precedence_node(cur, new);//재귀호출을 통해 재검사
		}


		if(cur->parent != NULL)// cur의 부모노드가 값이 있다면
			return get_high_precedence_node(cur->parent, new);//재귀호출을 통해 재검사
	}

	if(cur->parent == NULL)//cur의 부모노드가 없다면 cur 노드 반환
		return cur;
}

node *get_most_high_precedence_node(node *cur, node *new)//가장 높은 상위 노드 구하기
{
	node *operator = get_high_precedence_node(cur, new);//상위노드를 operator에 넣어줌
	node *saved_operator = operator;//saved_operator 노드에 저장

	while(1)
	{
		if(saved_operator->parent == NULL)//부모노드가 없다면 반복문 종료
			break;

		if(saved_operator->prev != NULL)//이전 형제노드가 있다면
			operator = get_high_precedence_node(saved_operator->prev, new);//이전 형제 노드의 상위 노드를 operator에 할당

		else if(saved_operator->parent != NULL)//부모노드가 있다면
			operator = get_high_precedence_node(saved_operator->parent, new);//부모 노드의 상위 노드를 operator에 할당

		saved_operator = operator;//saved_operator 노드에 저장
	}
	
	return saved_operator;//가장 높은 상위 노드 operator 반환
}

node *insert_node(node *old, node *new)//노드를 삽입하여 old 노드와 new 노드의 관계형성
{
	if(old->prev != NULL){//old노드의 이전 형제 노드 값이 있다면
		new->prev = old->prev;
		old->prev->next = new;
		old->prev = NULL;
	}

	new->child_head = old;//new의 자식 노드는 old노드가 됨
	old->parent = new;//old의 부모 노드는 new가 됨

	return new;
}

node *get_last_child(node *cur)//마지막 자식 노드를 구하는 과정
{
	if(cur->child_head != NULL)//cur의 자식 노드 구하기
		cur = cur->child_head;

	while(cur->next != NULL)//자식 노드의 끝 부분에 있는 형제 노드 구하기
		cur = cur->next;

	return cur;//마지막 자식 노드 반환
}

int get_sibling_cnt(node *cur)//형제 노드 갯수 파악하기
{
	int i = 0;

	while(cur->prev != NULL)//이전 노드값으로 돌아가여 부모노드와 다이렉트로 연결되어 있는 노드로 가게 됨
		cur = cur->prev;

	while(cur->next != NULL){//다음 노드값으로 이동하여 초기 노드 상태로 돌려줌. 이때 i값을 카운팅하면 형제 노드 갯수를 파악할 수 있음
		cur = cur->next;
		i++;
	}

	return i;
}

void free_node(node *cur)//노드 해제
{
	if(cur->child_head != NULL)
		free_node(cur->child_head);//연달아 연결된 자식노드를 해제하기 위한 재귀호출

	if(cur->next != NULL)
		free_node(cur->next);//연달아 연결된 형제노드를 해제하기 위한 재귀호출

	if(cur != NULL){//자식이 더이상 없으며 막내 노드까지 접근했으며 cur의 노드가 값이 존재한다면
		cur->prev = NULL;
		cur->next = NULL;
		cur->parent = NULL;
		cur->child_head = NULL;
		free(cur);//모두 해제
	}
}


int is_character(char c)//charater인지 판별
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_typeStatement(char *str) //type 상태 구하기, str == (학생 답안지 || 표준 답안지)
{ 
	// return 0 -> 0점
	// return 1 -> datatype이 존재하지 않을 경우
	// return 2 -> gcc라는 명령어가 존재할 경우, datatype이 존재할 경우
	char *start;
	char str2[BUFLEN] = {0}; 
	char tmp[BUFLEN] = {0}; 
	char tmp2[BUFLEN] = {0}; 
	int i;	 
	
	start = str;
	strncpy(str2,str,strlen(str)); // str2에 str를 copy
	remove_space(str2); // 중간 중간 존재하는 공백 제거

	while(start[0] == ' ') // 왼쪽 공백 건너띄기
		start += 1;

	if(strstr(str2, "gcc") != NULL) // gcc라는 명령어가 str2에 존재한다면
	{
		strncpy(tmp2, start, strlen("gcc"));
		if(strcmp(tmp2,"gcc") != 0)
			return 0; // gcc가 str2 문자열 맨 앞에 존재하지 않는 경우, (x)
		else
			return 2; // gcc가 str2 문자열 맨 앞에 존재하는 경우, (o)
	}
	
	for(i = 0; i < DATATYPE_SIZE; i++) // DATATYPE_SIZE의 개수는 35개 이므로 #define으로 정의해둠
	{
		if(strstr(str2,datatype[i]) != NULL) // datatype이 존재할 경우
		{	
			strncpy(tmp, str2, strlen(datatype[i]));
			strncpy(tmp2, start, strlen(datatype[i]));
			
			if(strcmp(tmp, datatype[i]) == 0)
				if(strcmp(tmp, tmp2) != 0)
					return 0; // tmp와 tmp2의 값이 같을 때, (x)
				else
					return 2; // tmp와 tmp2의 값이 같을 때, (o)
		}

	}
	return 1; // datatype이 존재하지 않을 경우

}

int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]) //타입 지정자 구하기
{
	int i, j;

	for(i = 0; i < TOKEN_CNT; i++)//각 방의 tokens[]를 datatype의 갯수만큼 비교 하기 위한 부분
	{
		for(j = 0; j < DATATYPE_SIZE; j++)
		{
			if(strstr(tokens[i], datatype[j]) != NULL && i > 0)//
			{
				//tokens[i] 좌우측에 '(',')'가 있다 &&
				//')'다음 char*에 '&' 또는 '*' 또는 ')' 또는 '(' 또는 '-' 또는 '+' 또는 문자가 온다면
				if(!strcmp(tokens[i - 1], "(") && !strcmp(tokens[i + 1], ")") 
						&& (tokens[i + 2][0] == '&' || tokens[i + 2][0] == '*' 
							|| tokens[i + 2][0] == ')' || tokens[i + 2][0] == '(' 
							|| tokens[i + 2][0] == '-' || tokens[i + 2][0] == '+' 
							|| is_character(tokens[i + 2][0])))  
					return i;// i값 반환
			}
		}
	}
	return -1;
}

int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]) //타입지정자 구하기2
{
    int i, j;

   
    for(i = 0; i < TOKEN_CNT; i++)
    {
        for(j = 0; j < DATATYPE_SIZE; j++)
        {
            if(!strcmp(tokens[i], "struct") && (i+1) <= TOKEN_CNT && is_character(tokens[i + 1][strlen(tokens[i + 1]) - 1])) //토큰값이 struct이며 다음 토큰값의 끝부분이 문자라면 
                    return i;
        }
    }
    return -1;
}

int all_star(char *str)//문자 모두 * 인지 확인
{
	int i;
	int length= strlen(str);
	
 	if(length == 0)	
		return 0;
	
	for(i = 0; i < length; i++)
		if(str[i] != '*')
			return 0;
	return 1;

}

int all_character(char *str)//모두 문자인지 확인
{
	int i;

	for(i = 0; i < strlen(str); i++)
		if(is_character(str[i]))
			return 1;
	return 0;
	
}

int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]) //토큰 리셋
{
	int i;
	int j = start - 1;
	int lcount = 0, rcount = 0;
	int sub_lcount = 0, sub_rcount = 0;

	if(start > -1){
		if(!strcmp(tokens[start], "struct")) {	//start토큰 값이 struct라면	
			strcat(tokens[start], " ");//start토큰 값에 공백 이어 붙이기
			strcat(tokens[start], tokens[start+1]); // start토큰 값에 start+1 토큰 값 이어붙이기

			for(i = start + 1; i < TOKEN_CNT - 1; i++){//위 과정에서 토큰 값을 이어 붙였으므로 앞으로 당기며 token값을 리셋해야함
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

		else if(!strcmp(tokens[start], "unsigned") && strcmp(tokens[start+1], ")") != 0) {		
			//start토큰 값이 unsigned이며 start+1토큰 값이 )이 아니라면-> (unsigned)의 경우를 피하는 것임
			strcat(tokens[start], " ");//공백 이어붙이기
			strcat(tokens[start], tokens[start + 1]);//start토큰에 start+1토큰값 이어붙이기	     
			strcat(tokens[start], tokens[start + 2]);//start+2토큰을 start토큰에 이어붙이기

			for(i = start + 1; i < TOKEN_CNT - 1; i++){//토큰 값 리셋
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

     		j = start + 1;           
        	while(!strcmp(tokens[j], ")")){// j토큰이 ) 라면
                	rcount ++;//괄호 카운터
                	if(j==TOKEN_CNT)//세그멘테이션 에러를 피하기 위한 조건문
                        	break;
                	j++;
        	}
	
		j = start - 1;
		while(!strcmp(tokens[j], "(")){//괄호 카운터
        	        lcount ++;
                	if(j == 0)//잘못된 배열에 접근하는 것을 피하기 위한 조건문
                        	break;
               		j--;
		}
		if( (j!=0 && is_character(tokens[j][strlen(tokens[j])-1]) ) || j==0)
			lcount = rcount;//오른쪽 괄호 개수를 왼쪽 괄호 개수에 할당

		if(lcount != rcount )//문법적 에러, 0점 부여
			return false;

		if( (start - lcount) >0 && !strcmp(tokens[start - lcount - 1], "sizeof")){
			return true;//참인 경우 
		}
		
		else if((!strcmp(tokens[start], "unsigned") || !strcmp(tokens[start], "struct")) && strcmp(tokens[start+1], ")")) {		
		// unsigned이거나 struct이며 닫는 괄호 ) 가 아니라면
			strcat(tokens[start - lcount], tokens[start]);
			strcat(tokens[start - lcount], tokens[start + 1]);
			strcpy(tokens[start - lcount + 1], tokens[start + rcount]);
		 
		 	//토큰 리셋
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount]);
				memset(tokens[i + lcount + rcount], 0, sizeof(tokens[0]));
			}


		}
 		else{
			if(tokens[start + 2][0] == '('){//토큰 값이 ( 라면
				j = start + 2;
				while(!strcmp(tokens[j], "(")){//j토큰 값이 ( 라면
					sub_lcount++;// ( 개수 카운터
					j++;
				} 	
				if(!strcmp(tokens[j + 1],")")){//다음 토큰 값이 ) 라면
					j = j + 1;
					while(!strcmp(tokens[j], ")")){
						sub_rcount++;
						j++;
					}
				}
				else 
					return false;//문법적 에러, 0점 부여

				if(sub_lcount != sub_rcount)
					return false;//문법적 에러, 0점 부여
				
				strcpy(tokens[start + 2], tokens[start + 2 + sub_lcount]);	
				for(int i = start + 3; i<TOKEN_CNT; i++)//토큰 리셋
					memset(tokens[i], 0, sizeof(tokens[0]));

			}
			strcat(tokens[start - lcount], tokens[start]);
			strcat(tokens[start - lcount], tokens[start + 1]);
			strcat(tokens[start - lcount], tokens[start + rcount + 1]);
		 
		 	//토큰 리셋
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount -1; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount +1]);
				memset(tokens[i + lcount + rcount + 1], 0, sizeof(tokens[0]));

			}
		}
	}
	return true;
}

void clear_tokens(char tokens[TOKEN_CNT][MINLEN])//토큰 모든 값을 0으로 초기화
{
	int i;

	for(i = 0; i < TOKEN_CNT; i++)
		memset(tokens[i], 0, sizeof(tokens[i]));
}

char *rtrim(char *_str) // 오른쪽 공백 제거
{
	char tmp[BUFLEN];
	char *end;

	strcpy(tmp, _str);
	end = tmp + strlen(tmp) - 1;//end 배열에 문자열 끝부분 저장
	while(end != _str && isspace(*end))
		--end;

	*(end + 1) = '\0'; // 마지막 끝 부분엔 \0로 마무리하여 끝맺음하기
	_str = tmp;
	return _str;//오른쪽 공백이 제거된 char 포인터 반환
}

char *ltrim(char *_str) // 왼쪽 공백 제거
{
	char *start = _str;

	while(*start != '\0' && isspace(*start))//문자열 끝이 아니고 start포인터 값이 공백이라면 진입
		++start;
	_str = start;
	return _str; // 왼쪽 공백이 제거된 char 포인터 반환
}

char* remove_extraspace(char *str)//공백 제거 함수
{
	int i;
	char *str2 = (char*)malloc(sizeof(char) * BUFLEN);
	char *start, *end;
	char temp[BUFLEN] = "";
	int position;

	if(strstr(str,"include<")!=NULL){
		start = str;
		end = strpbrk(str, "<");
		position = end - start;
	
		strncat(temp, str, position);//include 복사
		strncat(temp, " ", 1);//" "이어 붙이기
		strncat(temp, str + position, strlen(str) - position + 1);//< ~~~  >이어 붙이기

		str = temp;	//str에 include<~~~> -> include <~~~>로 바꾸기
	}
	
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] ==' ')//공백이 발견 된다면
		{
			if(i == 0 && str[0] ==' ')
				while(str[i + 1] == ' ')
					i++;//공백 패스	
			else{
				if(i > 0 && str[i - 1] != ' ')//이전 값이 공백이 아니라면
					str2[strlen(str2)] = str[i];
				while(str[i + 1] == ' ')
					i++;//공백 제거
			} 
		}//공백이 아니라면
		else
			str2[strlen(str2)] = str[i];
	}

	return str2;
}



void remove_space(char *str) // 중간 중간에 존재하는 공백 제거
{
	char* i = str;
	char* j = str;
	
	while(*j != 0)
	{
		*i = *j++;
		if(*i != ' ')
			i++;
	}
	*i = 0;
}

int check_brackets(char *str) // '(' 와 ')' 의 개수가 다르다면 정상적으로 동작하지 않을 것임. 즉 틀린 문제
{
	char *start = str;
	int lcount = 0, rcount = 0;
	
	while(1){// '(' 개수, ')'개수 구하기
		if((start = strpbrk(start, "()")) != NULL){// '(', ')'괄호가 존재하면
			if(*(start) == '(')
				lcount++;
			else
				rcount++;

			start += 1; 		
		}
		else
			break;
	}

	if(lcount != rcount)//짝이 맞지 않으면 문법적인 에러
		return 0;
	else //짝이 맞아야 true 반환
		return 1;
}

int get_token_cnt(char tokens[TOKEN_CNT][MINLEN])//토큰의 개수 구하는 함수
{
	int i;
	
	for(i = 0; i < TOKEN_CNT; i++)
		if(!strcmp(tokens[i], ""))
			break;

	return i;
}
