//实现游戏界面,两项控制,加速功能,背景图片
#include <graphics.h>
#include <conio.h>
#include <deque>
#include <iostream>
#include <cmath>
#include <ctime>
#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")  // 链接Windows多媒体库
using namespace std;
const TCHAR* BG_MUSIC = _T("G:/CPP大作业/贪吃蛇游戏3/音乐/(What a nice surprise）贪吃蛇配乐游戏.mp3");
const TCHAR* EAT_SOUND = _T("G:/CPP大作业/贪吃蛇游戏3/音乐/游戏金币收集_耳聆网_[声音ID：22093].wav");
const TCHAR* GAME_OVER_SOUND = _T("G:/CPP大作业/贪吃蛇游戏3/音乐/游戏结束_Freesound.aiff");

// 播放音乐函数，用于背景音乐
void PlayMusic(const TCHAR* filename, bool loop = true)
{
    TCHAR cmd[256];
    if (loop)
    {
        // 先关闭可能存在的背景音乐
        mciSendString(_T("close bg_music"), NULL, 0, NULL);

        _stprintf_s(cmd, _T("open \"%s\" alias bg_music"), filename);
        mciSendString(cmd, NULL, 0, NULL);
        mciSendString(_T("play bg_music repeat"), NULL, 0, NULL);
    }
    else
    {
        mciSendString(_T("close temp_sound"), NULL, 0, NULL);

        _stprintf_s(cmd, _T("open \"%s\" alias temp_sound"), filename);
        mciSendString(cmd, NULL, 0, NULL);
        mciSendString(_T("play temp_sound"), NULL, 0, NULL);
    }
}

// 播放吃食物音效
void PlayEatingSound()
{
    TCHAR cmd[256];

    // 先关闭可能存在的吃食物音效
    mciSendString(_T("close eat_sound"), NULL, 0, NULL);

    _stprintf_s(cmd, _T("open \"%s\" alias eat_sound"), EAT_SOUND);
    mciSendString(cmd, NULL, 0, NULL);
    mciSendString(_T("play eat_sound"), NULL, 0, NULL);
}

// 播放游戏结束音效
void PlayGameOverSound()
{
    TCHAR cmd[256];

    // 先关闭可能存在的游戏结束音效
    mciSendString(_T("close over_sound"), NULL, 0, NULL);

    _stprintf_s(cmd, _T("open \"%s\" alias over_sound"), GAME_OVER_SOUND);
    mciSendString(cmd, NULL, 0, NULL);
    mciSendString(_T("play over_sound"), NULL, 0, NULL);
}

// 停止背景音乐
void StopMusic()
{
    mciSendString(_T("stop bg_music"), NULL, 0, NULL);
    mciSendString(_T("close bg_music"), NULL, 0, NULL);
}

// 停止游戏结束音效
void StopGameOverSound()
{
    mciSendString(_T("stop over_sound"), NULL, 0, NULL);
    mciSendString(_T("close over_sound"), NULL, 0, NULL);
}

#define WIDTH 800
#define HEIGHT 600
#define SIZE 20
#define DELAY 160      // 移动速度调整
#define BASE_SPEED 0.8f // 基础移动速度系数
#define MAX_SPEED 1.4f  // 最大移动速度系数
#define SPEED_INCREMENT 0.1f // 每次吃到蓝色食物增加的速度
#define FOOD_COUNT 5    // 同时存在的食物数量
#define BTN_W 200
#define BTN_H 50
#define START_Y 200
#define SPACING 80
// 背景图片结构
struct GameBackgrounds {
    IMAGE menuBg;    // 菜单背景
    IMAGE gameBg;    // 游戏背景
    IMAGE helpBg;    // 帮助背景
} bg;

// 加载所有背景图片
void loadBackgrounds() {
    loadimage(&bg.menuBg, _T("G:/CPP大作业/贪吃蛇游戏3/图片/贪吃蛇菜单背景.png"), WIDTH, HEIGHT);
    loadimage(&bg.gameBg, _T("G:/CPP大作业/贪吃蛇游戏3/图片/游戏背景.png"), WIDTH, HEIGHT);
    loadimage(&bg.helpBg, _T("G:/CPP大作业/贪吃蛇游戏3/图片/游戏帮助背景.jpg"), WIDTH, HEIGHT);
}

