/* 	pg16sim1.cpp
##########
for guide parking 
cell automaton simulater
##########
	Teruomi KATORI
	2007 Katori Lab. all rights reserved
	9/nov/07 ==> 12/nov/07		シミュレータ基本部
	10/feb/09 ==> 13/feb/09		経路探索部の補綴
	26/apr/16					角度計算の追加

Execution is need " LINK_FILE, CAR_FILE, PARK_FILE "

1 cell = 5[m], 1 unit time = 0.5[sec] because v=36[km/h]
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "sim05test.h"

#define LINK_FILE	"sq3_data/link_sq3.txt"
#define CAR_FILE06	"sq3_data/test1.txt"
#define PARK_FILE	"sq3_data/park_sq3.txt"
#define LXY_FILE	"sq3_data/lxy_sq3.txt"
//#define NAVI_FILE   "navi_sq3.txt"	//additions of guide limit
#define LOCATE_FILE	"sq3_data/location_sq3.txt"	//additions of guide limit
#define MAX_LINK 104
#define MAX_NODE 44
#define MAX_CAR 80
#define MAX_PARK 4
#define MAX_NAVI 44		// additions of guide limit
#define MAX_TIME 10000	// =[sec]
#define MAX_LENGTH 1000	// = 500[m]
#define MAX_PATH 52
#define G_METHOD 1		//guide parking method(0-7)use or not 2bit:parking manager、1bit:road manager、0bit:driver、angle is need always 	
#define BEACON 0		//Beacon installation link 0:dont 1:According to data 2:all link　
#define ANNNAI 0		//guide limit　0:dont　1:do
#define CHECK 1			//for confirmation vehicle's trip time 0:dont　1:do

struct link{
	int s;	//flowin flowout
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
	int path[MAX_PATH];	//driving path
	int gbf;			//go or back flag. -1:stop 0:go, 1:park, 2:back, 3:arrive
};

struct park{
	int nnum;
	int capacity;
	int more;
};

/*struct navi{
	//int nn;  要らなかったやつ

	int co;
	int na[44];
};*/




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
メイン関数
==============================
*/
// You receive multiple locations not only one!
// So simulate receives multiple locations -> numLocations many
// -> loc is an array of size numLocations
// -> loc = loc[numLocations]
float simulate(location* locations, int numLocations)	
{
	void init_struct(void);
	void file_read(void);
	void flowout(float);
	void start(float);
	void parkout(float);
	float calc_ttime(void);
	void check(void);
	float ttave;
	float t;
	int i,j,k;

	init_struct();	//	Initialize of struct
	file_read();	//	read file
	for (int i=0; i<MAX_NAVI; i++){
		loc[i] = locations[i];
	}
	
	for(t=0;t<MAX_TIME;t+=0.5)
	{
		flowout(t);	//	flowout process
		i=0;
		while(i<MAX_LINK && l[i].s!=-1)
		{
			/*	move next vacant cell	*/
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
		start(t);	
		parkout(t);	
			//printf("\n");printf("22 = %d\n",p[0].capacity-p[0].more);if(t>=1000){getchar();}
			

	/*	for(i=0;i<MAX_CAR;i++)// check path 
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
	ttave = calc_ttime();	//	average trip time calculate
	//getchar();
	if(CHECK==1){	// each vehicle's trip time
		check();
	}
	//fclose(ch);
	return ttave;

}

/*	initialize struct	*/
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

	/*for(i=0;i<MAX_NAVI;i++)//追加分の初期化 
	{
		v[i].co=-1;
		for(j=0;j<44;j++)
		{
			v[i].na[j]=-1;
		}
	}*/

	/*for(i=0;i<MAX_NAVI;i++)		
	{
		loc[i].x=-1;
		loc[i].y=-1;
		loc[i].cn=-1;
		for(j=0;j<44;j++)
		{
			loc[i].np[j]=-1;
		}
	}
	*/

}

/*	read file	*/
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

	/*i=0;
	j=0;
	fp=fopen(LOCATE_FILE,"r");
	
	// ->>> REPLACED in simulate function
	while(i<MAX_NAVI && EOF!=fscanf(fp,"%d %d %d",&loc[i].x,&loc[i].y,&loc[i].cn))
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
*/
}

/*	intersection flowout processing	*/
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
			if(j==MAX_PATH)	//	remove vehicle that arrived at destination 
			{
				continue;
			}
			if(c[cnum].path[j+1]==-1)
			{
//printf("着");
				if(c[cnum].gbf==2 && c[cnum].path[j]==c[cnum].s)	//	if vehicle return origin
				{
					l[i].cell[l[i].l/5]=-1;
					c[cnum].path[j]=-1;
					c[cnum].tt=t-c[cnum].tt;
					c[cnum].gbf=3;
					
//printf("戻");
//getchar();
//getchar();
					continue;
				}
				if(c[cnum].gbf==0 && c[cnum].path[j]==c[cnum].pl)	//	if vehicle arrived at parking
				{
					pnum=search_pnum(c[cnum].pl);
					if(p[pnum].more>0)		//	if vehicle can park in the parking
					{
						l[i].cell[l[i].l/5]=-1;
						c[cnum].gbf=1;
						c[cnum].path[j]=-1;
						
						dijkstra(c[cnum].pl,&distance[0],&flag[0],&before[0]);
						c[cnum].st=t+c[cnum].pt+(float)distance[c[cnum].t]*2*3600/4000;	//最後の項は徒歩往復時間
						p[pnum].more-=1;
//printf("可 %4.1f = %4.1f + %4.1f + %4.1f",c[cnum].st,t,c[cnum].pt,(float)distance[c[cnum].t]*2*3600/4000);
					}else{					//	if parking is full
						c[cnum].pl=det_park(c[cnum].pl,c[cnum].t);	//	as new c[cnum].pl	search other parking
						copy_route(cnum,p[pnum].nnum,c[cnum].pl);	//	copy route function from c[cnum].path[j] to c[cnum].pl
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
			}else{	//	intersection flowout
				if(c[cnum].gbf==0 && l[i].b==1)	// Only installation beacon
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
//printf(" 換 %d : %d %d %d %d %d %d %d",cnum,c[cnum].path[0],c[cnum].path[1],c[cnum].path[2],c[cnum].path[3],c[cnum].path[4],c[cnum].path[5],c[cnum].path[6]);
				continue;
			}
		}
	}
}

