/*
 * ��Ŀ���ƣ���Ϸ2048
 * �������ڣ�2020��5��6��
 * �����Ŷӣ�Whywait�Ŷ�
 * ��Ŀ˵����
 *	1.����˵�����ƶ����̷������������Ϸ��ESC�˳���Ϸ��ENTER������Ϸ��backspace������һ����
 *	2.��Ϸ˵��������2048��ΪӮ����Ϸ��Ӯ����Ϸ֮����Լ�����Ϸ��
 *	3.��Ϸ��ɫ��
 *		a. ���Ӵ̼���ÿ�λ�������������·���ϵ����п��������ķ���ȫ��������
 *		b. 2048�Լ��Ժ�ķ��鶼������ƣ��ɽ�����ϲ��
 *		c. ������ֵΪ16384�ķ��飬��ΪӮ������ʤ������Ϸ������
 *		d. ���ŵ÷ֵ����ӣ������������ɫ����Ӧ�ı䡣��Ϊ �ҳ�Ȼ��������� �˸��ȼ���
 *		e. �������2��4������2�ĸ�������Ϊ0.7��
 *		f. ������������200���ſ��ֽţ�������ˣ��
 */

#include<graphics.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include"conio.h"

/*������Ӧ�ļ�ֵ*/
#define LEFT		294                                                                //���
#define UP			293                                                                //�ϼ�
#define RIGHT		296                                                                //�Ҽ�
#define DOWN		295                                                                //�¼�
#define ESC			27                                                                 //ESC �˳���
#define ENTER		13                                                                 //ENTER ������
#define REDO		8		                                                           //backspace ������
/*���ڵ�����*/
#define TITLESIZE	80                                                                 //�����С
#define WIDTH		650                                                                // ���ڵĿ�
#define HEIGHT		680                                                                //���ڵĸ�
/*��Ϸ�û��ٿؽ��������*/
#define LINE		4                                                                  //4*4�ķ���
#define pos_X		(WIDTH-EDGE*LINE)/2+40                                             //���Ͻǵĺ�����:���Ҿ���
#define pos_Y		160                                                                //���Ͻǵ�������
#define EDGE		108                                                                //ÿ������ĳ���
/*��������λ�õ�����*/
#define FONTSIZE		28                                                             //������ʾ�ַ���С
#define POS_B1_WIDTH	pos_X	                                                       //�����������ϽǺ�����
#define POS_B1_HEIGHT	pos_Y-FONTSIZE-2                                               //�����������Ͻ�������
/*��Ϸ����*/
#define REDO_TIMES		200                                                            //��Ϸ�ɷ�����һ���Ĵ����Ͻ磨������ڣ�

int score = 0, score0 = 0;			                                                   //����������
int ismove = false;
COLORREF rainbow_color[8] = {                                                          //�ʺ�ɫ���������ȼ���
	EGERGB(205, 201, 201),                                                             //��
	RED,                                                                               //��
	EGERGB(255, 200, 0),                                                               //��
	EGERGB(255, 215, 0),                                                               //��
	GREEN,                                                                             //��
	EGERGB(64, 224, 208),                                                              //��
	BLUE,                                                                              //��
	EGERGB(160, 32, 240)                                                               //��
};
typedef struct myStack {                                                               //����ջ�����ݽṹ���ڲ�ͬ�ķ���Ļ���������ʹ��
	int stack[LINE];
	int top;
} myStack;

int index(int n) {                                                                     //Ϊ��ͬ����ֵn�ҵ����Ӧ��ͼƬ�����е�����
	if (!n) return 0;
	int returnNum = 0;
	while (n != 1) {
		n /= 2;
		returnNum++;
	}
	return returnNum;
}