struct Snake {
    float x;
    float y;
    float dx;
    float dy;
};

struct Food {
    int x;
    int y;
    bool eaten = false;
    COLORREF color; // 食物颜色
};

// 函数声明
void showMenu();
void isOver(const deque<Snake>& snake);
void startGame();
void showHelp();
void initGame();
// 计算两点间距离
float distance(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// 完全重绘游戏界面，消除所有痕迹
void redrawScene(const deque<Snake>& snake, const Food foods[], int score, float currentSpeed) {
    cleardevice();
    putimage(0, 0, &bg.gameBg);  // 先绘制背景

    // 绘制蛇身
    for (size_t i = 0; i < snake.size(); ++i) {
        Snake body = snake[i];

        // 计算渐变颜色 - 从深绿到浅绿
        int green = 50 + static_cast<int>(205.0f * i / snake.size());

        if (i == 0) {
            // 蛇头 - 深绿色
            setfillcolor(RGB(0, 100, 0));
        }
        else {
            // 蛇身 - 渐变色
            setfillcolor(RGB(0, green, 0));
        }

        int centerX = static_cast<int>(body.x * SIZE + SIZE / 2);
        int centerY = static_cast<int>(body.y * SIZE + SIZE / 2);
        fillcircle(centerX, centerY, SIZE / 2 - 1);
    }

    // 绘制所有食物
    for (int i = 0; i < FOOD_COUNT; i++) {
        if (!foods[i].eaten) {
            setfillcolor(foods[i].color);
            int centerX = foods[i].x * SIZE + SIZE / 2;
            int centerY = foods[i].y * SIZE + SIZE / 2;
            fillcircle(centerX, centerY, SIZE / 2 - 1);
        }
    }

    // 显示分数和速度
    settextcolor(YELLOW);
    settextstyle(25, 0, _T("Consolas"));
    TCHAR scoreStr[50];
    _stprintf_s(scoreStr, _T("Score: %d  Speed: %.1f"), score, currentSpeed);
    outtextxy(10, 10, scoreStr);
}

void isOver(const deque<Snake>& snake) {
    // 边界碰撞检测
    if (snake.front().x < 0 || snake.front().x >= WIDTH / SIZE ||
        snake.front().y < 0 || snake.front().y >= HEIGHT / SIZE)
    {

        // 保存当前窗口状态
        HWND hwnd = GetHWnd();

        // 先停止背景音乐
        StopMusic();

        // 显示结束信息
        cleardevice();
        settextcolor(RED);
        settextstyle(100, 0, _T("Consolas"));
        outtextxy(WIDTH / 2 - 300, HEIGHT / 2 - 50, _T("GAME OVER!!!"));

        // 播放游戏结束音效
        PlayGameOverSound();

        FlushBatchDraw();

        // 等待3秒
        Sleep(3000);

        // 停止游戏结束音效
        StopGameOverSound();

        // 重启游戏流程
        EndBatchDraw();
        closegraph();  // 关闭当前窗口

        // 重新初始化并显示菜单
        initgraph(WIDTH, HEIGHT);  // 创建新窗口

        // 重新开始播放背景音乐
        PlayMusic(BG_MUSIC);

        showMenu();
    }

    // 自身碰撞检测
    if (snake.size() > 4) {
        for (auto it = snake.begin() + 4; it != snake.end(); ++it) {
            if (abs(snake.front().x - it->x) < 0.9f && abs(snake.front().y - it->y) < 0.9f) {
                // 保存当前窗口状态
                HWND hwnd = GetHWnd();

                // 先停止背景音乐
                StopMusic();

                // 显示结束信息
                cleardevice();
                settextcolor(RED);
                settextstyle(100, 0, _T("Consolas"));
                outtextxy(WIDTH / 2 - 300, HEIGHT / 2 - 50, _T("GAME OVER!!!"));

                // 播放游戏结束音效
                PlayGameOverSound();

                FlushBatchDraw();

                // 等待3秒
                Sleep(3000);

                // 停止游戏结束音效
                StopGameOverSound();

                // 重启游戏流程
                EndBatchDraw();
                closegraph();  // 关闭当前窗口

                // 重新初始化并显示菜单
                initgraph(WIDTH, HEIGHT);  // 创建新窗口

                // 重新开始播放背景音乐
                PlayMusic(BG_MUSIC);

                showMenu();
            }
        }
    }
}

void moveSnake(deque<Snake>& snake, bool grow = false, float speedFactor = BASE_SPEED) {
    Snake newhead = snake.front();
    newhead.x += newhead.dx * speedFactor;
    newhead.y += newhead.dy * speedFactor;
    snake.push_front(newhead);

    if (!grow) {
        snake.pop_back();
    }

    isOver(snake);
}

void changeDirection(deque<Snake>& snake) {
    ExMessage msg;
    while (peekmessage(&msg, EX_KEY | EX_MOUSE)) {
        if (msg.message == WM_KEYDOWN) {
            switch (msg.vkcode) {
            case 'W': case 'w': case VK_UP:
                if (abs(snake.front().dy - 1) > 0.1f) {
                    snake.front().dx = 0;
                    snake.front().dy = -1;
                }
                break;
            case 'S': case 's': case VK_DOWN:
                if (abs(snake.front().dy + 1) > 0.1f) {
                    snake.front().dx = 0;
                    snake.front().dy = 1;
                }
                break;
            case 'A': case 'a': case VK_LEFT:
                if (abs(snake.front().dx - 1) > 0.1f) {
                    snake.front().dx = -1;
                    snake.front().dy = 0;
                }
                break;
            case 'D': case 'd': case VK_RIGHT:
                if (abs(snake.front().dx + 1) > 0.1f) {
                    snake.front().dx = 1;
                    snake.front().dy = 0;
                }
                break;
            }
        }
        else if (msg.message == WM_LBUTTONDOWN || (msg.message == WM_MOUSEMOVE && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))) {
            float mouseX = msg.x;
            float mouseY = msg.y;
            float headX = snake.front().x * SIZE + SIZE / 2;
            float headY = snake.front().y * SIZE + SIZE / 2;

            float dx = mouseX - headX;
            float dy = mouseY - headY;
            float len = sqrt(dx * dx + dy * dy);

            if (len > SIZE) {
                float newDx = dx / len;
                float newDy = dy / len;

                snake.front().dx = snake.front().dx * 0.7f + newDx * 0.3f;
                snake.front().dy = snake.front().dy * 0.7f + newDy * 0.3f;

                float newLen = sqrt(snake.front().dx * snake.front().dx + snake.front().dy * snake.front().dy);
                if (newLen > 0) {
                    snake.front().dx /= newLen;
                    snake.front().dy /= newLen;
                }
            }
        }
    }
}

