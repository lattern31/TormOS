#include "../stdio.h"
#include "vga.h"
#include "font8x8.h"
#include "../math.h"

typedef uint8_t u8;

unsigned char double_buffer[DOUBLE_BUFFER_SIZE];
// Ваши данные режима
u8 g_320x200x256[] = {
/* MISC */
    0x63,
/* SEQ */
    0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
    0xFF,
/* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
/* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x41, 0x00, 0x0F, 0x00, 0x00
};

// Вспомогательные функции для работы с портами
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void dummy(void) {}
void write_registers(u8 *regs) {
    u8 i;
    
    // Запись регистров MISC
    outb(MISC_PORT, regs[0]);
    
    // Запись регистров SEQ
    for(i = 0; i < 5; i++) {
        outb(SEQ_PORT, i);
        outb(SEQ_PORT + 1, regs[1 + i]);
    }
    
    // Разблокировка CRTC регистров
    outb(CRTC_PORT, 0x03);
    outb(CRTC_PORT + 1, inb(CRTC_PORT + 1) | 0x80);
    outb(CRTC_PORT, 0x11);
    outb(CRTC_PORT + 1, inb(CRTC_PORT + 1) & ~0x80);
    
    regs[0x13] = (regs[0x13] & ~0x40) | 0x40;
    
    // Запись CRTC регистров
    for(i = 0; i < 25; i++) {
        outb(CRTC_PORT, i);
        outb(CRTC_PORT + 1, regs[6 + i]);
    }
    // Запись GC регистров
    for(i = 0; i < 9; i++) {
        outb(GC_PORT, i);
        outb(GC_PORT + 1, regs[31 + i]);
    }
    
    // Запись AC регистров 
    for(i = 0; i < 21; i++) {
        (void)inb(ISR1_PORT); // сброс flip-flop
        outb(AC_PORT, i);
        outb(AC_PORT, regs[40 + i]);
    }
    // Установка цветовой палитры
    (void)inb(ISR1_PORT); // сброс flip-flop
    outb(AC_PORT, 0x20);
}

void set_palette_color(int index, int r, int g, int b) {
    outb(DAC_WRITE_INDEX, index);  // Выбираем цвет
    outb(DAC_DATA, r);              // Красный (0-63)
    outb(DAC_DATA, g);              // Зеленый (0-63)
    outb(DAC_DATA, b);              // Синий (0-63)
}

void hex_to_vga(int hex_color, int *r, int *g, int *b) {
    *r = ((hex_color >> 16) & 0xFF) * 63 / 255;
    *g = ((hex_color >> 8) & 0xFF) * 63 / 255;
    *b = (hex_color & 0xFF) * 63 / 255;
}

void set_vga_mode_320x200x256(void) {
    write_registers(g_320x200x256);
    int r, g, b;
    hex_to_vga(0x40242f, &r, &g, &b);
    set_palette_color(0, r, g, b);
    hex_to_vga(0x075040, &r, &g, &b);
    set_palette_color(1, r, g, b);
    hex_to_vga(0xe1a847, &r, &g, &b);
    set_palette_color(2, r, g, b);
    hex_to_vga(0xc9dfb1, &r, &g, &b);
    set_palette_color(3, r, g, b);
}

void clear_screen(u8 color) {
    for(int i = 0; i < 320 * 200; i++) {
        double_buffer[i] = color;
    }
}

void put_pixel(PointScreen_t point, u8 color) {
    if(point.x >= 0 && point.x < 320 && point.y >= 0 && point.y < 200) {
        double_buffer[(point.y << 8) + (point.y << 6) + point.x] = color;
    }
}
void render_screen() {
    u8 *vram = (u8*)0xA0100;
    for (int i = 0; i < 64000; i++) {
        vram[i] = double_buffer[i];
        double_buffer[i] = '\0';
    }
}

void draw_character(char c, PointScreen_t start_point, int color) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((font8x8_basic[c][i] >> j) & 1) {
                put_pixel((PointScreen_t){
                    start_point.x+j, start_point.y+i
                }, color);
            }
        }
    }
}
void draw_string(char *str, PointScreen_t start_point, int color) {
    int i = 0;
    while (str[i] != '\0') {
        draw_character(str[i], (PointScreen_t){
            start_point.x+i*8, start_point.y
        }, color);
        i++;
    }
}

void draw_line(PointScreen_t a, PointScreen_t b, u8 color) {
    // dont draw line if:
    // all 2 points have z < 0
    // all 2 points have x < -1.6 or y < -1
    // all 2 points have x > 1.6 or y > 1
    put_pixel(a, color);
    put_pixel(b, color);

	int diff_x = module_diff(b.x, a.x);
	int diff_y = module_diff(b.y, a.y);

	PointScreen_t *first_p = a.x < b.x ? &a : &b;
    PointScreen_t *second_p = a.x > b.x ? &a : &b;

	int longest = diff_x > diff_y ? diff_x : diff_y;
	int shortest = diff_x < diff_y ? diff_x : diff_y;

	int counter = 0;
	for (int i=1; i<=longest; i++) {
		double idk = shortest * (double)i / (double)longest;

		if (first_p->y < second_p->y) {
			if (diff_x > diff_y) {
				put_pixel((PointScreen_t){first_p->x + i, first_p->y + counter}, color);
				if (idk - counter > 1) {
					counter++;
                    put_pixel((PointScreen_t){first_p->x + i, first_p->y + counter}, color);
				}
			} else {
				put_pixel((PointScreen_t){first_p->x + counter, first_p->y + i}, color);
				if (idk - counter > 1) {
					counter++;
                    put_pixel((PointScreen_t){first_p->x + counter, first_p->y + i}, color);
				}
			}
		} else {
			if (diff_x > diff_y) {
				put_pixel((PointScreen_t){first_p->x + i, first_p->y - counter}, color);
				if (idk - counter > 1) {
					counter++;
                    put_pixel((PointScreen_t){first_p->x + i, first_p->y - counter}, color);
				}
			} else {
                put_pixel((PointScreen_t){first_p->x + counter, first_p->y - i}, color);
				if (idk - counter > 1) {
					counter++;
                    put_pixel((PointScreen_t){first_p->x + counter, first_p->y - i}, color);
				}
			}
		}
    }
}

void draw_rectangular(PointScreen_t p1, PointScreen_t p2, u8 color) {
    for (int i = p1.y; i <= p2.y; i++) {
        for (int j = p1.x; j <= p2.x; j++) {
            put_pixel((PointScreen_t){j, i}, color);
        }
    }
}
