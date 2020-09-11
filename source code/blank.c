#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "blank.h"

//��� ������ Ÿ�� ����
char datatype[DATATYPE_SIZE][MINLEN] = {"int", "char", "double", "float", "long"
			, "short", "ushort", "FILE", "DIR","pid"
			,"key_t", "ssize_t", "mode_t", "ino_t", "dev_t"
			, "nlink_t", "uid_t", "gid_t", "time_t", "blksize_t"
			, "blkcnt_t", "pid_t", "pthread_mutex_t", "pthread_cond_t", "pthread_t"
			, "void", "size_t", "unsigned", "sigset_t", "sigjmp_buf"
			, "rlim_t", "jmp_buf", "sig_atomic_t", "clock_t", "struct"};


//operator���� �켱���� �Ҵ�
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

//root1 -> �л������
//root2 -> ǥ�ش����
void compare_tree(node *root1,  node *root2, int *result)//�л� ���� & ǥ�� ���� ��
{
	node *tmp;
	int cnt1, cnt2;

	if(root1 == NULL || root2 == NULL){//�л� ���� �Ǵ� ǥ�� ���� ��尡 NULL�̶�� false ����
		*result = false;
		return;
	}

	if(!strcmp(root1->name, "<") || !strcmp(root1->name, ">") || !strcmp(root1->name, "<=") || !strcmp(root1->name, ">=")){//�л� ���� name�� <,>,<=,>= ��� �Ʒ� �ڵ� ����
		if(strcmp(root1->name, root2->name) != 0){

			if(!strncmp(root2->name, "<", 1))//ǥ�� ������ < ���
				strncpy(root2->name, ">", 1);//root2->name�� > ����

			else if(!strncmp(root2->name, ">", 1))//ǥ�� ������ >���
				strncpy(root2->name, "<", 1);//root2->name�� < ����

			else if(!strncmp(root2->name, "<=", 2))//ǥ�ش����� <=���
				strncpy(root2->name, ">=", 2);//root2->name�� >= ����

			else if(!strncmp(root2->name, ">=", 2))//ǥ�ش����� >=���
				strncpy(root2->name, "<=", 2);//root2->name�� <= ����

			root2 = change_sibling(root2);//���� ��� change
		}
	}

	if(strcmp(root1->name, root2->name) != 0){//�л������� �̸��� ǥ�ش����� �̸��� �ٸ��ٸ� false, 0�� �ο�
		*result = false;
		return;
	}

	//�л������� ǥ�ش����� �ڽ� ��尡 �� �� ���ʿ��� ���� �ִٸ� flase��
	if((root1->child_head != NULL && root2->child_head == NULL)
		|| (root1->child_head == NULL && root2->child_head != NULL)){
		*result = false;
		return;
	}

	else if(root1->child_head != NULL){//�л� ������ �ڽĳ�尡 ����ִٸ�
		if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
			*result = false;//�̶�, �л� ������ ǥ�� ������ ���� ��� ������ �ٸ��ٸ� false�ο�
			return;
		}

		
		if(!strcmp(root1->name, "==") || !strcmp(root1->name, "!="))
		{//�л� ������ == �̰ų� != ���
			compare_tree(root1->child_head, root2->child_head, result);//��� ȣ���� ���� ���� �� 

			if(*result == false)//false���
			{
				*result = true;//result�� true�� �ٲپ���
				root2 = change_sibling(root2);//�̶�, �ٽ� ������带 �ٲپ���
				compare_tree(root1->child_head, root2->child_head, result);//�ٽ� ��� ȣ���� ���� ���� ��
			}
		}
		else if(!strcmp(root1->name, "+") || !strcmp(root1->name, "*")
				|| !strcmp(root1->name, "|") || !strcmp(root1->name, "&")
				|| !strcmp(root1->name, "||") || !strcmp(root1->name, "&&"))
		{//�л������� -> + �ų� * �ų� | �ų� & �ų� || �ų� && ���

			//�л� ������ ǥ�� ������ ���� ��尳���� �ٸ��ٸ� false
			if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
				*result = false;
				return;
			}

			tmp = root2->child_head;//ǥ�� ������ �ڽĳ�带 tmp�� ����

			while(tmp->prev != NULL)//tmp���� ���� ���� ���ư��� �θ� ���� ���̷�Ʈ�� ����Ǿ� �ֵ��� ����
				tmp = tmp->prev;

			while(tmp != NULL)//tmp�� ���� �ִٸ� 
			{
				compare_tree(root1->child_head, tmp, result);//��� ȣ���� ���� result���� ����
			
				if(*result == true)//���̶�� ���� �ο�
					break;
				else{//�����̶�� result�� �ٽ� true�� �ο��ϰ� ���� ���� ��带 tmp�� ����
					if(tmp->next != NULL)
						*result = true;
					tmp = tmp->next;
				}
			}
		}
		else{
			compare_tree(root1->child_head, root2->child_head, result);//�� ���ǿ� �����ϴ� ������ ������ ���ȣ���� ���� �� ���� ��
		}
	}	


	if(root1->next != NULL){//�л����� ���� ��尡 �ִٸ�

		if(get_sibling_cnt(root1) != get_sibling_cnt(root2)){//���� ��尳�� �ľ�
			*result = false;
			return;
		}

		if(*result == true)//result�� ���̶��
		{
			tmp = get_operator(root1);//�л� ������ operator�� ����
	
			if(!strcmp(tmp->name, "+") || !strcmp(tmp->name, "*")
					|| !strcmp(tmp->name, "|") || !strcmp(tmp->name, "&")
					|| !strcmp(tmp->name, "||") || !strcmp(tmp->name, "&&"))
			{//tmp�� + �ų� * �ų� | �ų� & �ų� || �ų� && ���	

				tmp = root2;//tmp�� ǥ�ش��� ��� �Ҵ�
	
				while(tmp->prev != NULL)//�θ���� ���̷�Ʈ�� ����Ǿ� �ִ� ���� ��� ���ϱ�
					tmp = tmp->prev;

				while(tmp != NULL)//tmp�� ���� �ִٸ�
				{
					compare_tree(root1->next, tmp, result);//��� ȣ���� ���� ���� ��

					if(*result == true)//�׷����� result�� ���̶�� �����ο�
						break;
					else{//�����̶��
						if(tmp->next != NULL)//�׷��� ���� ���� ��尡 ���� ��, ��˻��� �����ϵ��� result�� true �Ҵ�
							*result = true;
						tmp = tmp->next;// tmp�� ���� ������� ����
					}
				}
			}

			else//+,*,|,&,||,&& �����ڰ� �ƴ϶��
				compare_tree(root1->next, root2->next, result);//���ȣ���� ���� �� ���� ��
		}
	}
}