void generateFood(Food foods[], const deque<Snake>& snake) 
{
    static unsigned int seedOffset = 0;
	srand(static_cast<unsigned>(time(0)) + seedOffset++);// 确保每次生成食物时都使用不同的随机种子

    for (int i = 0; i < FOOD_COUNT; i++) {
        if (foods[i].eaten) {
            while (true) {
                foods[i].x = rand() % (WIDTH / SIZE);
                foods[i].y = rand() % (HEIGHT / SIZE);
                bool overlap = false;

                // 检查是否与蛇身重叠
                for (const auto& body : snake) {
                    if (abs(body.x - foods[i].x) < 1.0f && abs(body.y - foods[i].y) < 1.0f) {
                        overlap = true;
                        break;
                    }
                }

                // 检查是否与其他食物重叠
                for (int j = 0; j < FOOD_COUNT; j++) {
                    if (i != j && !foods[j].eaten &&
                        abs(foods[i].x - foods[j].x) < 1.0f &&
                        abs(foods[i].y - foods[j].y) < 1.0f) {
                        overlap = true;
                        break;
                    }
                }

                if (!overlap) break;
            }
            // 随机生成红色或蓝色食物
            foods[i].color = (rand() % 2 == 0) ? RED : BLUE;
            foods[i].eaten = false;
        }
    }
}

