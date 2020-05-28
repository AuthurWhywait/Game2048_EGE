/*
 * 项目名称：游戏2048
 * 开发日期：2020年5月6日
 * 开发团队：Whywait团队
 * 项目说明：
 *	1.操作说明：移动键盘方向键来控制游戏，ESC退出游戏，ENTER继续游戏，backspace返回上一步；
 *	2.游戏说明：出现2048即为赢得游戏，赢得游戏之后可以继续游戏；
 *	3.游戏特色：
 *		a. 更加刺激：每次滑动，都将滑动路径上的所有可以消除的方块全部消除；
 *		b. 2048以及以后的方块都另外设计，可解锁惊喜；
 *		c. 出现数值为16384的方块，即为赢得最终胜利，游戏结束；
 *		d. 随着得分的增加，分数字体的颜色会相应改变。分为 灰赤橙黄绿青蓝紫 八个等级；
 *		e. 随机出现2或4，出现2的概率设置为0.7；
 *		f. 撤销步数上限200，放开手脚，快乐玩耍！
 */

#include<graphics.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include"conio.h"

/*按键对应的键值*/
#define LEFT		294                                                                     //左键
#define UP			293                                                                     //上键
#define RIGHT		296                                                                     //右键
#define DOWN		295                                                                     //下键
#define ESC			27                                                                      //ESC 退出键
#define ENTER		13                                                                      //ENTER 继续键
#define REDO		8		                                                                //backspace 键撤销
/*窗口的属性*/
#define TITLESIZE	80                                                                      //标题大小
#define WIDTH		650                                                                     // 窗口的宽
#define HEIGHT		680                                                                     //窗口的高
/*游戏用户操控界面的属性*/
#define LINE		4                                                                       //4*4的方块
#define pos_X		(WIDTH-EDGE*LINE)/2+40                                                  //左上角的横坐标:左右居中
#define pos_Y		160                                                                     //左上角的纵坐标
#define EDGE		108                                                                     //每个方格的长度
/*分数所在位置的属性*/
#define FONTSIZE		28                                                                  //分数显示字符大小
#define POS_B1_WIDTH	pos_X	                                                            //分数方块左上角横坐标
#define POS_B1_HEIGHT	pos_Y-FONTSIZE-2                                                    //分数方块左上角纵坐标
/*游戏属性*/
#define REDO_TIMES		200                                                                 //游戏可返回上一步的次数上界（任意调节）

int score = 0, score0 = 0;			                                                        //分数计数器
int ismove = false;
COLORREF rainbow_color[8] = {                                                               //彩虹色表明分数等级：
	EGERGB(205, 201, 201),                                                                  //灰
	RED,                                                                                    //红
	EGERGB(255, 200, 0),                                                                    //橙
	EGERGB(255, 215, 0),                                                                    //黄
	GREEN,                                                                                  //绿
	EGERGB(64, 224, 208),                                                                   //青
	BLUE,                                                                                   //蓝
	EGERGB(160, 32, 240)                                                                    //紫
};
typedef struct myStack {                                                                    //类似栈的数据结构，在不同的方向的滑动函数中使用
	int stack[LINE];
	int top;
} myStack;

int index(int n) {                                                                          //为不同的数值n找到其对应的图片数组中的索引
	if (!n) return 0;
	int returnNum = 0;
	while (n != 1) {
		n /= 2;
		returnNum++;
	}
	return returnNum;
}

void num_to_string_10(int num, char* string) {                                              //将数字转化为字符串，适应xyprintf()
	int temp[10] = { 0 };
	int string_length = 0;
	for (; num > 0; string_length++) {
		temp[string_length] = num % 10;
		num = (num - temp[string_length]) / 10;
	}
	if (!string_length) {
		strcpy(string, "0");
		return;
	}
	string[string_length] = '\0';
	for (int i = 0; i < string_length; i++)
		string[i] = '0' + temp[string_length - i - 1];
}

