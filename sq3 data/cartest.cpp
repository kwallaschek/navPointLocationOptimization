/*	pg16sim1.cpp
##########
���ԏ�ē��p
�Z���I�[�g�}�g���V�~�����[�^
##########
	Teruomi KATORI
	2007 Katori Lab. all rights reserved
	9/nov/07 ==> 12/nov/07		�V�~�����[�^��{��
	10/feb/09 ==> 13/feb/09		�o�H�T�����̕��
	26/apr/16					�p�x�v�Z�̒ǉ�

���s�ɂ�
 LINK_FILE, CAR_FILE, PARK_FILE
���K�v�ł��B
1 cell = 5[m], 1 unit time = 0.5[sec] because v=36[km/h]
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define START 4		//s[]=�����_�̗v�f��
#define GOAL 2			//g[]=�ړI�n�̗v�f��

int sel,sum_p;

/*
==============================
���C���֐�
==============================
*/
void main()	
{

	int make_cardata();
	printf("\n�ԗ��������@�̌���(������I��)\n");
	printf("->1.test \n");
	scanf("%d", &sel);

	make_cardata();
}
	

/*�ԗ��f�[�^�쐬�̎d��*/
int test()		//�����_�����_���@�ړI�n3�_�p�@���������K�v����
{
	FILE *outputfile;         // �o�̓X�g���[��

	outputfile = fopen("test1.txt", "w");  // �t�@�C�����������ݗp�ɃI�[�v��(�J��)

	int i, j,k,l, r, a, b, c, d,p, cnt;
	int s[START] = { 0,1,2,3 };	//�����o�_�m�[�h
	int g[GOAL] = {23,35};		//�ړI�n�m�[�h
	i = 0;
	j = 0;
	cnt = 0;
	srand((unsigned)time(NULL));

	while(1)
	{
		a = rand() % START;
		b = rand() % GOAL;
		c = s[a]; //���o���n�_
		d = g[b]; //�ړI�n

		if(b==0)
		{
			if(i<40)		//g[0]�̖ړI�n�ɂ����Ԃ̃L���p�V�e�B���t�߂̒��ԏ�̒��ԑ䐔
			{
				fprintf(outputfile, "%d\t%d\t%d\t1200\n", cnt, c, d); // �t�@�C���ɏ��� ��������,���o��,�ړI�n,���v����
				i++;
				cnt = cnt + 4;
			}
		}else if(b==1)
		{
			if(j<40)		//g[1]�̖ړI�n�ɂ����Ԃ̃L���p�V�e�B���t�߂̒��ԏ�̒��ԑ䐔
			{
				fprintf(outputfile, "%d\t%d\t%d\t1200\n", cnt, c, d); // �t�@�C���ɏ��� ��������,���o��,�ړI�n,���v����
				j++;
				cnt = cnt + 4;
			}
		}
		sum_p=i+j;
		if(sum_p==80)			//���v
		{
			break;		
		}
	}

	fclose(outputfile);          // �t�@�C�����N���[�Y(����)
	return sum_p;
}
/*�ԗ��f�[�^�쐬*/
/*int make_cardata()
{
	int i;
	if (sel == 1)
	{
		all();
		return 0;
	}
	else if (sel == 2) {

		half();
		return 0;
	}
	else if (sel == 3) {

		turn();
		return 0;
	}
	else if (sel == 4) {

		twice();
		return 0;
	}
	else if (sel == 5) {

		random();
		return 0;
	}
	else {

		printf("error");
		exit(0);
	}
	return (sel);
}*/

	int make_cardata()
	{
		switch(sel)
		{
			case 1:
				test();
				break;
			default:
				printf("error");
				exit(0);
		}

		return sel;
	}