int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN]) //str ��ū ���� �� ����
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

	start = str; // str�� �ؽ�Ʈ ������ ���빰<�л� ����� || ǥ�� �����>
	
	if(is_typeStatement(str) == 0)
		return false;	
	// is_typeStatement() -> 0 : ����, 0��
	//                    -> 1 : datatype�� ���� ���
	//                    -> 2 : datatype�� �ִ� ���
	
	while(1)
	{
		if((end = strpbrk(start, op)) == NULL) // ������<(),;><=!|&^/+-*\">�� �������� �ʴ´ٸ� �Ʒ� �ڵ���� ������ �ʿ���� �ݺ��� ����
			break;

		if(start == end){ // start<�ؽ�Ʈ ���� ����>�� end<opearation>�� �����ϴٸ�

			if(!strncmp(start, "--", 2) || !strncmp(start, "++", 2)){ // -- �Ǵ� ++ �̶��
				if(!strncmp(start, "++++", 4)||!strncmp(start,"----",4)) // ++++ �Ǵ� ---- ��� false�� ��ȯ<0��>
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�

				if(is_character(*ltrim(start + 2))){ // (++ �Ǵ� --) ���� '0 ~ 9', 'A ~ Z', 'a ~ z'��� ĳ���Ͱ� �´ٸ� 
					if(row > 0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]))
						return false; // �������� �����̹Ƿ� false ��ȯ, 0�� �ο�

					end = strpbrk(start + 2, op);//++ or -- ���� opã�Ƽ� ������ ��ġ ��ȯ
					if(end == NULL)
						end = &str[strlen(str)];
					while(start < end) {//start�� end�� ���� �ʾҴٸ� 
						if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
							return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
						else if(*start != ' ')
							strncat(tokens[row], start, 1);//start�� ����Ű�� ��ġ�� ������ �ƴ϶�� tokens[row]�� �̾����
						start++;	
					}
				}
				
				else if(row>0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){//���� ��ū�� �� ���� charater�� ��
					if(strstr(tokens[row - 1], "++") != NULL || strstr(tokens[row - 1], "--") != NULL)	
						return false; // �������� �����̹Ƿ� false ��ȯ, 0�� �ο�

					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);//++ �Ǵ� -- tmp�� ����
					strcat(tokens[row - 1], tmp);//���� ��ū�� �̾���̱�, ex) ���� ��ū ���� p++�� ���� �� �� ����
					start += 2;//���� ��ū ����Ű��
					row--;// ���� ���� �̾� �ٿ����Ƿ� row--�� ����
				}
				else{//���� ��ū�� tmp �̾� ���̱�
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
				
				// ���� �κп� �Դٸ� start�� ++,-- �� �ƴ� ����
				// �� �κ��� �񱳿��깮 

				strncpy(tokens[row], start, 2);//tokens[row]�� �� ���깮 ����
				start += 2;//�� ���깮 ���� �κ�
			}
			else if(!strncmp(start, "->", 2))//��� ���� �����ڶ��
			{
				end = strpbrk(start + 2, op);//end���� ->�� �ǳʶ� ���� op�� �����ϰԵ�

				if(end == NULL) //-> �ڿ� �ٸ� op�� ���ٸ� 
					end = &str[strlen(str)]; //str�� ���κ��� ������ ��(\0)�� end�� ����

				while(start < end){//start == end(\0)�� �ɶ����� ���ѹݺ� 
					if(*start != ' ')//start ������ ���� �����̸� �ǳʶ�� ���� ������ ���� ����
						strncat(tokens[row - 1], start, 1);// ��ū ���� ���� start ������ ���� �̾����
					start++;
				}
				row--;//���� ���� �̾� �ٿ����Ƿ� row -1
			}
			else if(*end == '&')//end ������ ���� &�� ����Ų�ٸ�
			{
				
				if(row == 0 || (strpbrk(tokens[row - 1], op) != NULL)){//row�� 0�̰ų� ���� ��ū ���� row-1�� op�� �ִٸ�
					end = strpbrk(start + 1, op);//end���� start�� &�� �ǳʶ� op������ ���� ����
					if(end == NULL)//end�� NULL�����͸� ����Ų�ٸ�
						end = &str[strlen(str)];// NULL������ ���� �Ҵ�
					
					strncat(tokens[row], start, 1);//start�� 1 ��ū�� tokens[row]�� �Ҵ�
					start++;// +1 ��ġ �̵�

					while(start < end){
						if(*(start - 1) == ' ' && tokens[row][strlen(tokens[row]) - 1] != '&')
							return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�//
						else if(*start != ' ')
							strncat(tokens[row], start, 1);//tokens[row]�� start 1��ū �ֱ�
						start++; // +1 ��ġ �̵�
					}
				}
				
				else{//row>0 && ���� ��ū ���� op�� ���ٸ�
					strncpy(tokens[row], start, 1);//���� ��ū�� & �̾���̱�
					start += 1;//���� ������ ����Ű��
				}
				
			}
		  	else if(*end == '*')//end�� ����Ű�� �κ��� '*'���
			{
				isPointer=0;

				if(row > 0)
				{
					
					for(i = 0; i < DATATYPE_SIZE; i++) {
						if(strstr(tokens[row - 1], datatype[i]) != NULL){//������Ÿ���� �ִٸ� ���� 
							strcat(tokens[row - 1], "*");//���� ��ū�� * ���̱� ex)int*
							start += 1;	
							isPointer = 1;
							break;
						}
					}
					if(isPointer == 1)
						continue;
					if(*(start+1) !=0)//������Ÿ���� ������ start+1 ���� 0�� �ƴ϶��
						end = start + 1;//end�� start+1 ������ �� �ֱ�

					
					if(row>1 && !strcmp(tokens[row - 2], "*") && (all_star(tokens[row - 1]) == 1)){
						strncat(tokens[row - 1], start, end - start);//���� ��ū ���� *�̾� ���̱�
						row--;
					}
					
					
					else if(is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]) == 1){ 
						strncat(tokens[row], start, end - start);//���� ��ū ���� ���ڿ��ٸ� ���� ��ū�� �ֱ�   
					}

					
					else if(strpbrk(tokens[row - 1], op) != NULL){//���� ��ū�� �����ڰ� �ִٸ�		
						strncat(tokens[row] , start, end - start);//���� ��ū�� �̾���� 
					}
					else
						strncat(tokens[row], start, end - start);//������ ��� ���� ��ū�� �̾� ���δ�.

					start += (end - start);
				}

			 	else if(row == 0)//row�� 0�� ���
				{
					if((end = strpbrk(start + 1, op)) == NULL){// '*'���� op�� ���ٸ�
						strncat(tokens[row], start, 1);//���� ��ū�� * �̾� ���̱�
						start += 1;//+1 ��ġ �̵�
					}
					else{
						while(start < end){
							if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
								return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
							else if(*start != ' ')
								strncat(tokens[row], start, 1);//end�� �ο��� '*'���� op������ ��� ������ ��ū�� �̾� ���̱�
							start++;	
						}
						if(all_star(tokens[row]))//���� ���� ��ū���� ��� * ��� row-1�� �Ѵ�
							row--;
						
					}
				}
			}
			else if(*end == '(') //end�� ����Ű�� ��ġ�� '('���
			{
				lcount = 0;
				rcount = 0;
				if(row>0 && (strcmp(tokens[row - 1],"&") == 0 || strcmp(tokens[row - 1], "*") == 0)){//���� ��ū ���� &�ų� *���
					while(*(end + lcount + 1) == '(')
						lcount++;
					start += lcount;

					end = strpbrk(start + 1, ")");

					if(end == NULL)
						return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
					else{
						while(*(end + rcount +1) == ')')
							rcount++;
						end += rcount;

						if(lcount != rcount)
							return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�

						if( (row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) || row == 1){ 
							strncat(tokens[row - 1], start + 1, end - start - rcount - 1);//start+1�� ( ���� ���ڸ� ����Ű�� �ȴ�. ��, ���� ��ū�� ���ڸ��� �̾���̰ڴٴ� �ǹ̴�. 
//ex) *(p)  >>  *p
							row--;
							start = end + 1;// ')' ���� ������ ����Ŵ
						}
						else{
							strncat(tokens[row], start, 1);
							start += 1;
						}
					}
						
				}
				else{//row<=0 �̰ų� ���� ��ū ���� &,*�� �ƴ϶��
					strncat(tokens[row], start, 1);//��ū�� '('�� �̾����
					start += 1;
				}

			}
			else if(*end == '\"') //end�� ����Ű�� ��ġ�� " ���
			{
				end = strpbrk(start + 1, "\"");// end���� ���� "�� ����Ű�� �����ͷ� �ʱ�ȭ
				
				if(end == NULL)//"�� ¦�� ���� �������� �������� ������
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�

				else{
					strncat(tokens[row], start, end - start + 1);//" ~ "�� tokens[row]�� ��ū ����
					start = end + 1;// " ~ " ���� ��ġ�� ������ ���� ��ȯ
				}

			}

			else{// start���� ++,--,�� ������,-> �� �ƴϰ� end�� &,*,(," �� �ƴ϶��
				
				if(row > 0 && !strcmp(tokens[row - 1], "++"))
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�

				
				if(row > 0 && !strcmp(tokens[row - 1], "--"))
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
	
				strncat(tokens[row], start, 1);//token[row]�� start �� �ο�
				start += 1;//+1 ��ġ �̵�
				
			
				if(!strcmp(tokens[row], "-") || !strcmp(tokens[row], "+") || !strcmp(tokens[row], "--") || !strcmp(tokens[row], "++")){
					// token[row]�� - �Ǵ� + �Ǵ� -- �Ǵ� ++ �� ���


				
					if(row == 0)
						row--;

					
					else if(!is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){//���� ��ū �� �� �κ��� ���ڶ��
					
						if(strstr(tokens[row - 1], "++") == NULL && strstr(tokens[row - 1], "--") == NULL)//���� ��ū ���� ++ �Ǵ� --�� ������ row--
							row--;
					}
				}
			}
		}
		else{ 
		// start<�ؽ�Ʈ ����> �� end<operation> �� �ٸ��ٸ� ex) &p -> (x), creat() -> (o)
			// tokens �� ���� ��� *�̰�, row�� ���� 2 �̻��̰�, ��ū �������� �� �κ��� character�� �ƴ϶��
			if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))   
				row--;				
			// tokens�� �� ���� ��� *�̰�, row�� ���� 1�̶��
			if(all_star(tokens[row - 1]) && row == 1)   
				row--;	

			for(i = 0; i < end - start; i++){ // end - start == operation �������� ĳ���͵��� �˻��Ҽ� ����
				if(i > 0 && *(start + i) == '.'){// . ��� ���ڿ��� �ִٸ� ���������� tokens[row]�� �̾� ���̱�
					strncat(tokens[row], start + i, 1);//. ��ū ����

					while( *(start + i +1) == ' ' && i< end - start )
						i++; 
				}
				else if(start[i] == ' '){
					while(start[i] == ' ') // ������ �ǳ� �ٱ� ���� ����
						i++;
					break; // ������ �ǳ� �پ��ٸ� for���� ���
				}
				else // ó������ ������ �ƴ϶�� 
					strncat(tokens[row], start + i, 1);//end(op) ������ �̾� ���̱�
			}

			if(start[0] == ' '){ // ù��° �κп� ���� �ǳʶٴ� ����
				start += i;
				continue;
			}
			start += i; // end���� ����Ų operation �������� ���ڸ� ��� tokens�� �־����� ������ ��ġ�� ����
			// ex) creat(fname, O_RDONLY) �϶�, start�� ( �� ��ġ�� ����Ű�� ��
		}
			
		strcpy(tokens[row], ltrim(rtrim(tokens[row]))); // ��ū�� �¿� ���鰪���� �����Ͽ� �ٽ� ����

		 // (row�� ���� 1 �̻� && ���� tokens�� ���� ���κ��� character) 
		 // &&
		 // (tokens �� �� typeStatement�� 2 || tokens �� ���� ���κ��� character || tokens �� ���� ���κ��� .)
		 // �̶��
		 if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) //���� ��ū ��ġ�� 0���� ũ�� ���� ��ū�� �� ���� character���
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.' ) ){

			if(row > 1 && strcmp(tokens[row - 2],"(") == 0)
			{
				if(strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1],"unsigned") != 0)
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
			}
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
			}
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
			}
			
		}

		if((row == 0 && !strcmp(tokens[row], "gcc")) ){ // tokens ù �κ��� gcc��� ��ū�� ���� �ʿ� ����
			clear_tokens(tokens); // ��ū clear
			strcpy(tokens[0], str);	 // �ؽ�Ʈ ������ ��� �κ��� tokens[0]�� ����
			return 1; // true ��ȯ
		} 

		row++;
	}//while �� ���� �κ�
	//**********************************************************�ݺ��� ����**********************************************************//

	if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) //���� ��ū ���� ��� *�̰� ������ ��ū ������ ���ڰ� �ƴ϶�� 
		row--;
	if(all_star(tokens[row - 1]) && row == 1) //row=1, ���� ��ū ���� ��� * ���  
		row--;	

	for(i = 0; i < strlen(start); i++)  //start ���� ��ŭ �ݺ��� ���� 
	{
		if(start[i] == ' ') // ���� ���ڸ� �߰��Ѵٸ� ���� 
		{
			while(start[i] == ' ')//���� ���ڰ� ������ ���������� �ݺ��� ����(���� ���ڴ� �ǳʶ�ٴ� �ǹ���)
				i++;
			if(start[0]==' ') {//�迭�� ù��° �κ��� ���� ���ڶ��
				start += i; //���� ���� ����(i��) ��ŭ �ǳʶ�� start ���� ��ġ �ʱ�ȭ
				i = 0;//�̷� ��쿣 �ݺ����� ó������ �ٽ� ����
			}
			else
				row++; //���鹮�ڰ� �ƴ϶�� row +1 ����
			
			i--;
		} 
		else
		{
			strncat(tokens[row], start + i, 1);//tokens[] row��° �濡 start+i�� 1 ��ū ���̱�
			if( start[i] == '.' && i<strlen(start)){
				while(start[i + 1] == ' ' && i < strlen(start))
					i++;

			}
		}
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));//tokens[row] �迭�� �¿� ���� ����

		if(!strcmp(tokens[row], "lpthread") && row > 0 && !strcmp(tokens[row - 1], "-")){ //-lpthread�� ���� ��� ����
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
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
			}
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
			}
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
			}
		} 
	}// for �ݺ��� ����, ������ Ȯ���ϸ� ���� �̾� ������ ���� ��ū�� �̾����


	if(row > 0)
	{

		
		if(strcmp(tokens[0], "#include") == 0 || strcmp(tokens[0], "include") == 0 || strcmp(tokens[0], "struct") == 0){ 
			clear_tokens(tokens);//��� ��ū 0���� �ʱ�ȭ 
			strcpy(tokens[0], remove_extraspace(str));//�ٽ� ���� 
		}
	}

	if(is_typeStatement(tokens[0]) == 2 || strstr(tokens[0], "extern") != NULL){//tokens[0]�� char ������ ���� ���� datatype�� �ľ��ϰ� ���� datatype�� �����Ѵٸ� 2�� ���� ��.
		for(i = 1; i < TOKEN_CNT; i++){
			if(strcmp(tokens[i],"") == 0) // ��ū�� ����ִٸ� �� �̻��� �񱳴� �ʿ� ����. �� ������ ���� ���׸����̼� ������ ���� �� ���� 
				break;		       

			if(i != TOKEN_CNT -1 )//��ū�� ������ ���� �ʾҴٸ�
				strcat(tokens[0], " ");//tokens[0]�� ������ ����
			strcat(tokens[0], tokens[i]);
			memset(tokens[i], 0, sizeof(tokens[i]));//tokens[i]�� ���� tokens[0]�� �ٿ����Ƿ� 0���� �ʱ�ȭ ��Ŵ
		}
	}
	
	
	while((p_str = find_typeSpecifier(tokens)) != -1){ 
		if(!reset_tokens(p_str, tokens))
			return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
	}

	
	while((p_str = find_typeSpecifier2(tokens)) != -1){  
		if(!reset_tokens(p_str, tokens))
			return false;// �������� �����̹Ƿ� false ��ȯ, 0�� �ο�
	}
	
	return true;
}

