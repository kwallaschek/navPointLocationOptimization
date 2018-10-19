/*	pg16sim1.cpp
##########
駐車場案内用
セルオートマトンシミュレータ
##########
	Teruomi KATORI
	2007 Katori Lab. all rights reserved
	9/nov/07 ==> 12/nov/07		シミュレータ基本部
	10/feb/09 ==> 13/feb/09		経路探索部の補綴
	26/apr/16					角度計算の追加

実行には
 LINK_FILE, CAR_FILE, PARK_FILE
が必要です。
1 cell = 5[m], 1 unit time = 0.5[sec] because v=36[km/h]
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define START 4		//s[]=流入点の要素数
#define GOAL 2			//g[]=目的地の要素数

int sel,sum_p;

/*
==============================
メイン関数
==============================
*/
void main()	
{

	int make_cardata();
	printf("\n車両流入方法の決定(数字を選択)\n");
	printf("->1.test \n");
	scanf("%d", &sel);

	make_cardata();
}
	

/*車両データ作成の仕方*/
int test()		//流入点ランダム　目的地3点用　内部調整必要あり
{
	FILE *outputfile;         // 出力ストリーム

	outputfile = fopen("test1.txt", "w");  // ファイルを書き込み用にオープン(開く)

	int i, j,k,l, r, a, b, c, d,p, cnt;
	int s[START] = { 0,1,2,3 };	//流入出点ノード
	int g[GOAL] = {23,35};		//目的地ノード
	i = 0;
	j = 0;
	cnt = 0;
	srand((unsigned)time(NULL));

	while(1)
	{
		a = rand() % START;
		b = rand() % GOAL;
		c = s[a]; //流出入地点
		d = g[b]; //目的地

		if(b==0)
		{
			if(i<40)		//g[0]の目的地にいく車のキャパシティ＝付近の駐車場の駐車台数
			{
				fprintf(outputfile, "%d\t%d\t%d\t1200\n", cnt, c, d); // ファイルに書く 流入時間,流出入,目的地,所要時間
				i++;
				cnt = cnt + 4;
			}
		}else if(b==1)
		{
			if(j<40)		//g[1]の目的地にいく車のキャパシティ＝付近の駐車場の駐車台数
			{
				fprintf(outputfile, "%d\t%d\t%d\t1200\n", cnt, c, d); // ファイルに書く 流入時間,流出入,目的地,所要時間
				j++;
				cnt = cnt + 4;
			}
		}
		sum_p=i+j;
		if(sum_p==80)			//合計
		{
			break;		
		}
	}

	fclose(outputfile);          // ファイルをクローズ(閉じる)
	return sum_p;
}
/*車両データ作成*/
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

