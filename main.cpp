#include <iostream>
#include <future>
#include <atomic>
#include <chrono>

std::atomic<bool> canceled(false); // Прапорець для скасування обчислень

// Функція f, яку потрібно обчислити 
int f(int x) {
    if (x <= 0) {
        canceled = true;
        return -1;
    }
    return x + 1;
}

// Функція g, яку потрібно обчислити 
int g(int x) {
    if (x >= 100) {
        canceled = true;
        return -1;
    }
    return x * 2;
}

// Функція для обчислення f(x) ⊗ g(x)
int calculate(int x) {
    if (canceled.load()) {
        return -1; // Скасовуємо обчислення
    }

    int result_f = f(x);
    int result_g = g(x);

    if (canceled.load()) {
        return -1; // Скасовуємо обчислення
    }

    return result_f ^ result_g;
}

int main() {
    int x;

    std::cout << "Введіть значення x: ";
    std::cin >> x;

    // Використовуємо std::async для асинхронного обчислення
    std::future<int> result_future = std::async(std::launch::async, calculate, x);

    // Встановлюємо обмеження часу на виконання в 5 секунд
    auto timeout = std::chrono::seconds(5);
    
    // Очікуємо завершення обчислення з таймаутом
    if (result_future.wait_for(timeout) == std::future_status::timeout) {
        canceled = true; // Скасовуємо обчислення
        std::cout << "Обчислення перевищили таймаут" << std::endl;
    } else {
        int result = result_future.get();
        if (canceled.load()) {
            std::cout << "Обчислення скасовано: причина: " << (x <= 0 ? "x <= 0" : "x >= 100") << std::endl;
        } else {
            std::cout << "Результат: " << result << std::endl;
        }
    }

    return 0;
}