//root�� std_root,ans_root
//tokens�� make_tokens()�Լ��� ���� �������
//idx�� �� �Լ��� ����� ���� ���Ǵ� ����
//parentheses �ʱⰪ�� 0�̸� ��ȣ�� �ǹ�
node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses)//��� Ʈ������ ����
{
	node *cur = root;
	node *new;
	node *saved_operator;
	node *operator;
	int fstart;
	int i;

	while(1)	
	{
		if(strcmp(tokens[*idx], "") == 0)//��ü �ݺ��� ���� ����
			break;
	
		if(!strcmp(tokens[*idx], ")"))
			return get_root(cur);//��� ȣ�� ��ȯ��

		else if(!strcmp(tokens[*idx], ","))//��� ȣ�� ��ȯ��
			return get_root(cur);

		else if(!strcmp(tokens[*idx], "("))
		{
			
			if(*idx > 0 && !is_operator(tokens[*idx - 1]) && strcmp(tokens[*idx - 1], ",") != 0){ // ��ū ������ operator�� �ƴϰ� && �� ���� , �� �ƴ� 
				fstart = true;

				while(1)
				{
					*idx += 1;//���� ��ū�� ���ϱ� ���� +1

					if(!strcmp(tokens[*idx], ")"))//��� ȣ�� ���� ����, ��ȣ�� �ݾұ� ����
						break;
					
					new = make_tree(NULL, tokens, idx, parentheses + 1);//��ȣ +1
					
					if(new != NULL){//new��尡 ���� �ִٸ�
						if(fstart == true){//fstart�� true���
							cur->child_head = new;//new��带 ���� ����� �ڽĳ��� �־���
							new->parent = cur;//new����� �θ���� �翬�� cur�� ��
	
							fstart = false;
						}
						else{//fstart�� false��� ������� �����
							cur->next = new;
							new->prev = cur;
						}

						cur = new;//cur�� ���� ��� Ʈ���� �����ϱ� ���� new��� ���� ����
					}

					if(!strcmp(tokens[*idx], ")"))// ')' ���ڰ� ���´ٸ� �ݺ��� ����
						break;
				}
			}
			else{//�� ���� ���� ��� 
				*idx += 1;//���� idx
	
				new = make_tree(NULL, tokens, idx, parentheses + 1);//���ο� ��� ����, ��ȣ �߰�

				if(cur == NULL)//cur�� ��� ���� ���ٸ�
					cur = new;//new��带 cur�� �־���

				else if(!strcmp(new->name, cur->name)){//new->name�� cur->name�� ���ٸ�
					if(!strcmp(new->name, "|") || !strcmp(new->name, "||") 
						|| !strcmp(new->name, "&") || !strcmp(new->name, "&&"))
					{// | �Ǵ� || �Ǵ� & �Ǵ� && 

						cur = get_last_child(cur);//������ �ڽĳ�� ���ϱ�

						if(new->child_head != NULL){//new�� �ڽ� ��尡 �����Ѵٸ�
							new = new->child_head;//new�� �ڽĳ�带 new�� �־���

							new->parent->child_head = NULL;//new->parent->child_head�� �翬�� NULL ���� ��
							new->parent = NULL;//new�� �θ� ��� ���� NULL
							
							// ���� ��� �Ἲ
							new->prev = cur;
							cur->next = new;
						}
					}
					else if(!strcmp(new->name, "+") || !strcmp(new->name, "*"))
					{//new�� + �Ǵ� * ���

						i = 0;

						while(1)
						{
							if(!strcmp(tokens[*idx + i], ""))// ��ū�� �� ������ �����ߴٸ� break
								break;

							if(is_operator(tokens[*idx + i]) && strcmp(tokens[*idx + i], ")") != 0)//operator �̸� �� operator�� ) �� �ƴҶ�
								break;

							i++;
						}
						
						// ���� �ݺ����� ���ؼ� ���� i��ġ ��ū�� new�� name�� ��
						if(get_precedence(tokens[*idx + i]) < get_precedence(new->name))
						{
							cur = get_last_child(cur);//cur�� ������ �ڽ� ��� ���ϱ�
							//���� ��� ����
							cur->next = new;
							new->prev = cur;
							cur = new;//cur�� new ��尡 ��
						}
						else
						{
							cur = get_last_child(cur);//cur�� ������ �ڽ� ��� ���ϱ�

							if(new->child_head != NULL){//new�� �ڽ� ��尡 ���� �ִٸ�
								new = new->child_head;//new�� new�� �ڽ� ��� �Ҵ�

								new->parent->child_head = NULL;
								new->parent = NULL;
								// ���� ��� ����
								new->prev = cur;
								cur->next = new;
							}
						}
					}
					else{
						cur = get_last_child(cur);//cur�� ������ �ڽ� ��� ���ϱ�
						// ���� ��� ����
						cur->next = new;
						new->prev = cur;
						cur = new;//cur�� new ��� �Ҵ�
					}
				}
	
				else//new->name�� cur->name�� �ٸ��ٸ�
				{
					cur = get_last_child(cur);//cur�� ������ �ڽ� ��� ���ϱ�
					// ���� ��� ����
					cur->next = new;
					new->prev = cur;
	
					cur = new;//new��带 cur�� �Ҵ�
				}
			}
		}// '('�񱳹� ����
		else if(is_operator(tokens[*idx]))//���� ��ū�� operator���
		{
			if(!strcmp(tokens[*idx], "||") || !strcmp(tokens[*idx], "&&")
					|| !strcmp(tokens[*idx], "|") || !strcmp(tokens[*idx], "&") 
					|| !strcmp(tokens[*idx], "+") || !strcmp(tokens[*idx], "*"))
			{// || �Ǵ� && �Ǵ� | �Ǵ� & �Ǵ� + �Ǵ� *
				if(is_operator(cur->name) == true && !strcmp(cur->name, tokens[*idx]))
					operator = cur;//cur�� ������ && cur�� ��ū�� ���� ���̶�� operator�� cur ��� �Ҵ�
		
				else
				{
					new = create_node(tokens[*idx], parentheses);//���ο� ��� �����
					operator = get_most_high_precedence_node(cur, new);//���� ū ���� ��� ���ϱ�

					if(operator->parent == NULL && operator->prev == NULL){//�θ��尡 ���� ���� ������尡 ���ٸ�

						if(get_precedence(operator->name) < get_precedence(new->name)){
							cur = insert_node(operator, new);//operator�� new����� ���� ����
						}

						else if(get_precedence(operator->name) > get_precedence(new->name))
						{
							if(operator->child_head != NULL){//�ڽ� ��尡 �ִٸ�
								operator = get_last_child(operator);//������ �ڽĳ�� ���ϱ�
								cur = insert_node(operator, new);//operator�� new����� ���� ����
							}
						}
						else
						{
							operator = cur;
	
							while(1)
							{
								if(is_operator(operator->name) == true && !strcmp(operator->name, tokens[*idx]))//operator ��尡 operator�̸� ��ū ���� ���� ���� �����ٸ�
									break;
						
								if(operator->prev != NULL)//operator ����� ���� ��尡 ���� ���� �Ѵٸ�
									operator = operator->prev;//���� ������带 operator�� �Ҵ�
								else
									break;
							}

							if(strcmp(operator->name, tokens[*idx]) != 0)//���� �ٸ��ٸ�
								operator = operator->parent;//operator�� �θ��带 operator�� �Ҵ�

							if(operator != NULL){//operator�� ���� �����Ѵٸ�
								if(!strcmp(operator->name, tokens[*idx]))//operator�� ��ū�� ���� ���ٸ�
									cur = operator;
							}
						}
					}

					else//�θ��� ���� �ְų� ���� ���� ��尡 �ִٸ�
						cur = insert_node(operator, new);//����� ���� ����
				}

			}
			else
			{
				new = create_node(tokens[*idx], parentheses);//���ο� ��� ����

				if(cur == NULL)
					cur = new;//cur��� ����

				else
				{
					operator = get_most_high_precedence_node(cur, new);//������� �����

					if(operator->parentheses > new->parentheses)//��ȣ���� ��
						cur = insert_node(operator, new);//��� ���� ����

					else if(operator->parent == NULL && operator->prev ==  NULL){//�θ� ���� ���� ������尡 �������� �ʴ´ٸ� ->(�ֻ��� ���)
					
						if(get_precedence(operator->name) > get_precedence(new->name))
						{
							if(operator->child_head != NULL){//�ڽ� ��尡 �ִٸ�
	
								operator = get_last_child(operator);//operator�� ������ �ڽ� ��� ���ϱ�
								cur = insert_node(operator, new);//��� ���� ����
							}
						}
					
						else	
							cur = insert_node(operator, new);//��� ���� ����
					}
	
					else
						cur = insert_node(operator, new);//��� ���� ����
				}
			}
		}//operator �񱳹� ����
		else 
		{
			new = create_node(tokens[*idx], parentheses);//���ο� ��� ����

			if(cur == NULL)
				cur = new;//node���� cur�� ����

			else if(cur->child_head == NULL){
				cur->child_head = new;//�ڽ� ��尡 ����ִٸ� �ڽĳ�忡 new��� �Ҵ�
				new->parent = cur;//�翬�� new�� �θ���� cur�� ��

				cur = new;// cur�� new ��� �Ҵ�
			}
			else{

				cur = get_last_child(cur);//�ڽĳ�尡 �����Ƿ� ���� ������ �ڽ� ��带 cur�� �Ҵ�
				//������� ����
				cur->next = new;
				new->prev = cur;
				//cur�� new��尡 ��
				cur = new;
			}
		}

		*idx += 1;//��ū ���ȣ ����
	}//��ü �ݺ��� ����

	return get_root(cur);
}

