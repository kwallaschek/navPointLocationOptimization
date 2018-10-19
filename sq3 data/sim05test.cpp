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

#define LINK_FILE	"link_sq3.txt"
#define CAR_FILE06	"test1.txt"
#define PARK_FILE	"park_sq3.txt"
#define LXY_FILE	"lxy_sq3.txt"
//#define NAVI_FILE   "navi_sq3.txt"	//�ǉ��̈ē�����
#define LOCATE_FILE	"location_sq3.txt"
#define MAX_LINK 104
#define MAX_NODE 44
#define MAX_CAR 80
#define MAX_PARK 4
#define MAX_NAVI 44		//�ǉ��̈ē������̉ӏ�
#define MAX_TIME 10000	// =[sec]
#define MAX_LENGTH 1000	// = 500[m]
#define MAX_PATH 52
#define G_METHOD 1		//���ԏ�ē����@(0-7) 2bit:���ԏ�Ǘ��ҁA1bit:���H�Ǘ��ҁA0bit:�h���C�o�[�A�p�x�͕K���̗p 	
#define BEACON 0		//�r�[�R���ݒu�����N 0:�Ȃ� 1:�f�[�^�ǂ��� 2:�S�����N�@
#define ANNNAI 0		//�ē������@0:�Ȃ��@1:����
#define CHECK 1			//���ԍ��m�F�p 0:�Ȃ��@1:����

struct link{
	int s;	//���o��
	int t;	//[s]
	int l;	//[m]
	int b;	//beacon
	int cell[MAX_LENGTH/5+1];
};

struct node{
	double x; 
	double y;
};

struct car{
	float st;			//start time
	int s;	int t;
	int pl;				//park location
	float pt;			//park time
	float tt;			//trip time;
	int path[MAX_PATH];	//�����Ă����o�H�i�����_�ԍ���j
	int gbf;			//go or back flag. -1:stop 0:go, 1:park, 2:back, 3:arrive
};

struct park{
	int nnum;
	int capacity;
	int more;
};

/*struct navi{
	//int nn;  �v��Ȃ��������

	int co;
	int na[44];
};*/

struct location{
	int x;
	int y;
	int cn;
	int np[44];
	bool finished;
};


struct link l[MAX_LINK];
struct node n[MAX_NODE];
struct car	c[MAX_CAR];
struct park p[MAX_PARK];
//struct navi v[MAX_NAVI];
struct location loc[MAX_NAVI];

int search_lnum(int,int);
int search_pnum(int);


int distance[MAX_NODE],flag[MAX_NODE],before[MAX_NODE];

/*
==============================
���C���֐�
==============================
*/
float simulate(location loc, int numNodes)	
{
	void init_struct(void);
	void file_read(void);
	void flowout(float);
	void start(float);
	void parkout(float);
	float calc_ttime();
	void check(void);
	float ttave;
	float t;
	int i,j,k;
	init_struct();	//	�\���̂̏�����
	file_read();	//	�t�@�C������̃f�[�^�ǂݏo��

	
	for(t=0;t<MAX_TIME;t+=0.5)
	{
		flowout(t);	//	�����_����̗��o����
		i=0;
		while(i<MAX_LINK && l[i].s!=-1)
		{
			/*	�ׂ̋󂫃Z���֑O�i	*/
			for(j=(int)l[i].l/5-1;j>=0;j--)
			{
				
				if(l[i].cell[j+1]==-1)
				{
					l[i].cell[j+1]=l[i].cell[j];
					l[i].cell[j]=-1;
				}
			}
			
			i+=1;
		}
		start(t);	//	�o��
		parkout(t);	//	���ԏꂩ��̏o��
			//printf("\n");printf("22 = %d\n",p[0].capacity-p[0].more);if(t>=1000){getchar();}
			

	/*	for(i=0;i<MAX_CAR;i++)//�p�X���̊m�F�p
		{
			printf("\n%d",i);
			for(j=0;j<MAX_PATH;j++)
			{
				printf(" %2d",c[i].path[j]);
			}
		}*/
		/*for(i=0;i<MAX_CAR;i++)
		{
			printf(" %f",c[i].tt);
		}*/
	//	getchar();
	}
	ttave = calc_ttime();	//	���ϗ��s���Ԃ̌v�Z
	//getchar();
	if(CHECK==1){//���ԍ��m�F
		check();
	}
	//fclose(ch);
	return ttave;

}