int** init_nums() {                                                                         //初始化一个数组
	int** nums = (int**)malloc(sizeof(int*) * LINE);
	for (int i = 0; i < LINE; i++) {
		*(nums + i) = (int*)malloc(sizeof(int) * LINE);
		for (int j = 0; j < LINE; j++)
			nums[i][j] = 0;
	}
	return nums;
}

bool HaveEmpty(int** nums) {                                                                //检查是否有空余位置：true 或 false。
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (!nums[i][j]) return true;
	return false;
}

void random_2or4(int** nums) {                                                              //空余位子（元素为0）的位置随机出现一个2或者4
	if (!HaveEmpty(nums)) return;                                                           //如果没有空余位置，而且用户还可以移动，跳过此函数
	srand((unsigned)time(NULL));                                                            //位置随机
	int temp = rand(), count = 0, randnum = 0;
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (!nums[i][j]) count++;

	randnum = (temp % 10) < 7 ? 2 : 4;                                                      //出现2的概率为0.7
	temp %= count;
	count = 0;
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++) {
			if (nums[i][j]) continue;
			if (!nums[i][j] && count != temp) count++;
			else if (count == temp) {
				nums[i][j] = randnum;
				return;
			}
		}
}

void refresh(int** nums, PIMAGE* p) {					                                    //刷新游戏界面4*4方块视图
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			putimage(pos_X + EDGE * i, pos_Y + EDGE * j, p[index(nums[i][j])]);
}

COLORREF score_color() {
	int n = 0, s = score;
	while (s > 10) {
		s /= 10; n++;
	}
	if (n < 7) return rainbow_color[n];
	else return rainbow_color[7];
}

bool can_continue(int** nums) {                                                             //游戏是否可以继续：true or false
	for (int i = 0; i < LINE; i++) {                                                        //行方向的检查
		for (int j = 0; j < LINE - 1; j++) {
			if (!nums[i][j]) return true;
			if (nums[i][j] == nums[i][j + 1]) return true;
		}
	}
	for (int j = 0; j < LINE; j++)                                                          //列方向的检查
		for (int i = 0; i < LINE - 1; i++)
			if (nums[i][j] == nums[i + 1][j]) return true;
	return false;
}

bool iswin(int** nums, int target) {                                                        //判断是否达到某个数值
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (nums[i][j] == target)
				return true;
	return false;
}

int** deep_copy(int** nums) {                                                               //深拷贝状态矩阵
	int** nums0 = (int**)malloc(sizeof(int*) * LINE);
	for (int i = 0; i < LINE; i++) {
		nums0[i] = (int*)malloc(sizeof(int) * LINE);
		for (int j = 0; j < LINE; j++)
			nums0[i][j] = nums[i][j];
	}
	return nums0;
}

bool is_same(int** nums1, int** nums2) {                                                    //判断两个状态矩阵是否相等
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (nums1[i][j] != nums2[i][j]) return false;
	return true;
}

bool is_zeros(int** nums) {
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (nums[i][j]) return false;
	return true;
}

void adjust_nums_assist(int** nums, int*** nums_assist, int nums_assist_size, int number, bool in) {
	if (in) {
		for (int i = nums_assist_size - 1; i - number > -1; i--)
			nums_assist[i] = deep_copy(nums_assist[i - number]);
		nums_assist[0] = deep_copy(nums);
	}
	else {
		for (int i = 0; i + number < nums_assist_size; i++)
			nums_assist[i] = deep_copy(nums_assist[i + number]);
	}
}