/*	start	*/
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
			c[j].pl=det_park(c[j].s,c[j].t);		//	decide parking function
			//printf("%d",c[j].pl);	
			copy_route(j,c[j].s,c[j].pl);	//	copy route function from c[j].s to c[j].pl
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

/*	leave  paking	*/
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
			copy_route(j,c[j].pl,c[j].s);	//	copy route funciton from c[j].pl to c[j].s
			lnum=search_lnum(c[j].path[0],c[j].path[1]);
			if(l[lnum].cell[0]==-1)	//
			{
				pnum=search_pnum(c[j].path[0]);
				p[pnum].more+=1;
				l[lnum].cell[0]=j;
				c[j].path[0]=-1;

				c[j].gbf=2;
//printf(" 出庫 %d : %d %d %d %d %d %d %d",j,c[j].path[0],c[j].path[1],c[j].path[2],c[j].path[3],c[j].path[4],c[j].path[5],c[j].path[6]);
			}else{	//stop
				c[j].st+=0.5;

			}
		}
		j+=1;
	}
}

/*	search link's number	*/
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

/*	search parking number	*/
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

/*	Average trip time calculation	*/
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

/*	Copy of future traveling route	*/
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
search shortest path by dijkstra method 
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
Initialization of route search array
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
branching
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
search minimum weight
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
decide parking function
Finally guide number of parking 
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

	//printf("%d,%d",now,terminal); //for test

	for(i=0;i<MAX_PARK;i++)
	{
		pprob[i]=0;
		for(j=0;j<5;j++)
		{
			cond[i][j]=0;
		}
	}
	//	conditions1 f1
	dijkstra(now,&distance[0],&flag[0],&before[0]);
//	printf(" %f",cond[i][0]);
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][1]=(float)distance[p[i].nnum];
		//printf("cond[%d][1]=%lf",i,cond[i][1]);
		//getchar();
	}
	//	conditions0 f0
	dijkstra(terminal,&distance[0],&flag[0],&before[0]);
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][0]=(float)distance[p[i].nnum];
	}
	//	conditions2 f2
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][2]=(float)((p[i].capacity-p[i].more)/p[i].capacity);
	}
	//	conditions3 f3
	for(i=0;i<MAX_PARK;i++)
	{
		cond[i][3]=(float)degree(now,p[i].nnum,terminal);
	}
	//additions conditions
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

	//。If it can not be assigned anywhere, guide to nearest parking to destination
	//In the parking nearest to the destination, there is a traffic jam queue.
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

/*	angle calculation	*/
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

/*	Degree of guiding to the parking lot by fuzzy calculation	*/
float fuzzy(float cond[MAX_PARK][5],int i)
{
	float f0(float);
	float f1(float);
	float f2(float);
	float f3(float);
	float f4(float,float);// additions of guide limit
	float min(float []);
	float prob[5]={999,999,999,999,999};

	prob[0]=f0(cond[i][0]);	//	distance from destination to parking
	prob[1]=f1(cond[i][1]);	//	distance from origins to parking
	prob[2]=f2(cond[i][2]);	//	parking occupancy
	prob[3]=f3(cond[i][3]);	//	angle between destination and parking
	if(ANNNAI==1){
		prob[4]=f4(cond[i][4],prob[0]); // for guide limit
		//printf("%d",(int)cond[i][4]);
		//getchar();
		//printf("prov[4]=%d",(int)prob[4]);
		//getchar();
	}
//	printf("%f\n",prob[0]);
	return min(prob);
}

/*	search minimum degree	*/
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

/*	driver (want to nearest parking to the destination) */
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

/*	manager(dont want to run much)	*/
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

/*	parking(no difference usage status)*/
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

/* angle between parking and destination from now location	*/
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

/*additions of guide limit process*/
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
		//a=f0(b);// nearest parking
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

/*for confirmation each vehicle's trip time */
void check(void)
{
	
	int i;
	FILE *outputfile;         // output stream
  
	outputfile = fopen("check-out.txt", "w");  // open writing file
	
		for(i=0;i<MAX_CAR;i++){
			fprintf(outputfile, "%f %d\n",c[i].tt,c[i].pl); // write
		}	
	fclose(outputfile);          // file close 
	
}

/* dont use
void route(int cnum int j)
{
	int i;
	for(i=j;i>=0;i--)
	{
		fprintf(ch,"%s,%d\n",c[cnum],c[cnum].path[j]);
	}
	fclose(ch);
}*/

