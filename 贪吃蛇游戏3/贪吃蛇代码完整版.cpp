//ʵ����Ϸ����,�������,���ٹ���,����ͼƬ
#include <graphics.h>
#include <conio.h>
#include <deque>
#include <iostream>
#include <cmath>
#include <ctime>
#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")  // ����Windows��ý���
using namespace std;
const TCHAR* BG_MUSIC = _T("G:/CPP����ҵ/̰������Ϸ3/����/(What a nice surprise��̰����������Ϸ.mp3");
const TCHAR* EAT_SOUND = _T("G:/CPP����ҵ/̰������Ϸ3/����/��Ϸ����ռ�_������_[����ID��22093].wav");
const TCHAR* GAME_OVER_SOUND = _T("G:/CPP����ҵ/̰������Ϸ3/����/��Ϸ����_Freesound.aiff");

// �������ֺ��������ڱ�������
void PlayMusic(const TCHAR* filename, bool loop = true)
{
    TCHAR cmd[256];
    if (loop)
    {
        // �ȹرտ��ܴ��ڵı�������
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

// ���ų�ʳ����Ч
void PlayEatingSound()
{
    TCHAR cmd[256];

    // �ȹرտ��ܴ��ڵĳ�ʳ����Ч
    mciSendString(_T("close eat_sound"), NULL, 0, NULL);

    _stprintf_s(cmd, _T("open \"%s\" alias eat_sound"), EAT_SOUND);
    mciSendString(cmd, NULL, 0, NULL);
    mciSendString(_T("play eat_sound"), NULL, 0, NULL);
}

// ������Ϸ������Ч
void PlayGameOverSound()
{
    TCHAR cmd[256];

    // �ȹرտ��ܴ��ڵ���Ϸ������Ч
    mciSendString(_T("close over_sound"), NULL, 0, NULL);

    _stprintf_s(cmd, _T("open \"%s\" alias over_sound"), GAME_OVER_SOUND);
    mciSendString(cmd, NULL, 0, NULL);
    mciSendString(_T("play over_sound"), NULL, 0, NULL);
}

// ֹͣ��������
void StopMusic()
{
    mciSendString(_T("stop bg_music"), NULL, 0, NULL);
    mciSendString(_T("close bg_music"), NULL, 0, NULL);
}

// ֹͣ��Ϸ������Ч
void StopGameOverSound()
{
    mciSendString(_T("stop over_sound"), NULL, 0, NULL);
    mciSendString(_T("close over_sound"), NULL, 0, NULL);
}

#define WIDTH 800
#define HEIGHT 600
#define SIZE 20
#define DELAY 160      // �ƶ��ٶȵ���
#define BASE_SPEED 0.8f // �����ƶ��ٶ�ϵ��
#define MAX_SPEED 1.4f  // ����ƶ��ٶ�ϵ��
#define SPEED_INCREMENT 0.1f // ÿ�γԵ���ɫʳ�����ӵ��ٶ�
#define FOOD_COUNT 5    // ͬʱ���ڵ�ʳ������
#define BTN_W 200
#define BTN_H 50
#define START_Y 200
#define SPACING 80
// ����ͼƬ�ṹ
struct GameBackgrounds {
    IMAGE menuBg;    // �˵�����
    IMAGE gameBg;    // ��Ϸ����
    IMAGE helpBg;    // ��������
} bg;

// �������б���ͼƬ
void loadBackgrounds() {
    loadimage(&bg.menuBg, _T("G:/CPP����ҵ/̰������Ϸ3/ͼƬ/̰���߲˵�����.png"), WIDTH, HEIGHT);
    loadimage(&bg.gameBg, _T("G:/CPP����ҵ/̰������Ϸ3/ͼƬ/��Ϸ����.png"), WIDTH, HEIGHT);
    loadimage(&bg.helpBg, _T("G:/CPP����ҵ/̰������Ϸ3/ͼƬ/��Ϸ��������.jpg"), WIDTH, HEIGHT);
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
    COLORREF color; // ʳ����ɫ
};

// ��������
void showMenu();
void isOver(const deque<Snake>& snake);
void startGame();
void showHelp();
void initGame();
// ������������
float distance(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// ��ȫ�ػ���Ϸ���棬�������кۼ�
void redrawScene(const deque<Snake>& snake, const Food foods[], int score, float currentSpeed) {
    cleardevice();
    putimage(0, 0, &bg.gameBg);  // �Ȼ��Ʊ���

    // ��������
    for (size_t i = 0; i < snake.size(); ++i) {
        Snake body = snake[i];

        // ���㽥����ɫ - �����̵�ǳ��
        int green = 50 + static_cast<int>(205.0f * i / snake.size());

        if (i == 0) {
            // ��ͷ - ����ɫ
            setfillcolor(RGB(0, 100, 0));
        }
        else {
            // ���� - ����ɫ
            setfillcolor(RGB(0, green, 0));
        }

        int centerX = static_cast<int>(body.x * SIZE + SIZE / 2);
        int centerY = static_cast<int>(body.y * SIZE + SIZE / 2);
        fillcircle(centerX, centerY, SIZE / 2 - 1);
    }

    // ��������ʳ��
    for (int i = 0; i < FOOD_COUNT; i++) {
        if (!foods[i].eaten) {
            setfillcolor(foods[i].color);
            int centerX = foods[i].x * SIZE + SIZE / 2;
            int centerY = foods[i].y * SIZE + SIZE / 2;
            fillcircle(centerX, centerY, SIZE / 2 - 1);
        }
    }

    // ��ʾ�������ٶ�
    settextcolor(YELLOW);
    settextstyle(25, 0, _T("Consolas"));
    TCHAR scoreStr[50];
    _stprintf_s(scoreStr, _T("Score: %d  Speed: %.1f"), score, currentSpeed);
    outtextxy(10, 10, scoreStr);
}

void isOver(const deque<Snake>& snake) {
    // �߽���ײ���
    if (snake.front().x < 0 || snake.front().x >= WIDTH / SIZE ||
        snake.front().y < 0 || snake.front().y >= HEIGHT / SIZE)
    {

        // ���浱ǰ����״̬
        HWND hwnd = GetHWnd();

        // ��ֹͣ��������
        StopMusic();

        // ��ʾ������Ϣ
        cleardevice();
        settextcolor(RED);
        settextstyle(100, 0, _T("Consolas"));
        outtextxy(WIDTH / 2 - 300, HEIGHT / 2 - 50, _T("GAME OVER!!!"));

        // ������Ϸ������Ч
        PlayGameOverSound();

        FlushBatchDraw();

        // �ȴ�3��
        Sleep(3000);

        // ֹͣ��Ϸ������Ч
        StopGameOverSound();

        // ������Ϸ����
        EndBatchDraw();
        closegraph();  // �رյ�ǰ����

        // ���³�ʼ������ʾ�˵�
        initgraph(WIDTH, HEIGHT);  // �����´���

        // ���¿�ʼ���ű�������
        PlayMusic(BG_MUSIC);

        showMenu();
    }

    // ������ײ���
    if (snake.size() > 4) {
        for (auto it = snake.begin() + 4; it != snake.end(); ++it) {
            if (abs(snake.front().x - it->x) < 0.9f && abs(snake.front().y - it->y) < 0.9f) {
                // ���浱ǰ����״̬
                HWND hwnd = GetHWnd();

                // ��ֹͣ��������
                StopMusic();

                // ��ʾ������Ϣ
                cleardevice();
                settextcolor(RED);
                settextstyle(100, 0, _T("Consolas"));
                outtextxy(WIDTH / 2 - 300, HEIGHT / 2 - 50, _T("GAME OVER!!!"));

                // ������Ϸ������Ч
                PlayGameOverSound();

                FlushBatchDraw();

                // �ȴ�3��
                Sleep(3000);

                // ֹͣ��Ϸ������Ч
                StopGameOverSound();

                // ������Ϸ����
                EndBatchDraw();
                closegraph();  // �رյ�ǰ����

                // ���³�ʼ������ʾ�˵�
                initgraph(WIDTH, HEIGHT);  // �����´���

                // ���¿�ʼ���ű�������
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
	srand(static_cast<unsigned>(time(0)) + seedOffset++);// ȷ��ÿ������ʳ��ʱ��ʹ�ò�ͬ���������

    for (int i = 0; i < FOOD_COUNT; i++) {
        if (foods[i].eaten) {
            while (true) {
                foods[i].x = rand() % (WIDTH / SIZE);
                foods[i].y = rand() % (HEIGHT / SIZE);
                bool overlap = false;

                // ����Ƿ��������ص�
                for (const auto& body : snake) {
                    if (abs(body.x - foods[i].x) < 1.0f && abs(body.y - foods[i].y) < 1.0f) {
                        overlap = true;
                        break;
                    }
                }

                // ����Ƿ�������ʳ���ص�
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
            // ������ɺ�ɫ����ɫʳ��
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
            // ���ų�ʳ����Ч
            PlayEatingSound();

            if (foods[i].color == BLUE) {
                // �Ե���ɫʳ������ٶ�ϵ��
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
    BeginBatchDraw();  // ������������

    // ���峣��
    const int totalLines = 10;
    const int visibleLines = 5;
    const int lineHeight = 40;
    const int textLeftMargin = 120;
    const int buttonWidth = 200;
    const int buttonHeight = 50;

    int currentPage = 0;

    const TCHAR* helpText[] = 
    {
        _T("1. ʹ�÷������WASD�����������ߵ��ƶ�����"),
        _T("2. �Ե���ɫʳ��������䳤"),
        _T("3. �Ե���ɫʳ����ߵ��ٶȻ�����"),
        _T("4. ײ��ǽ�ڻ��Լ���������Ϸ����"),
        _T("5. ��ESC��������ʱ�˳���Ϸ"),
        _T("6. ��Ϸ����:"),
        _T("- ���������ڳ��������ƶ�"),
        _T("- ��ǰ�滮�ƶ�·�ߣ�������������"),
        _T("- ������ϳ�ʱ���ƶ�Ҫ���ӽ���"),
        _T("ף����Ϸ���!")
    };

    auto drawHelpPage = [&](int page) {
        cleardevice();

        // ���Ʊ���
        putimage(0, 0, &bg.helpBg);

        // ��͸��������
        setfillcolor(RGB(0, 0, 0, 180));
        fillrectangle(50, 50, WIDTH - 50, HEIGHT - 100);

        // ����
        settextcolor(LIGHTGREEN);
        settextstyle(48, 0, _T("Arial"));
        outtextxy(WIDTH / 2 - textwidth(_T("��Ϸ����")) / 2, 50, _T("��Ϸ����"));

        // �����ı�
        settextcolor(WHITE);
        settextstyle(24, 0, _T("����"));
        int y = 150;
        for (int i = 0; i < visibleLines; ++i) {
            int lineIndex = page * visibleLines + i;
            if (lineIndex < totalLines) {
                outtextxy(textLeftMargin, y, helpText[lineIndex]);
                y += lineHeight;
            }
        }

        // ���ذ�ť
        setfillcolor(GREEN);
        fillroundrect(WIDTH / 2 - buttonWidth / 2, HEIGHT - 100,
            WIDTH / 2 + buttonWidth / 2, HEIGHT - 50, 10, 10);

        settextcolor(WHITE);
        settextstyle(28, 0, _T("Arial"));
        outtextxy(WIDTH / 2 - textwidth(_T("�������˵�")) / 2,
            HEIGHT - 90, _T("�������˵�"));

        FlushBatchDraw();  // ˢ�»���
        };

    drawHelpPage(currentPage);

    ExMessage msg;
    while (true) {
        if (peekmessage(&msg, EX_KEY | EX_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                // ����Ƿ����˷��ذ�ť
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
                case VK_UP:  // �Ϸ�ҳ
                    if (currentPage > 0) {
                        --currentPage;
                        drawHelpPage(currentPage);
                    }
                    break;
                case VK_DOWN:  // �·�ҳ
                    if (currentPage < (totalLines - 1) / visibleLines) {
                        ++currentPage;
                        drawHelpPage(currentPage);
                    }
                    break;
                case VK_ESCAPE:  // ESC���˳�
                    EndBatchDraw();
                    closegraph();
                    return;
                }
            }
        }
    }
}
// �޸ĺ��showMenu�������˵����棩
void showMenu() {
    initgraph(WIDTH, HEIGHT);
    // ���Ʋ˵�����
    putimage(0, 0, &bg.menuBg);

    // �˵���ť��ʽ����͸��Ч����
    auto drawButton = [&](int index, const TCHAR* text) {
        int y = START_Y + index * SPACING;

        // ��͸����ť����
        setfillcolor(RGB(0, 0, 0, 150));
        fillroundrect((WIDTH - BTN_W) / 2, y, (WIDTH + BTN_W) / 2, y + BTN_H, 10, 10);

        // ��ť����
        settextcolor(WHITE);
        settextstyle(28, 0, _T("Arial"));
        int textWidth = textwidth(text);
        int textHeight = textheight(text);
        outtextxy((WIDTH - textWidth) / 2, y + (BTN_H - textHeight) / 2, text);
        };

    const TCHAR* btnText[] = {
    _T("��ʼ��Ϸ"),
    _T("������Ϸ"),
    _T("��Ϸ����"),
    _T("�˳���Ϸ")
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
// ��Ϸ��ʼ��
void initGame() {
    // 1. ��ʼ��ͼ�λ���
    initgraph(WIDTH, HEIGHT);

    // 2. �������� - �򻯰汾
    LOGFONT f;
    memset(&f, 0, sizeof(LOGFONT));
    _tcscpy_s(f.lfFaceName, _T("����"));  // ʹ��ϵͳĬ������
    f.lfHeight = 24;
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);

    // 3. ���ر���ͼƬ
    loadBackgrounds();

    // 4. ��ʼ�����������
    srand(static_cast<unsigned>(time(0)));

    PlayMusic(BG_MUSIC);  // ��ʼ���ű�������
}
int main() {
    // ��ʼ����Ϸ
    initGame();

    // �������˵�
    showMenu();
    // ��Ϸ����ǰֹͣ����
    StopMusic();
    return 0;
}