/*	�\���̂̏�����	*/
void init_struct(void)
{
	int i,j;
	for(i=0;i<MAX_LINK;i++)
	{
		l[i].s=l[i].t=l[i].l=l[i].b=-1;
		for(j=0;j<MAX_LENGTH/5+1;j++)
		{
			l[i].cell[j]=-1;
		}
	}
	for(i=0;i<MAX_NODE;i++)
	{
		n[i].x=n[i].y=-1;
	}
	for(i=0;i<MAX_CAR;i++)
	{
		c[i].st=c[i].pt=-1;
		c[i].s=c[i].t=c[i].pl=c[i].gbf=-1;
		c[i].tt=-1;
		for(j=0;j<MAX_PATH;j++)
		{
			c[i].path[j]=-1;
		}
	}
	for(i=0;i<MAX_PARK;i++)
	{
		p[i].nnum=p[i].capacity=p[i].more=-1;
	}

	/*for(i=0;i<MAX_NAVI;i++)//�ǉ����̏�����
	{
		v[i].co=-1;
		for(j=0;j<44;j++)
		{
			v[i].na[j]=-1;
		}
	}*/

	for(i=0;i<MAX_NAVI;i++)		//test
	{
		loc[i].x=-1;
		loc[i].y=-1;
		loc[i].cn=-1;
		for(j=0;j<44;j++)
		{
			loc[i].np[j]=-1;
		}
	}

}

/*	�t�@�C������̃f�[�^�ǂݏo��	*/
void file_read()
{
	int i,num;

	FILE *fp;

	i=0;
	fp=fopen(LINK_FILE,"r");
	while(i<MAX_LINK && EOF!=fscanf(fp," %d %d %d %d",&l[i].s,&l[i].t,&l[i].l,&l[i].b))
	{
		#if BEACON==0
			l[i].b=0;
		#endif
		#if BEACON==2
			l[i].b=1;
		#endif
		i+=1;
	}
	fclose(fp);

	fp=fopen(LXY_FILE,"r");
	while(i<MAX_NODE && EOF!=fscanf(fp," %d %lf %lf",&num,&n[i].x,&n[i].y))
	{
		i+=1;
	}
	fclose(fp);

	i = 0;
		fp = fopen(CAR_FILE06, "r");
		while (i<MAX_CAR && EOF != fscanf(fp, " %f %d %d %f", &c[i].st, &c[i].s, &c[i].t, &c[i].pt))
		{
			i += 1;
		}
		fclose(fp);

	i=0;
	fp=fopen(PARK_FILE,"r");
	while(i<MAX_PARK && EOF!=fscanf(fp,"%d %d",&p[i].nnum,&p[i].capacity))
	{
		p[i].more=p[i].capacity;
		i+=1;
	}
	fclose(fp);
	
	//i=0;
	int j;
	/*fp=fopen(NAVI_FILE,"r");
	while(EOF!=fscanf(fp," %d",&v[i].co) && i<MAX_NAVI)
	{
		for(j=0;j<v[i].co;j++)
		{
			
			fscanf(fp,"%d",&v[i].na[j]);
			//printf("%d  \n",v[i].na[j]);
		}
		i+=1;
	}
	fclose(fp);*/

	i=0;
	j=0;
	fp=fopen(LOCATE_FILE,"r");
	while(i<MAX_NAVI && EOF!=fscanf(fp,"%d %d %d",&loc[i].x,&loc[i].y,&loc[i].cn))		//test
	{
			//printf("%d  \n",loc[i].x);
			for(j=0;j<loc[i].cn;j++)
			{
			
				fscanf(fp,"%d",&loc[i].np[j]);
				//printf("%d  \n",loc[i].np[j]);
			}
			i+=1;
	}
	fclose(fp);

}