bool checkCollision(const deque<Snake>& snake, Food foods[], float& speedFactor, int& score) {
    bool hit = false;
    for (int i = 0; i < FOOD_COUNT; i++) {
        if (!foods[i].eaten && distance(snake.front().x, snake.front().y, foods[i].x, foods[i].y) < 1.0f) {
            foods[i].eaten = true;
            hit = true;
            score++;
            // 播放吃食物音效
            PlayEatingSound();

            if (foods[i].color == BLUE) {
                // 吃到蓝色食物，增加速度系数
                speedFactor = min(speedFactor + SPEED_INCREMENT, MAX_SPEED);
            }
            break;
        }
    }
    return hit;
}

void startGame() {
    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();
    setbkcolor(BLACK);
    cleardevice();

    deque<Snake> snake;
    snake.push_front({ WIDTH / SIZE / 2.0f, HEIGHT / SIZE / 2.0f, 1.0f, 0.0f });

    Food foods[FOOD_COUNT];
    for (int i = 0; i < FOOD_COUNT; i++) {
        foods[i].eaten = true;
    }
    generateFood(foods, snake);

    int score = 0;
    float currentSpeedFactor = BASE_SPEED;

    while (true) 
    {
        changeDirection(snake);
        moveSnake(snake, false, currentSpeedFactor);

        if (checkCollision(snake, foods, currentSpeedFactor, score)) {
            generateFood(foods, snake);
            moveSnake(snake, true, currentSpeedFactor);
        }

        redrawScene(snake, foods, score, currentSpeedFactor);
        FlushBatchDraw();
        Sleep(DELAY);
    }

    EndBatchDraw();
    closegraph();
}