node *change_sibling(node *parent)//ǥ�ش����� ���ڷ� ����, ��带 change ���ִ� �۾�
{//���� ��带 �ٲ��ִ� ������ �Ѵ�.
	node *tmp;
	
	tmp = parent->child_head;//�θ� ����� �ڽ� ��带 tmp�� ����, parent�� ����Ǿ� �ִ� �ڽĳ�峢�� �ٲ� ����ϱ� ������ tmp�� �ӽ� ����

	parent->child_head = parent->child_head->next; //parent�� ���� ����Ǿ� �ִ� �ڽ�1�� �ڽ�2�� �������ִ� ����
	parent->child_head->parent = parent;//�ڽ�2�� �θ� �������ִ� ����
	parent->child_head->prev = NULL;//���� �ڽ�2�� prev�� �ƴ�

	parent->child_head->next = tmp;//���� �ڽ�1 -> �ڽ�2���ٸ� ���� �ڽ�2 -> �ڽ�1�� �ٲپ���
	parent->child_head->next->prev = parent->child_head;//���� �ڽ�1�� prev���� �ڽ�2�� �����Ե�
	parent->child_head->next->next = NULL;//�ڽ�1�� ���� next���� ����
	parent->child_head->next->parent = NULL;//���� �ڽ�1�� ���̷�Ʈ�� �θ���� ����Ǿ� ���� �ʱ⿡ NULL������ ����		

	return parent;
}