/*	�����_����̗��o����	*/
void flowout(float t)
{
	void dijkstra(int,int *,int *,int *);
	int det_park(int,int);
	void copy_route(int,int,int);
	int cnum,pnum,lnum;
	int i=0,j;

	for(i=0;i<MAX_LINK;i++)
	{
		if(l[i].s==-1)
		{
			break;
		}
		cnum=l[i].cell[l[i].l/5];
		if(cnum!=-1)
		{
//printf(" %d",cnum);
			j=0;
			while(j<MAX_PATH && c[cnum].path[j]==-1)
			{
				j+=1;
			}
			if(j==MAX_PATH)	//	�����ςݎԗ��͂͂���
			{
				continue;
			}
			if(c[cnum].path[j+1]==-1)
			{
//printf("��");
				if(c[cnum].gbf==2 && c[cnum].path[j]==c[cnum].s)	//	�o���n�ɖ߂����ꍇ
				{
					l[i].cell[l[i].l/5]=-1;
					c[cnum].path[j]=-1;
					c[cnum].tt=t-c[cnum].tt;
					c[cnum].gbf=3;
					
//printf("��");
//getchar();
//getchar();
					continue;
				}
				if(c[cnum].gbf==0 && c[cnum].path[j]==c[cnum].pl)	//	���ԏ�ɓ��������ꍇ
				{
					pnum=search_pnum(c[cnum].pl);
					if(p[pnum].more>0)		//	���ԏ�ɒ�߂���ꍇ
					{
						l[i].cell[l[i].l/5]=-1;
						c[cnum].gbf=1;
						c[cnum].path[j]=-1;
						
						dijkstra(c[cnum].pl,&distance[0],&flag[0],&before[0]);
						c[cnum].st=t+c[cnum].pt+(float)distance[c[cnum].t]*2*3600/4000;	//�Ō�̍��͓k����������
						p[pnum].more-=1;
//printf("�� %4.1f = %4.1f + %4.1f + %4.1f",c[cnum].st,t,c[cnum].pt,(float)distance[c[cnum].t]*2*3600/4000);
					}else{					//	���ԏꂪ���Ԃ̏ꍇ
						c[cnum].pl=det_park(c[cnum].pl,c[cnum].t);	//	�Vc[cnum].pl�Ƃ��ĕʁi�ߗׁH�j�̒��ԏ������i�T���j
						copy_route(cnum,p[pnum].nnum,c[cnum].pl);	//	c[cnum].path[j]����c[cnum].pl�܂ł̌o�H�R�s�[�֐�
						lnum=search_lnum(c[cnum].path[0],c[cnum].path[1]);
						if(l[lnum].cell[0]==-1)
						{
							l[lnum].cell[0]=cnum;
							l[i].cell[l[i].l/5]=-1;
							
							c[cnum].path[0]=-1;
						}else{
							c[cnum].st+=0.5;
						
						}
					}
					continue;
				}
			}else{	//	�P�Ȃ�����_���o
				if(c[cnum].gbf==0 && l[i].b==1)	//�r�[�R���ݒu�ӏ��̂�
				{
					c[cnum].pl=det_park(l[i].t,c[cnum].t);
					copy_route(cnum,l[i].t,c[cnum].pl);
					j=0;
				}
				lnum=search_lnum(c[cnum].path[j],c[cnum].path[j+1]);
				if(l[lnum].cell[0]==-1)
				{
					l[lnum].cell[0]=cnum;
					l[i].cell[l[i].l/5]=-1;
					c[cnum].path[j]=-1;
				
				}
//printf(" �� %d : %d %d %d %d %d %d %d",cnum,c[cnum].path[0],c[cnum].path[1],c[cnum].path[2],c[cnum].path[3],c[cnum].path[4],c[cnum].path[5],c[cnum].path[6]);
				continue;
			}
		}
	}
}

/*	�o��	*/
void start(float t)
{
	int det_park(int, int);
	void copy_route(int,int,int);
	int lnum;
	int j=0;

	while(j<MAX_CAR && c[j].st!=-1)
	{

		if(c[j].st==t && c[j].pl==-1)
		{
			//printf("a");
			c[j].pl=det_park(c[j].s,c[j].t);		//	���ԏ�̌���֐�
			//printf("%d",c[j].pl);	
			copy_route(j,c[j].s,c[j].pl);	//	c[j].s����c[j].pl�܂ł̌o�H�R�s�[�֐�
			lnum=search_lnum(c[j].path[0],c[j].path[1]);
			if(l[lnum].cell[0]==-1)
			{
				l[lnum].cell[0]=j;
				c[j].tt=t;
				c[j].path[0]=-1;
				c[j].gbf=0;
			}else{
				c[j].st+=0.5;
				c[j].tt+=0.5;
				c[j].pl=-1;						
			}
		}
		j+=1;
	}
}