void showHelp() {
    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();  // 开启批量绘制

    // 定义常量
    const int totalLines = 10;
    const int visibleLines = 5;
    const int lineHeight = 40;
    const int textLeftMargin = 120;
    const int buttonWidth = 200;
    const int buttonHeight = 50;

    int currentPage = 0;

    const TCHAR* helpText[] = 
    {
        _T("1. 使用方向键、WASD或者鼠标控制蛇的移动方向"),
        _T("2. 吃到红色食物后，蛇身会变长"),
        _T("3. 吃到蓝色食物后，蛇的速度会增加"),
        _T("4. 撞到墙壁或自己的身体游戏结束"),
        _T("5. 按ESC键可以随时退出游戏"),
        _T("6. 游戏技巧:"),
        _T("- 尽量让蛇在场地中央移动"),
        _T("- 提前规划移动路线，避免陷入死角"),
        _T("- 当蛇身较长时，移动要更加谨慎"),
        _T("祝您游戏愉快!")
    };

    auto drawHelpPage = [&](int page) {
        cleardevice();

        // 绘制背景
        putimage(0, 0, &bg.helpBg);

        // 半透明背景框
        setfillcolor(RGB(0, 0, 0, 180));
        fillrectangle(50, 50, WIDTH - 50, HEIGHT - 100);

        // 标题
        settextcolor(LIGHTGREEN);
        settextstyle(48, 0, _T("Arial"));
        outtextxy(WIDTH / 2 - textwidth(_T("游戏帮助")) / 2, 50, _T("游戏帮助"));

        // 内容文本
        settextcolor(WHITE);
        settextstyle(24, 0, _T("宋体"));
        int y = 150;
        for (int i = 0; i < visibleLines; ++i) {
            int lineIndex = page * visibleLines + i;
            if (lineIndex < totalLines) {
                outtextxy(textLeftMargin, y, helpText[lineIndex]);
                y += lineHeight;
            }
        }

        // 返回按钮
        setfillcolor(GREEN);
        fillroundrect(WIDTH / 2 - buttonWidth / 2, HEIGHT - 100,
            WIDTH / 2 + buttonWidth / 2, HEIGHT - 50, 10, 10);

        settextcolor(WHITE);
        settextstyle(28, 0, _T("Arial"));
        outtextxy(WIDTH / 2 - textwidth(_T("返回主菜单")) / 2,
            HEIGHT - 90, _T("返回主菜单"));

        FlushBatchDraw();  // 刷新绘制
        };

    drawHelpPage(currentPage);

    ExMessage msg;
    while (true) {
        if (peekmessage(&msg, EX_KEY | EX_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                // 检查是否点击了返回按钮
                if (msg.x >= WIDTH / 2 - buttonWidth / 2 &&
                    msg.x <= WIDTH / 2 + buttonWidth / 2 &&
                    msg.y >= HEIGHT - 100 &&
                    msg.y <= HEIGHT - 50) {
                    EndBatchDraw();
                    closegraph();
                    return;
                }
            }
            else if (msg.message == WM_KEYDOWN) {
                switch (msg.vkcode) {
                case VK_UP:  // 上翻页
                    if (currentPage > 0) {
                        --currentPage;
                        drawHelpPage(currentPage);
                    }
                    break;
                case VK_DOWN:  // 下翻页
                    if (currentPage < (totalLines - 1) / visibleLines) {
                        ++currentPage;
                        drawHelpPage(currentPage);
                    }
                    break;
                case VK_ESCAPE:  // ESC键退出
                    EndBatchDraw();
                    closegraph();
                    return;
                }
            }
        }
    }
}
// 修改后的showMenu函数（菜单界面）
void showMenu() {
    initgraph(WIDTH, HEIGHT);
    // 绘制菜单背景
    putimage(0, 0, &bg.menuBg);

    // 菜单按钮样式（半透明效果）
    auto drawButton = [&](int index, const TCHAR* text) {
        int y = START_Y + index * SPACING;

        // 半透明按钮背景
        setfillcolor(RGB(0, 0, 0, 150));
        fillroundrect((WIDTH - BTN_W) / 2, y, (WIDTH + BTN_W) / 2, y + BTN_H, 10, 10);

        // 按钮文字
        settextcolor(WHITE);
        settextstyle(28, 0, _T("Arial"));
        int textWidth = textwidth(text);
        int textHeight = textheight(text);
        outtextxy((WIDTH - textWidth) / 2, y + (BTN_H - textHeight) / 2, text);
        };

    const TCHAR* btnText[] = {
    _T("开始游戏"),
    _T("加载游戏"),
    _T("游戏帮助"),
    _T("退出游戏")
    };

    for (int i = 0; i < 4; ++i) {
        drawButton(i, btnText[i]);
    }

    ExMessage msg;
    while (true) {
        if (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                for (int i = 0; i < 4; ++i) {
                    int y = START_Y + i * SPACING;
                    if (msg.x > (WIDTH - BTN_W) / 2 &&
                        msg.x < (WIDTH + BTN_W) / 2 &&
                        msg.y > y &&
                        msg.y < y + BTN_H) {
                        closegraph();
                        switch (i) {
                        case 0: startGame(); break;
                        case 2: showHelp(); break;
                        case 3: exit(0);
                        }
                        showMenu();
                        return;
                    }
                }
            }
        }
    }
}
// 游戏初始化
void initGame() {
    // 1. 初始化图形环境
    initgraph(WIDTH, HEIGHT);

    // 2. 设置字体 - 简化版本
    LOGFONT f;
    memset(&f, 0, sizeof(LOGFONT));
    _tcscpy_s(f.lfFaceName, _T("宋体"));  // 使用系统默认字体
    f.lfHeight = 24;
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);

    // 3. 加载背景图片
    loadBackgrounds();

    // 4. 初始化随机数种子
    srand(static_cast<unsigned>(time(0)));

    PlayMusic(BG_MUSIC);  // 开始播放背景音乐
}
int main() {
    // 初始化游戏
    initGame();

    // 进入主菜单
    showMenu();
    // 游戏结束前停止音乐
    StopMusic();
    return 0;
}