node *create_node(char *name, int parentheses)//��� ����, name�� ��ū, parentheses�� ��ȣ
{
	node *new;

	new = (node *)malloc(sizeof(node));//��� �����Ҵ�
	new->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));//��ū�� ũ�⸸ŭ node->name�� name ���� �Ҵ� 
	strcpy(new->name, name); // new->name�� name ����

	//�ʱ�ȭ ����
	new->parentheses = parentheses;
	new->parent = NULL;
	new->child_head = NULL;
	new->prev = NULL;
	new->next = NULL;

	return new;
}

int get_precedence(char *op)//������ ���� ��ȣ ����
{
	int i;

	for(i = 2; i < OPERATOR_CNT; i++){//��ȣ �����ϰ� ������ �˻�
		if(!strcmp(operators[i].operator, op))// .operator�� char�� ���۷����͸� �ǹ�
			return operators[i].precedence;// precedence�� �켱 ������ �����ִ� ���� ��ȣ�� �ǹ�
	}
	return false;
}

int is_operator(char *op)//operator���� �Ǻ����ִ� �Լ�
{
	int i;

	for(i = 0; i < OPERATOR_CNT; i++)
	{
		if(operators[i].operator == NULL)//operator�� ��� �˻��ߴٸ� �ݺ��� ����
			break;
		if(!strcmp(operators[i].operator, op)){//operator�� op�� ���� ���ٸ�
			return true;
		}
	}

	return false;
}