void num_to_string_10(int num, char* string) {                                         //������ת��Ϊ�ַ�������Ӧxyprintf()
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

int** init_nums() {                                                                    //��ʼ��һ������
	int** nums = (int**)malloc(sizeof(int*) * LINE);
	for (int i = 0; i < LINE; i++) {
		*(nums + i) = (int*)malloc(sizeof(int) * LINE);
		for (int j = 0; j < LINE; j++)
			nums[i][j] = 0;
	}
	return nums;
}

bool HaveEmpty(int** nums) {                                                           //����Ƿ��п���λ�ã�true �� false��
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (!nums[i][j]) return true;
	return false;
}

void random_2or4(int** nums) {                                                         //����λ�ӣ�Ԫ��Ϊ0����λ���������һ��2����4
	if (!HaveEmpty(nums)) return;                                                      //���û�п���λ�ã������û��������ƶ��������˺���
	srand((unsigned)time(NULL));                                                       //λ�����
	int temp = rand(), count = 0, randnum = 0;
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (!nums[i][j]) count++;

	randnum = (temp % 10) < 7 ? 2 : 4;                                                 //����2�ĸ���Ϊ0.7
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

void refresh(int** nums, PIMAGE* p) {					                               //ˢ����Ϸ����4*4������ͼ
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

bool can_continue(int** nums) {                                                        //��Ϸ�Ƿ���Լ�����true or false
	for (int i = 0; i < LINE; i++) {                                                   //�з���ļ��
		for (int j = 0; j < LINE - 1; j++) {
			if (!nums[i][j]) return true;
			if (nums[i][j] == nums[i][j + 1]) return true;
		}
	}
	for (int j = 0; j < LINE; j++)                                                     //�з���ļ��
		for (int i = 0; i < LINE - 1; i++)
			if (nums[i][j] == nums[i + 1][j]) return true;
	return false;
}

bool iswin(int** nums, int target) {                                                   //�ж��Ƿ�ﵽĳ����ֵ
	for (int i = 0; i < LINE; i++)
		for (int j = 0; j < LINE; j++)
			if (nums[i][j] == target)
				return true;
	return false;
}

int** deep_copy(int** nums) {                                                          //���״̬����
	int** nums0 = (int**)malloc(sizeof(int*) * LINE);
	for (int i = 0; i < LINE; i++) {
		nums0[i] = (int*)malloc(sizeof(int) * LINE);
		for (int j = 0; j < LINE; j++)
			nums0[i][j] = nums[i][j];
	}
	return nums0;
}

bool is_same(int** nums1, int** nums2) {                                               //�ж�����״̬�����Ƿ����
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

/*�ĸ������ϵĻ�������*/
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

/*һϵ�д�ӡ����*/

void printTitle() {                                                                    //�������
	setcolor(EGERGB(62, 224, 208));
	setfont(TITLESIZE, 0, "��������");
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
	int posx = (WIDTH - lose_edge) / 2 + 30, posy = TITLESIZE + 50;                    //ͼƬλ�ã����Ҿ�����΢ƫ�ң������·�20���ش�
	putimage(posx, posy, win);
	setfont(30, 0, "��������");
	setcolor(RED);
	xyprintf(posx + 150, posy + 0.8 * lose_edge - 4, "��������棬��Ϊ���");

	setcolor(EGERGB(0, 178, 238));
	setfont(20, 0, "����");
	xyprintf(posx + 50, posy + 0.9 * lose_edge - 4, "ESC �˳������� ENTER�� ���ʸ߷壡");
}

void printFinalwin(PIMAGE win2_final) {
	cleardevice();
	putimage(0, 0, win2_final);
}

void printLose(PIMAGE lose, PIMAGE bkg) {
	clear_reset(bkg);
	int lose_edge = 450;
	int posx = (WIDTH - lose_edge) / 2 + 30, posy = TITLESIZE + 50;                    //ͼƬλ�ã����Ҿ�����΢ƫ�ң������·�20���ش�
	putimage(posx, posy, lose);
	setfont(20, 0, "��Բ");
	setcolor(EGERGB(155, 48, 255));
	xyprintf(posx + 10, posy + 0.8 * lose_edge, "����һ������ĵ÷��ǣ�");

	char ch[10]; num_to_string_10(score, ch);
	setfont(30, 0, "��Բ");
	setcolor(score_color());
	xyprintf(posx + 250, posy + 0.8 * lose_edge - 4, ch);

	setcolor(EGERGB(0, 178, 238));
	setfont(20, 0, "����");
	xyprintf(posx + 10, posy + 0.9 * lose_edge - 4, "ESC���˳������� ENTER�� ����һ�Σ�");
}

void printScore() {                                                                    //�������
	score0 = score > score0 ? score : score0;
	char ch[10];
	setcolor(score_color());
	setfont(FONTSIZE, 0, "��Բ");
	setbkmode(TRANSPARENT);
	num_to_string_10(score, ch);
	xyprintf(pos_X, pos_Y - FONTSIZE - 5, "������");
	xyprintf(pos_X + 0.8 * EDGE, pos_Y - FONTSIZE - 5, ch);

	setcolor(EGERGB(205, 112, 84));
	setfont(FONTSIZE - 5, 0, "��Բ");
	num_to_string_10(score0, ch);
	xyprintf(pos_X + EDGE * 2, pos_Y - FONTSIZE, "��ʷ��ߣ�");
	xyprintf(pos_X + 3 * EDGE, pos_Y - FONTSIZE, ch);
}

int main() {
	PIMAGE p[15];                                                                      //�հ�ͼƬ��108*108 �����洢0��16384һ��ʮһ��ͼ
	PIMAGE win;                                                                        //�հ�ͼƬ��450*450
	PIMAGE lose;                                                                       //�հ�ͼƬ��450*450
	PIMAGE bkg;                                                                        //�հ�ͼƬ��650*680 �����洢����
	PIMAGE rules;                                                                      //�հ�ͼƬ��650*680 �����洢����
	PIMAGE final_win;			                                                       //�հ�ͼƬ��650*680 �����洢�ս�ʤ��
	bool isquit = false;                                                               //�Ƿ��˳�����0��ʾ���˳���1��ʾ�˳�

	/*���ô���*/
	initgraph(WIDTH, HEIGHT);	                                                       //������������Ϊ WIDTH * HEIGHT ����
	cleardevice();
	setcaption("Whywait�Ŷӣ�GAME2048");

	/*����ͼƬ*/
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
	getimage(bkg       = newimage(), "Images\\С����.jpg");
	getimage(rules     = newimage(), "Images\\rules.jpg");
	getimage(final_win = newimage(), "Images\\ʤ��.jpg");

	while (!isquit) {                                                                  //���ǵ��ESC��������Է�������
		score = 0;                                                                     //ÿһ�ֿ�ʼ��ʼ������
		ismove = true;                                                                 //����״̬�Ƿ�ı䡣true���ı䣻false��û�иı䡣�Ծ����Ƿ�Ҫ����random����
		bool isOK = true;                                                              //�Ƿ���Լ�������(�㷨�ж�����true��û���� false�����ˡ�
		bool have2048 = false;                                                         //�������Ƿ����2048��true���У�false��û�С�
		int** nums = init_nums();                                                      //��ǰ״̬����
		int nums_assist_size = REDO_TIMES;                                             //���������Ͻ�
		int*** nums_assist = (int***)calloc(nums_assist_size, sizeof(int**));
		for (int i = 0; i < nums_assist_size; i++)
			nums_assist[i] = init_nums();

		printRules(rules);                                                             //��֪��ҹ���

		int ch = 0;
		while (ch != ESC && ch != ENTER)
			ch = getch();
		if (ch == ESC) break;

		while (isOK) {
			clear_reset(bkg);                                                          //ˢ�²����´�ӡ����
			printScore();                                                              //��ӡ�����Լ���ʷ��ߵ÷�
			if (ismove) {
				random_2or4(nums);
				if (!is_same(nums, nums_assist[0]))
					adjust_nums_assist(nums, nums_assist, nums_assist_size, 1, true);
			}

			refresh(nums, p);

			if (!can_continue(nums)) {                                                 //�жϣ�������ܼ�����Ϸ���������ˣ�
				getch();
				isOK = false;
				break;                                                                 //����ѭ��
			}

			if (iswin(nums, 16384)) {                                                  //����õ��������ֵ�ķ���
				printFinalwin(final_win);
				getch();
				return 0;                                                              //�����������Ϸ
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

			if (isquit) break;
			ismove = false;                                                            //��ʼ������
			int ch = getch();
			int temp_i = 0;
			switch (ch) {
			case ESC:
				isquit = 1;                                                            //���ESC���˳���Ϸ
				break;
			case REDO:
				for (temp_i = 0; temp_i < nums_assist_size; temp_i++) {
					bool condition1 = is_zeros(nums_assist[temp_i]);
					bool condition2 = temp_i > 0 && is_same(nums_assist[temp_i], nums_assist[temp_i - 1]);
					if (condition1 || condition2) {                                    //���Ϊ����������ǰһ��������ͬ����ʱ�������Լ�����ľ�������ͬ�ģ�������ѭ������ʱ�䣩
						temp_i--; break;
					}
					if (!is_same(nums, nums_assist[temp_i])) {                         //�ҵ��뵱ǰ״̬��ͬ��ǰһ��״̬����
						nums = deep_copy(nums_assist[temp_i]);
						break;
					}
				}
				adjust_nums_assist(nums, nums_assist, nums_assist_size, temp_i, false);//���µ���Э��״̬��������
				break;
			case LEFT:
				left(nums);                                                            //���ε��÷�������Ϊ��ʹ������ȫ���ף���ͬ
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
			if (isquit) break;			                                               //�˳�ѭ��
		}
		if (!isOK) {
			printLose(lose, bkg);
			int ch = 0;
			while (ch != ENTER && ch != ESC) {                                         //ֱ������ESC����ESC
				ch = getch();
				if (ch == ESC) isquit = true;
			}
		}
		if (isquit) break;
	}
	closegraph();
	return 0;
}