#include "stdint.h"
#include "stdio.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Буфер для ввода
#define BUFFER_SIZE 256
static char input_buffer[BUFFER_SIZE];
static unsigned int buffer_index = 0;

// Таблица скан-кодов (US QWERTY)
static const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Таблица для Shift
static const char scancode_to_ascii_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int caps_lock = 0;

// Функция для чтения из порта
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Функция для записи в порт
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Обработчик прерывания клавиатуры
void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Обрабатываем отпускание клавиши
    if (scancode & 0x80) {
        uint8_t key_released = scancode & 0x7F;
        
        switch (key_released) {
            case 0x2A: // Левый Shift
            case 0x36: // Правый Shift
                shift_pressed = 0;
                break;
            case 0x1D: // Ctrl
                ctrl_pressed = 0;
                break;
            case 0x38: // Alt
                alt_pressed = 0;
                break;
        }
    } else {
        // Обрабатываем нажатие клавиши
        switch (scancode) {
            case 0x2A: // Левый Shift
            case 0x36: // Правый Shift
                shift_pressed = 1;
                break;
                
            case 0x1D: // Ctrl
                ctrl_pressed = 1;
                break;
                
            case 0x38: // Alt
                alt_pressed = 1;
                break;
                
            case 0x3A: // Caps Lock
                caps_lock = !caps_lock;
                break;
                
            default:
                // Преобразуем скан-код в символ
                char c = 0;
                int use_shift = shift_pressed ^ caps_lock;
                
                if (use_shift) {
                    c = scancode_to_ascii_shift[scancode];
                } else {
                    c = scancode_to_ascii[scancode];
                }
                
                if (c && buffer_index < BUFFER_SIZE - 1) {
                    // Специальная обработка для Ctrl+C
                    if (ctrl_pressed && c == 'c') {
                        input_buffer[buffer_index++] = '\n';
                        input_buffer[buffer_index] = '\0';
                        // Можно добавить сигнал прерывания
                    } else {
                        input_buffer[buffer_index++] = c;
                        input_buffer[buffer_index] = '\0';
                    }
                    
                    // Выводим символ на экран (если есть VGA драйвер)
                     term_putc(c);
                }
                break;
        }
    }
    
    // Отправляем сигнал конца прерывания (EOI)
    outb(0x20, 0x20);
}

// Функция для получения введённой строки
char* kbgets(char* buffer, int max_len) {
    int i = 0;
    
    while (i < max_len - 1) {
        if (buffer_index > 0) {
            // Копируем символы из буфера
            buffer[i] = input_buffer[0];
            
            // Сдвигаем буфер
            for (unsigned int j = 1; j <= buffer_index; j++) {
                input_buffer[j-1] = input_buffer[j];
            }
            buffer_index--;
            
            // Если встретили новую строку - заканчиваем
            
            i++;
        }
    }
    
    buffer[i] = '\0';
    return buffer;
}

// Функция для проверки наличия ввода
int kbhit() {
    return buffer_index > 0;
}

// Очистка буфера клавиатуры
void clear_keyboard_buffer() {
    buffer_index = 0;
    input_buffer[0] = '\0';
}