/*	���ԏꂩ��̏o��	*/
void parkout(float t)
{
	void copy_route(int,int,int);
	int lnum,pnum;
	int j=0;

	while(j<MAX_CAR && c[j].st!=-1)
	{
//		printf("*");
		if(c[j].gbf==1 &&  c[j].st<=t && c[j].s!=c[j].path[0])
		{
//			printf("*");
//			getchar();
			copy_route(j,c[j].pl,c[j].s);	//	c[j].pl����c[j].s�܂ł̌o�H�R�s�[�֐�
			lnum=search_lnum(c[j].path[0],c[j].path[1]);
			if(l[lnum].cell[0]==-1)	//�o��
			{
				pnum=search_pnum(c[j].path[0]);
				p[pnum].more+=1;
				l[lnum].cell[0]=j;
				c[j].path[0]=-1;

				c[j].gbf=2;
//printf(" �o�� %d : %d %d %d %d %d %d %d",j,c[j].path[0],c[j].path[1],c[j].path[2],c[j].path[3],c[j].path[4],c[j].path[5],c[j].path[6]);
			}else{	//���~��
				c[j].st+=0.5;

			}
		}
		j+=1;
	}
}

/*	�����N�ԍ��̌���	*/
int search_lnum(int s,int t)
{
	int i;
	for(i=0;i<MAX_LINK;i++)
	{
		if(l[i].s==s && l[i].t==t)
		{
			break;
		}
	}
	return i;
}

/*	���ԏ�ԍ��̌���	*/
int search_pnum(int nnum)
{
	int i=0;
	while(i<MAX_PARK && p[i].nnum!=-1)
	{
		if(p[i].nnum==nnum)
		{
			break;
		}
		i+=1;
	}
	return i;
}

/*	���ϗ��s���Ԃ̌v�Z	*/
float calc_ttime()
{
	int i = 0;
	float ttave = 0;
	/*if(sel==6)
	{
		while(i<sum_p && c[i].tt!=0)
	{
		ttave+=c[i].tt;
		i+=1;
	}
	}else
	{*/
		while(i<MAX_CAR && c[i].tt!=0)
		{
			ttave+=c[i].tt;
			i+=1;
		}
	//}
	ttave/=(float)i;
	return ttave;
}

/*	���ꂩ��̑��s���[�g���R�s�[	*/
void copy_route(int cnum,int tmp,int terminal)
{

	void dijkstra(int,int *,int *,int *);
	int j,i;
	dijkstra(terminal,&distance[0],&flag[0],&before[0]);
	for(j=0;j<MAX_PATH;j++)
	{
		c[cnum].path[j]=-1;
	}
	i=MAX_CAR;
	j=0;
    do
	{
		c[cnum].path[j]=tmp;
		tmp=before[tmp];
		j+=1;
	}
	while(tmp!=-1 && j<MAX_PATH);
	/*if(i>=j)
		{
			fprintf(ch,"%s,%d\n",c[cnum],c[cnum].path[j]);
		}*/
}

/*
==============================
dijkstra�@�ɂ��ŒZ�p�X�T��
==============================
*/
void dijkstra(int s,int *distance,int *flag,int *before)
{
	void first_dim(int *,int *f,int *);
	void grow_branch(int,int *,int *,int *);
	int sort_min(int *,int *,int);
	int now=s;

	first_dim(distance,flag,before);
	*(distance+now)=0;
	*(flag+now)=2;
	while(now!=-1)
	{

		grow_branch(now,distance,flag,before);
//printf(" %d",now);
	now=sort_min(distance,flag,MAX_NODE);
	}
}

/*
------------------------------
�o�H�T���p�z��̏�����
------------------------------
*/
void first_dim(int *distance,int *flag,int *before)
{
	int i;
	for(i=0;i<MAX_NODE;i++)
	{
		*(distance+i)=9999;
		*(flag+i)=0;
		*(before+i)=-1;
	}
}

/*
------------------------------
�}�̂΂�
------------------------------
*/
void grow_branch(int now,int *distance,int *flag,int *before)
{
	int i=0;
	while(l[i].s!=-1 && i<MAX_LINK)
	{
		if(l[i].s==now && *(distance+l[i].t)>*(distance+l[i].s)+l[i].l && *(flag+l[i].t)!=2)
		{
//printf("*");
			*(distance+l[i].t)=*(distance+l[i].s)+l[i].l;
			*(flag+l[i].t)=1;
			*(before+l[i].t)=now;
		}
		i+=1;
	}
}