/*四个方向上的滑动操作*/
void up(int** nums) {
	for (int j = 0; j < LINE; j++) {
		myStack S; S.top = 0;
		S.stack[S.top++] = nums[0][j];
		for (int i = 1; i < LINE; i++) {
			if (S.stack[S.top - 1] == nums[i][j]) {
				S.stack[S.top - 1] *= 2;
				score += S.stack[S.top - 1];
				ismove = true;
			}
			else if (!S.stack[S.top - 1]) {
				S.stack[S.top - 1] = nums[i][j];
				ismove = true;
			}
			else if (nums[i][j])
				S.stack[S.top++] = nums[i][j];
			else;
		}
		int count = 0;
		for (int i = 0; i < LINE; i++) {
			if (count < S.top) nums[i][j] = S.stack[count++];
			else nums[i][j] = 0;
		}
	}
}
void down(int** nums) {
	for (int j = 0; j < LINE; j++) {
		myStack S; S.top = 0;
		S.stack[S.top++] = nums[3][j];
		for (int i = 2; i >= 0; i--) {
			if (S.stack[S.top - 1] == nums[i][j]) {
				S.stack[S.top - 1] *= 2;
				score += S.stack[S.top - 1];
				ismove = true;
			}
			else if (!S.stack[S.top - 1]) {
				S.stack[S.top - 1] = nums[i][j];
				ismove = true;
			}
			else if (nums[i][j])
				S.stack[S.top++] = nums[i][j];
			else;
		}
		int count = 0;
		for (int i = 3; i >= 0; i--) {
			if (count < S.top) nums[i][j] = S.stack[count++];
			else nums[i][j] = 0;
		}
	}
}
void left(int** nums) {
	for (int i = 0; i < LINE; i++) {
		myStack S; S.top = 0;
		S.stack[S.top++] = nums[i][0];
		for (int j = 1; j < LINE; j++) {
			if (S.stack[S.top - 1] == nums[i][j]) {
				S.stack[S.top - 1] *= 2;
				score += S.stack[S.top - 1];
				ismove = true;
			}
			else if (!S.stack[S.top - 1]) {
				S.stack[S.top - 1] = nums[i][j];
				ismove = true;
			}
			else if (nums[i][j])
				S.stack[S.top++] = nums[i][j];
			else;
		}
		int count = 0;
		for (int j = 0; j < LINE; j++) {
			if (count < S.top) nums[i][j] = S.stack[count++];
			else nums[i][j] = 0;
		}
	}
}
void right(int** nums) {
	for (int i = 0; i < LINE; i++) {
		myStack S; S.top = 0;
		S.stack[S.top++] = nums[i][3];
		for (int j = 2; j >= 0; j--) {
			if (S.stack[S.top - 1] == nums[i][j]) {
				S.stack[S.top - 1] *= 2;
				score += S.stack[S.top - 1];
				ismove = true;
			}
			else if (!S.stack[S.top - 1]) {
				S.stack[S.top - 1] = nums[i][j];
				ismove = true;
			}
			else if (nums[i][j]) 
				S.stack[S.top++] = nums[i][j];
			else;
		}
		int count = 0;
		for (int j = 3; j >= 0; j--) {
			if (count < S.top) nums[i][j] = S.stack[count++];
			else nums[i][j] = 0;
		}
	}
}

/*一系列打印函数*/

void printTitle() {                                                                         //输出标题
	setcolor(EGERGB(62, 224, 208));
	setfont(TITLESIZE, 0, "华文隶书");
	setbkmode(TRANSPARENT);
	xyprintf(WIDTH / 2 - TITLESIZE * 1, 30, "Game2048");
}

void clear_reset(PIMAGE background) {
	cleardevice();
	putimage(0, 0, background);
	printTitle();
}

void printRules(PIMAGE rules) {
	cleardevice();
	putimage(0, 0, rules);
}

void printWin(PIMAGE win, PIMAGE bkg) {
	clear_reset(bkg);
	int lose_edge = 450;
	int posx = (WIDTH - lose_edge) / 2 + 30, posy = TITLESIZE + 50;                         //图片位置：左右居中略微偏右，标题下方20像素处
	putimage(posx, posy, win);
	setfont(30, 0, "华文隶书");
	setcolor(RED);
	xyprintf(posx + 150, posy + 0.8 * lose_edge - 4, "汝骨骼清奇，必为奇才");

	setcolor(EGERGB(0, 178, 238));
	setfont(20, 0, "楷体");
	xyprintf(posx + 50, posy + 0.9 * lose_edge - 4, "ESC 退出，或者 ENTER键 再攀高峰！");
}

