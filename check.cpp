#include <random>

void GenericRobot::fire(int dx, int dy) {
    std::random_device rd;
    std::mt19937 gen(rd()); // 随机种子初始化
    std::uniform_int_distribution<> dis(0, 99);

    int chance = dis(gen);
    if (chance < 70) {
        // 命中
    } else {
        // 未命中
    }
}

srand(time(0));
int num = rand() % empty_points.size();

140
161
214
249
273 //
348
chooseUpgrade