/*
------------------------------
�ŏ��̂��̂�T��
------------------------------
*/
int sort_min(int *number,int *flag,int loop_count)
{
	int min_point=-1;
	int min_number=9999;
	int i;

	for(i=0;i<loop_count;i++)
	{
		if( *(number+i)<min_number && *(flag+i)==1 )
		{
			min_number=*(number+i);
			min_point=i;
		}
	}
	if(min_point!=-1)
	{
		*(flag+min_point)=2;
	}
	return min_point;
}

/*
==============================
���ԏꌈ��֐�
�ŏI�I�Ɉē����钓�ԏ�̔ԍ�
==============================
*/
int det_park(int now,int terminal)
{
	void dijkstra(int,int *,int *,int *);
	float degree(int,int,int);
	float fuzzy(float [MAX_PARK][5],int);
	int pmax(float []);
	float cond[MAX_PARK][5];
	float pprob[MAX_PARK];
   	float maxprob=0;
	int maxnum=-1;
	int i,j;

	//printf("%d,%d",now,terminal); //�e�X�g�p

	for(i=0;i<MAX_PARK;i++)
	{
		pprob[i]=0;
		for(j=0;j<5;j++)
		{
			cond[i][j]=0;
		}
	}
	//	�����P
	dijkstra(now,&distance[0],&flag[0],&before[0]);
//	printf(" %f",cond[i][0]);
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][1]=(float)distance[p[i].nnum];
		//printf("cond[%d][1]=%lf",i,cond[i][1]);
		//getchar();
	}
	//	�����O
	dijkstra(terminal,&distance[0],&flag[0],&before[0]);
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][0]=(float)distance[p[i].nnum];
	}
	//	�����Q
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][2]=(float)((p[i].capacity-p[i].more)/p[i].capacity);
	}
	//	�����R
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][3]=(float)degree(now,p[i].nnum,terminal);
	}
	//�ǉ���
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][4]=now;
		//printf("cond[%d][4]=%d",i,(int)cond[i][4]);
		//getchar();
	}

	for(i=0;i<MAX_PARK;i++)
	{
		pprob[i]=fuzzy(cond,i);
//		printf(" %f",pprob[i]);
		if(maxprob<pprob[i])
		{
			maxprob=pprob[i];
			maxnum=i;
		}
	}

	//�ǂ��ɂ����蓖�Ă��Ȃ������ꍇ�́A�ړI�n�Ɉ�ԋ߂����ԏ�ցB
	//�ړI�n�Ɉ�ԋ߂����ԏ�ł͓��ɑ҂��a�؁B
	if(maxnum==-1)
	{
		maxprob=99999;
		for(i=0;i<MAX_PARK;i++)
		{
			if(cond[i][0]<maxprob)
			{
				maxprob=cond[i][0];
				maxnum=i;
			}
		}
	}
	return p[maxnum].nnum;
}

/*	�p�x�̌v�Z	*/
float degree(int now,int pnum,int goal)
{
	double ai,aj,bi,bj;
	float c;
	//if(l[now].l>=300)
	//{
		ai=(double)n[pnum].x-n[now].x;
		aj=(double)n[pnum].y-n[now].y;
		bi=(double)n[goal].x-n[now].x;
		bj=(double)n[goal].y-n[now].y;
		c=(float)acos((ai*bi+aj*bj)/sqrt(ai*ai+aj*aj)/sqrt(bi*bi+bj*bj));
		c=c*180.0/3.141592;
		return c;
	/*}else{
		ai=(double)n[pnum].x-n[now].x;
		aj=(double)n[pnum].y-n[now].y;
		bi=(double)n[goal].x-n[now].x;
		bj=(double)n[goal].y-n[now].y;
		c=(float)acos((ai*bi+aj*bj)/sqrt(ai*ai+aj*aj)/sqrt(bi*bi+bj*bj));
		c=c*360.0/3.141592;
		return c;
	}*/

}