void printFinalwin(PIMAGE win2_final) {
	cleardevice();
	putimage(0, 0, win2_final);
}

void printLose(PIMAGE lose, PIMAGE bkg) {
	clear_reset(bkg);
	int lose_edge = 450;
	int posx = (WIDTH - lose_edge) / 2 + 30, posy = TITLESIZE + 50;                         //图片位置：左右居中略微偏右，标题下方20像素处
	putimage(posx, posy, lose);
	setfont(20, 0, "幼圆");
	setcolor(EGERGB(155, 48, 255));
	xyprintf(posx + 10, posy + 0.8 * lose_edge, "在上一轮中你的得分是：");

	char ch[10]; num_to_string_10(score, ch);
	setfont(30, 0, "幼圆");
	setcolor(score_color());
	xyprintf(posx + 250, posy + 0.8 * lose_edge - 4, ch);

	setcolor(EGERGB(0, 178, 238));
	setfont(20, 0, "楷体");
	xyprintf(posx + 10, posy + 0.9 * lose_edge - 4, "ESC键退出，或者 ENTER键 再来一次！");
}

void printScore() {                                                                         //输出分数
	score0 = score > score0 ? score : score0;
	char ch[10];
	setcolor(score_color());
	setfont(FONTSIZE, 0, "幼圆");
	setbkmode(TRANSPARENT);
	num_to_string_10(score, ch);
	xyprintf(pos_X, pos_Y - FONTSIZE - 5, "分数：");
	xyprintf(pos_X + 0.8 * EDGE, pos_Y - FONTSIZE - 5, ch);

	setcolor(EGERGB(205, 112, 84));
	setfont(FONTSIZE - 5, 0, "幼圆");
	num_to_string_10(score0, ch);
	xyprintf(pos_X + EDGE * 2, pos_Y - FONTSIZE, "历史最高：");
	xyprintf(pos_X + 3 * EDGE, pos_Y - FONTSIZE, ch);
}