void print(node *cur)//��带 ����Ͽ� �׽�Ʈ
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

node *get_operator(node *cur)//operator ���ϱ�
{
	if(cur == NULL)//��� �ִ� ���̶�� ���� �� ����
		return cur;

	if(cur->prev != NULL)//���� ���� ��� ���� �ִٸ�
		while(cur->prev != NULL)//�θ� ���� ���̷�Ʈ�� ����Ǿ� �ִ� ��� ����
			cur = cur->prev;

	return cur->parent;//cur�� �θ��带 ������
}

node *get_root(node *cur)//root ���ϱ�
{
	if(cur == NULL)//���� ��� �ִٸ� �ڱ� �ڽ� ����
		return cur;

	while(cur->prev != NULL)//�� ó�� ������� ���ϱ�
		cur = cur->prev;

	if(cur->parent != NULL)//�θ��尡 �����Ѵٸ� get_root()�Լ� ��ȣ��
		cur = get_root(cur->parent);

	return cur;
}

node *get_high_precedence_node(node *cur, node *new)//���� ���� ��� ���ϱ�
{
	//cur�� operator && cur�� new���� ������ȣ�� ���ٸ�
	if(is_operator(cur->name))
		if(get_precedence(cur->name) < get_precedence(new->name))
			return cur;//cur ��� ��ȯ

	if(cur->prev != NULL){//cur ���� ���� ��尡 ���� �ִٸ�
		while(cur->prev != NULL){
			cur = cur->prev;//cur�� ���� ������带 �Ҵ�
			
			return get_high_precedence_node(cur, new);//���ȣ���� ���� ��˻�
		}


		if(cur->parent != NULL)// cur�� �θ��尡 ���� �ִٸ�
			return get_high_precedence_node(cur->parent, new);//���ȣ���� ���� ��˻�
	}

	if(cur->parent == NULL)//cur�� �θ��尡 ���ٸ� cur ��� ��ȯ
		return cur;
}

node *get_most_high_precedence_node(node *cur, node *new)//���� ���� ���� ��� ���ϱ�
{
	node *operator = get_high_precedence_node(cur, new);//������带 operator�� �־���
	node *saved_operator = operator;//saved_operator ��忡 ����

	while(1)
	{
		if(saved_operator->parent == NULL)//�θ��尡 ���ٸ� �ݺ��� ����
			break;

		if(saved_operator->prev != NULL)//���� ������尡 �ִٸ�
			operator = get_high_precedence_node(saved_operator->prev, new);//���� ���� ����� ���� ��带 operator�� �Ҵ�

		else if(saved_operator->parent != NULL)//�θ��尡 �ִٸ�
			operator = get_high_precedence_node(saved_operator->parent, new);//�θ� ����� ���� ��带 operator�� �Ҵ�

		saved_operator = operator;//saved_operator ��忡 ����
	}
	
	return saved_operator;//���� ���� ���� ��� operator ��ȯ
}

node *insert_node(node *old, node *new)//��带 �����Ͽ� old ���� new ����� ��������
{
	if(old->prev != NULL){//old����� ���� ���� ��� ���� �ִٸ�
		new->prev = old->prev;
		old->prev->next = new;
		old->prev = NULL;
	}

	new->child_head = old;//new�� �ڽ� ���� old��尡 ��
	old->parent = new;//old�� �θ� ���� new�� ��

	return new;
}