/*	�t�@�W�C���Z�ɂ�邻�̒��ԏ�Ɉē�����x����	*/
float fuzzy(float cond[MAX_PARK][5],int i)
{
	float f0(float);
	float f1(float);
	float f2(float);
	float f3(float);
	float f4(float,float);//�ē������̒ǉ��p
	float min(float []);
	float prob[5]={999,999,999,999,999};

	prob[0]=f0(cond[i][0]);	//�ړI�n���璓�ԏ�܂ł̋���	
	prob[1]=f1(cond[i][1]);	//���������璓�ԏ�܂ł̋���
	prob[2]=f2(cond[i][2]);	//���ԏ��L��
	prob[3]=f3(cond[i][3]);	//�ړI�n�ƒ��ԏ�̂Ȃ��p�x
	if(ANNNAI==1){
		prob[4]=f4(cond[i][4],prob[0]); //�ē������p
		//printf("%d",(int)cond[i][4]);
		//getchar();
		//printf("prov[4]=%d",(int)prob[4]);
		//getchar();
	}
//	printf("%f\n",prob[0]);
	return min(prob);
}

/*	�ŏ��̓x���������߂�	*/
float min(float prob[5])
{
	float fmin=999;
	int i;
	for(i=0;i<5;i++)
	{
		if(fmin>prob[i])
		{
			fmin=prob[i];
		}
	}
//	printf("%f\n",fmin);
	return fmin;
}

/*	�h���C�o�[�i�ړI�n�ɋ߂��Ƃ���ɒ��Ԃ������j*/
float f0(float x)
{
	float p=1;
	#if (G_METHOD==0 || G_METHOD%2==1)
		p=-x/300+1;
	#endif
	if(p<0)
	{
		p=0;
	}
	return p;
}

/*	�Ǘ��ҁi���܂葖�点�����Ȃ��j	*/
float f1(float x)
{
	float p=1;
	#if G_METHOD%4>1
		p=-x/2000+1;
	#endif
	if(p<0)
	{
		p=0;
	}
	return p;
}

/*	���ԏ�i�΂�̂Ȃ����p�󋵁j*/
float f2(float x)
{
	float p=1;
	#if G_METHOD/4>0
		p=-x+1;
	#endif
	if(p<0)
	{
		p=0;
	}
	return p;
}

/*	���ݒn���璓�ԏ�ƖړI�n�̂Ȃ��p�x	*/
float f3(float x)
{
	float p;
	p=-x/90+1;
	if(p<0)
	{
		p=0;
	}
	return p;
}

/*�ǉ��̈ē��ӏ������̏���*/
float f4(float x,float a)
{
	//float f0(float);
	int i;
	//printf("test");
	//getchar();
	float p;
	float f0(float);
	//printf("%f",x);getchar();
	//for(x=0;x<MAX_NAVI;x++)
	//{
		//a=f0(b);//��ԋ߂����ԏ�
		//printf("a=%lf",a);
		//getchar();
		
		for(i=0;i<loc[(int)x].cn;i++)
		{
			p=0;
			//printf("v[%d].na[%d]=%d",(int)x,i,v[(int)x].na[i]);
			//getchar();
			if(loc[(int)x].np[i]!=-1 && a==1)
			{
				p=1;
				//printf("p=%d",p);
				//getchar();
			}//printf("%lf\n",p);

			
			return p;
			
			//printf("%d>>%d>>%d>>%lf\n",x,i,v[(int)x].na[i],p);
		}

		//getchar();
		
	//}
}

/*���ԍ��m�F�p*/
void check(void)
{
	
	int i;
	FILE *outputfile;         // �o�̓X�g���[��
  
	outputfile = fopen("check-out.txt", "w");  // �t�@�C�����������ݗp�ɃI�[�v��(�J��)
	
		for(i=0;i<MAX_CAR;i++){
			fprintf(outputfile, "%f %d\n",c[i].tt,c[i].pl); // �t�@�C���ɏ���
		}	
	fclose(outputfile);          // �t�@�C�����N���[�Y(����)
	
}

/*void route(int cnum int j)
{
	int i;
	for(i=j;i>=0;i--)
	{
		fprintf(ch,"%s,%d\n",c[cnum],c[cnum].path[j]);
	}
	fclose(ch);
}*/




//�ԗ��̃��[�g�@�e�L�X�g�o��
//g���\�b�h�ōŏ��bit(���ԏ�)���P�̂Ƃ��h���C�o�[�������_���Ŏw���ɏ]��Ȃ�
//�ړI�n�����䐔�Œ艻
	//NAVI�t�@�C���̃����N�f�[�^�C��




//g			1	5	5
//beacon	0	1	2
//annai		0	1	0
//		�@�Ȃ� komakaku zenchiten