int main() {
	PIMAGE p[15];                                                                           //空白图片，108*108 留待存储0到16384一共十一张图
	PIMAGE win;                                                                             //空白图片，450*450
	PIMAGE lose;                                                                            //空白图片，450*450
	PIMAGE bkg;                                                                             //空白图片，650*680 留待存储背景
	PIMAGE rules;                                                                           //空白图片，650*680 留待存储规则
	PIMAGE final_win;			                                                            //空白图片，650*680 留待存储终结胜利
	bool isquit = false;                                                                    //是否退出程序：0表示不退出，1表示退出

	/*设置窗口*/
	initgraph(WIDTH, HEIGHT);	                                                            //设置整个窗口为 WIDTH * HEIGHT 像素
	cleardevice();
	setcaption("Whywait团队：GAME2048");

	/*加载图片*/
	getimage(p[0]      = newimage(), "Images\\0.jpg");
	getimage(p[1]      = newimage(), "Images\\2.jpg");
	getimage(p[2]      = newimage(), "Images\\4.jpg");
	getimage(p[3]      = newimage(), "Images\\8.jpg");
	getimage(p[4]      = newimage(), "Images\\16.jpg");
	getimage(p[5]      = newimage(), "Images\\32.jpg");
	getimage(p[6]      = newimage(), "Images\\64.jpg");
	getimage(p[7]      = newimage(), "Images\\128.jpg");
	getimage(p[8]      = newimage(), "Images\\256.jpg");
	getimage(p[9]      = newimage(), "Images\\512.jpg");
	getimage(p[10]     = newimage(), "Images\\1024.jpg");
	getimage(p[11]     = newimage(), "Images\\2048.jpg");
	getimage(p[12]     = newimage(), "Images\\4096.jpg");
	getimage(p[13]     = newimage(), "Images\\8192.jpg");
	getimage(p[14]     = newimage(), "Images\\16384.jpg");
	getimage(win       = newimage(), "Images\\win.jpg");
	getimage(lose      = newimage(), "Images\\gameover.jpg");
	getimage(bkg       = newimage(), "Images\\小清新.jpg");
	getimage(rules     = newimage(), "Images\\rules.jpg");
	getimage(final_win = newimage(), "Images\\胜利.jpg");

	while (!isquit) {                                                                       //除非点击ESC，否则可以反复开局
		score = 0;                                                                          //每一局开始初始化分数
		ismove = true;                                                                      //矩阵状态是否改变。true：改变；false：没有改变。以决定是否要调用random函数
		bool isOK = true;                                                                   //是否可以继续运行(算法判定）。true：没死， false：死了。
		bool have2048 = false;                                                              //矩阵中是否存在2048。true：有；false：没有。
		int** nums = init_nums();                                                           //当前状态矩阵
		int nums_assist_size = REDO_TIMES;                                                  //撤销步数上界
		int*** nums_assist = (int***)calloc(nums_assist_size, sizeof(int**));
		for (int i = 0; i < nums_assist_size; i++)
			nums_assist[i] = init_nums();

		printRules(rules);                                                                  //告知玩家规则

		int ch = 0;
		while (ch != ESC && ch != ENTER)
			ch = getch();
		if (ch == ESC) break;

		while (isOK) {
			clear_reset(bkg);                                                               //刷新并重新打印背景
			printScore();                                                                   //打印分数以及历史最高得分
			if (ismove) {
				random_2or4(nums);
				if (!is_same(nums, nums_assist[0]))
					adjust_nums_assist(nums, nums_assist, nums_assist_size, 1, true);
			}

			refresh(nums, p);

			if (iswin(nums, 16384)) {                                                       //如果得到了最高数值的方块
				printFinalwin(final_win);
				getch();
				return 0;                                                                   //任意键结束游戏
			}

			if (have2048 == false && iswin(nums, 2048)) {
				getch();
				have2048 = true;
				printWin(win, bkg);

				int ch = 0;
				while (ch != ESC && ch != ENTER) {
					ch = getch();
					if (ch == ESC) isquit = true;
				}
				if (ch == ENTER) continue;
			}

			if (!can_continue(nums)) {                                                      //判断：如果不能继续游戏（就是死了）
				getch();
				isOK = false;
				break;                                                                      //跳出循环
			}

			if (isquit) break;
			ismove = false;                                                                 //初始化变量
			int ch = getch();
			int temp_i = 0;
			switch (ch) {
			case ESC:
				isquit = 1;                                                                 //输出ESC键退出游戏
				break;
			case REDO:
				for (temp_i = 0; temp_i < nums_assist_size; temp_i++) {
					bool condition1 = is_zeros(nums_assist[temp_i]);
					bool condition2 = temp_i > 0 && is_same(nums_assist[temp_i], nums_assist[temp_i - 1]);
					if (condition1 || condition2) {                                         //如果为零矩阵或者与前一个矩阵相同（此时该索引以及后面的矩阵都是相同的，故跳出循环减少时间）
						temp_i--; break;
					}
					if (!is_same(nums, nums_assist[temp_i])) {                              //找到与当前状态不同的前一个状态矩阵
						nums = deep_copy(nums_assist[temp_i]);
						break;
					}
				}
				adjust_nums_assist(nums, nums_assist, nums_assist_size, temp_i, false);		//重新调整协助状态矩阵数组
				break;
			case LEFT:
				left(nums);                                                                 //两次调用方向函数是为了使消除完全彻底，下同
				left(nums);
				break;
			case RIGHT:
				right(nums);
				right(nums);
				break;
			case UP:
				up(nums);
				up(nums);
				break;
			case DOWN:
				down(nums);
				down(nums);
			}
			if (isquit) break;			                                                    //退出循环
		}
		if (!isOK) {
			printLose(lose, bkg);
			int ch = 0;
			while (ch != ENTER && ch != ESC) {                                              //直到输入ESC或者ESC
				ch = getch();
				if (ch == ESC) isquit = true;
			}
		}
		if (isquit) break;
	}
	closegraph();
	return 0;
}