node *get_last_child(node *cur)//������ �ڽ� ��带 ���ϴ� ����
{
	if(cur->child_head != NULL)//cur�� �ڽ� ��� ���ϱ�
		cur = cur->child_head;

	while(cur->next != NULL)//�ڽ� ����� �� �κп� �ִ� ���� ��� ���ϱ�
		cur = cur->next;

	return cur;//������ �ڽ� ��� ��ȯ
}

int get_sibling_cnt(node *cur)//���� ��� ���� �ľ��ϱ�
{
	int i = 0;

	while(cur->prev != NULL)//���� ��尪���� ���ư��� �θ���� ���̷�Ʈ�� ����Ǿ� �ִ� ���� ���� ��
		cur = cur->prev;

	while(cur->next != NULL){//���� ��尪���� �̵��Ͽ� �ʱ� ��� ���·� ������. �̶� i���� ī�����ϸ� ���� ��� ������ �ľ��� �� ����
		cur = cur->next;
		i++;
	}

	return i;
}

void free_node(node *cur)//��� ����
{
	if(cur->child_head != NULL)
		free_node(cur->child_head);//���޾� ����� �ڽĳ�带 �����ϱ� ���� ���ȣ��

	if(cur->next != NULL)
		free_node(cur->next);//���޾� ����� ������带 �����ϱ� ���� ���ȣ��

	if(cur != NULL){//�ڽ��� ���̻� ������ ���� ������ ���������� cur�� ��尡 ���� �����Ѵٸ�
		cur->prev = NULL;
		cur->next = NULL;
		cur->parent = NULL;
		cur->child_head = NULL;
		free(cur);//��� ����
	}
}


int is_character(char c)//charater���� �Ǻ�
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_typeStatement(char *str) //type ���� ���ϱ�, str == (�л� ����� || ǥ�� �����)
{ 
	// return 0 -> 0��
	// return 1 -> datatype�� �������� ���� ���
	// return 2 -> gcc��� ��ɾ ������ ���, datatype�� ������ ���
	char *start;
	char str2[BUFLEN] = {0}; 
	char tmp[BUFLEN] = {0}; 
	char tmp2[BUFLEN] = {0}; 
	int i;	 
	
	start = str;
	strncpy(str2,str,strlen(str)); // str2�� str�� copy
	remove_space(str2); // �߰� �߰� �����ϴ� ���� ����

	while(start[0] == ' ') // ���� ���� �ǳʶ��
		start += 1;

	if(strstr(str2, "gcc") != NULL) // gcc��� ��ɾ str2�� �����Ѵٸ�
	{
		strncpy(tmp2, start, strlen("gcc"));
		if(strcmp(tmp2,"gcc") != 0)
			return 0; // gcc�� str2 ���ڿ� �� �տ� �������� �ʴ� ���, (x)
		else
			return 2; // gcc�� str2 ���ڿ� �� �տ� �����ϴ� ���, (o)
	}
	
	for(i = 0; i < DATATYPE_SIZE; i++) // DATATYPE_SIZE�� ������ 35�� �̹Ƿ� #define���� �����ص�
	{
		if(strstr(str2,datatype[i]) != NULL) // datatype�� ������ ���
		{	
			strncpy(tmp, str2, strlen(datatype[i]));
			strncpy(tmp2, start, strlen(datatype[i]));
			
			if(strcmp(tmp, datatype[i]) == 0)
				if(strcmp(tmp, tmp2) != 0)
					return 0; // tmp�� tmp2�� ���� ���� ��, (x)
				else
					return 2; // tmp�� tmp2�� ���� ���� ��, (o)
		}

	}
	return 1; // datatype�� �������� ���� ���

}

int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]) //Ÿ�� ������ ���ϱ�
{
	int i, j;

	for(i = 0; i < TOKEN_CNT; i++)//�� ���� tokens[]�� datatype�� ������ŭ �� �ϱ� ���� �κ�
	{
		for(j = 0; j < DATATYPE_SIZE; j++)
		{
			if(strstr(tokens[i], datatype[j]) != NULL && i > 0)//
			{
				//tokens[i] �¿����� '(',')'�� �ִ� &&
				//')'���� char*�� '&' �Ǵ� '*' �Ǵ� ')' �Ǵ� '(' �Ǵ� '-' �Ǵ� '+' �Ǵ� ���ڰ� �´ٸ�
				if(!strcmp(tokens[i - 1], "(") && !strcmp(tokens[i + 1], ")") 
						&& (tokens[i + 2][0] == '&' || tokens[i + 2][0] == '*' 
							|| tokens[i + 2][0] == ')' || tokens[i + 2][0] == '(' 
							|| tokens[i + 2][0] == '-' || tokens[i + 2][0] == '+' 
							|| is_character(tokens[i + 2][0])))  
					return i;// i�� ��ȯ
			}
		}
	}
	return -1;
}

int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]) //Ÿ�������� ���ϱ�2
{
    int i, j;

   
    for(i = 0; i < TOKEN_CNT; i++)
    {
        for(j = 0; j < DATATYPE_SIZE; j++)
        {
            if(!strcmp(tokens[i], "struct") && (i+1) <= TOKEN_CNT && is_character(tokens[i + 1][strlen(tokens[i + 1]) - 1])) //��ū���� struct�̸� ���� ��ū���� ���κ��� ���ڶ�� 
                    return i;
        }
    }
    return -1;
}

int all_star(char *str)//���� ��� * ���� Ȯ��
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

int all_character(char *str)//��� �������� Ȯ��
{
	int i;

	for(i = 0; i < strlen(str); i++)
		if(is_character(str[i]))
			return 1;
	return 0;
	
}

