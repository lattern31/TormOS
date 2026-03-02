#include "math.h"

int module_diff(int a, int b) {
	if (a - b < 0) {
		return b - a;
	} else {
		return a - b;
	}
}

// Собственная реализация fmod для приведения угла
double my_fmod(double x, double y) {
    if (y == 0.0) return 0.0;
    return x - (int)(x / y) * y;
}

// Быстрый синус с использованием полинома
double fast_sin(double x) {
    // Приведение к диапазону [-π, π]
    x = my_fmod(x, TWO_PI);
    if (x > PI) x -= TWO_PI;
    else if (x < -PI) x += TWO_PI;
    
    // Полиномиальная аппроксимация (метод Чебышева)
    double x2 = x * x;
    double result = x * (1.0 - x2 * (1.0/6.0 - x2 * (1.0/120.0 - x2 * (1.0/5040.0))));
    
    return result;
}

// Быстрый косинус
double fast_cos(double x) {
    // Используем тождество cos(x) = sin(π/2 - x)
    return fast_sin(PI/2.0 - x);
}