int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]) //��ū ����
{
	int i;
	int j = start - 1;
	int lcount = 0, rcount = 0;
	int sub_lcount = 0, sub_rcount = 0;

	if(start > -1){
		if(!strcmp(tokens[start], "struct")) {	//start��ū ���� struct���	
			strcat(tokens[start], " ");//start��ū ���� ���� �̾� ���̱�
			strcat(tokens[start], tokens[start+1]); // start��ū ���� start+1 ��ū �� �̾���̱�

			for(i = start + 1; i < TOKEN_CNT - 1; i++){//�� �������� ��ū ���� �̾� �ٿ����Ƿ� ������ ���� token���� �����ؾ���
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

		else if(!strcmp(tokens[start], "unsigned") && strcmp(tokens[start+1], ")") != 0) {		
			//start��ū ���� unsigned�̸� start+1��ū ���� )�� �ƴ϶��-> (unsigned)�� ��츦 ���ϴ� ����
			strcat(tokens[start], " ");//���� �̾���̱�
			strcat(tokens[start], tokens[start + 1]);//start��ū�� start+1��ū�� �̾���̱�	     
			strcat(tokens[start], tokens[start + 2]);//start+2��ū�� start��ū�� �̾���̱�

			for(i = start + 1; i < TOKEN_CNT - 1; i++){//��ū �� ����
				strcpy(tokens[i], tokens[i + 1]);
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

     		j = start + 1;           
        	while(!strcmp(tokens[j], ")")){// j��ū�� ) ���
                	rcount ++;//��ȣ ī����
                	if(j==TOKEN_CNT)//���׸����̼� ������ ���ϱ� ���� ���ǹ�
                        	break;
                	j++;
        	}
	
		j = start - 1;
		while(!strcmp(tokens[j], "(")){//��ȣ ī����
        	        lcount ++;
                	if(j == 0)//�߸��� �迭�� �����ϴ� ���� ���ϱ� ���� ���ǹ�
                        	break;
               		j--;
		}
		if( (j!=0 && is_character(tokens[j][strlen(tokens[j])-1]) ) || j==0)
			lcount = rcount;//������ ��ȣ ������ ���� ��ȣ ������ �Ҵ�

		if(lcount != rcount )//������ ����, 0�� �ο�
			return false;

		if( (start - lcount) >0 && !strcmp(tokens[start - lcount - 1], "sizeof")){
			return true;//���� ��� 
		}
		
		else if((!strcmp(tokens[start], "unsigned") || !strcmp(tokens[start], "struct")) && strcmp(tokens[start+1], ")")) {		
		// unsigned�̰ų� struct�̸� �ݴ� ��ȣ ) �� �ƴ϶��
			strcat(tokens[start - lcount], tokens[start]);
			strcat(tokens[start - lcount], tokens[start + 1]);
			strcpy(tokens[start - lcount + 1], tokens[start + rcount]);
		 
		 	//��ū ����
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount]);
				memset(tokens[i + lcount + rcount], 0, sizeof(tokens[0]));
			}


		}
 		else{
			if(tokens[start + 2][0] == '('){//��ū ���� ( ���
				j = start + 2;
				while(!strcmp(tokens[j], "(")){//j��ū ���� ( ���
					sub_lcount++;// ( ���� ī����
					j++;
				} 	
				if(!strcmp(tokens[j + 1],")")){//���� ��ū ���� ) ���
					j = j + 1;
					while(!strcmp(tokens[j], ")")){
						sub_rcount++;
						j++;
					}
				}
				else 
					return false;//������ ����, 0�� �ο�

				if(sub_lcount != sub_rcount)
					return false;//������ ����, 0�� �ο�
				
				strcpy(tokens[start + 2], tokens[start + 2 + sub_lcount]);	
				for(int i = start + 3; i<TOKEN_CNT; i++)//��ū ����
					memset(tokens[i], 0, sizeof(tokens[0]));

			}
			strcat(tokens[start - lcount], tokens[start]);
			strcat(tokens[start - lcount], tokens[start + 1]);
			strcat(tokens[start - lcount], tokens[start + rcount + 1]);
		 
		 	//��ū ����
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount -1; i++) {
				strcpy(tokens[i], tokens[i + lcount + rcount +1]);
				memset(tokens[i + lcount + rcount + 1], 0, sizeof(tokens[0]));

			}
		}
	}
	return true;
}

void clear_tokens(char tokens[TOKEN_CNT][MINLEN])//��ū ��� ���� 0���� �ʱ�ȭ
{
	int i;

	for(i = 0; i < TOKEN_CNT; i++)
		memset(tokens[i], 0, sizeof(tokens[i]));
}

char *rtrim(char *_str) // ������ ���� ����
{
	char tmp[BUFLEN];
	char *end;

	strcpy(tmp, _str);
	end = tmp + strlen(tmp) - 1;//end �迭�� ���ڿ� ���κ� ����
	while(end != _str && isspace(*end))
		--end;

	*(end + 1) = '\0'; // ������ �� �κп� \0�� �������Ͽ� �������ϱ�
	_str = tmp;
	return _str;//������ ������ ���ŵ� char ������ ��ȯ
}

char *ltrim(char *_str) // ���� ���� ����
{
	char *start = _str;

	while(*start != '\0' && isspace(*start))//���ڿ� ���� �ƴϰ� start������ ���� �����̶�� ����
		++start;
	_str = start;
	return _str; // ���� ������ ���ŵ� char ������ ��ȯ
}

char* remove_extraspace(char *str)//���� ���� �Լ�
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
	
		strncat(temp, str, position);//include ����
		strncat(temp, " ", 1);//" "�̾� ���̱�
		strncat(temp, str + position, strlen(str) - position + 1);//< ~~~  >�̾� ���̱�

		str = temp;	//str�� include<~~~> -> include <~~~>�� �ٲٱ�
	}
	
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] ==' ')//������ �߰� �ȴٸ�
		{
			if(i == 0 && str[0] ==' ')
				while(str[i + 1] == ' ')
					i++;//���� �н�	
			else{
				if(i > 0 && str[i - 1] != ' ')//���� ���� ������ �ƴ϶��
					str2[strlen(str2)] = str[i];
				while(str[i + 1] == ' ')
					i++;//���� ����
			} 
		}//������ �ƴ϶��
		else
			str2[strlen(str2)] = str[i];
	}

	return str2;
}



void remove_space(char *str) // �߰� �߰��� �����ϴ� ���� ����
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

int check_brackets(char *str) // '(' �� ')' �� ������ �ٸ��ٸ� ���������� �������� ���� ����. �� Ʋ�� ����
{
	char *start = str;
	int lcount = 0, rcount = 0;
	
	while(1){// '(' ����, ')'���� ���ϱ�
		if((start = strpbrk(start, "()")) != NULL){// '(', ')'��ȣ�� �����ϸ�
			if(*(start) == '(')
				lcount++;
			else
				rcount++;

			start += 1; 		
		}
		else
			break;
	}

	if(lcount != rcount)//¦�� ���� ������ �������� ����
		return 0;
	else //¦�� �¾ƾ� true ��ȯ
		return 1;
}

int get_token_cnt(char tokens[TOKEN_CNT][MINLEN])//��ū�� ���� ���ϴ� �Լ�
{
	int i;
	
	for(i = 0; i < TOKEN_CNT; i++)
		if(!strcmp(tokens[i], ""))
			break;

	return i;
}
