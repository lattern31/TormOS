#include "stdio.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "keyboard.h"
#include "pic.h"
#include "vga/vga.h"
#include "vga/3d.h"
#include "vga/terminal.h"
#include "vga/gui.h"
#include "math.h"

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif

void debug_point(Point_3d_t p) {
    char bufs[3][5];

    dbl_to_str(bufs[0], p.x);
    dbl_to_str(bufs[1], p.y);
    dbl_to_str(bufs[2], p.z);
    for (int i = 0; i < 3; i++) {
        vga_terminal_print(bufs[i]);
        vga_terminal_print(" ");
    }
    vga_terminal_index[0]++;
    vga_terminal_index[1] = 0;
}

void handle_command(char *command) {
    for (int i = 0; i < 256; i++) {
    }
    vga_terminal_print(command);
}

void handle_keyboard() {
    if (kbhit()) {
        char buffer[2];
        kbgets(buffer, sizeof(buffer));

        if (buffer[0] == '\t') {
            tab = (tab + 1) % 3;
            return;
        }

        if (tab == 0) {
            char str[2] = {buffer[0], '\0'};
            if (str[0] == '\n') {
                handle_command(vga_terminal_command);
                for (int i = 0; i < 256; i++) {
                    vga_terminal_command[i] = '\0';
                }
                vga_terminal_command_pointer = vga_terminal_command;
            }
            *vga_terminal_command_pointer = str[0];
            vga_terminal_command_pointer++;
            vga_terminal_print(str);
        }

        if (tab == 1 || tab == 2) {
            if (buffer[0] == 'w') {
                cam = move_cam_by_yaw(cam, global_angle, 0.05);
                //debug_point(cam);
                global_dz -= 0.05;
            }
            if (buffer[0] == 's') {
                cam = move_cam_by_yaw(cam, global_angle, -0.05);
                global_dz += 0.05;
            }
            if (buffer[0] == 'q') {
                global_angle -= PI * 0.05;
            }
            if (buffer[0] == 'e') {
                global_angle += PI * 0.05;
            }
            if (buffer[0] == 'z') {
                test_angle += PI * 0.05;
            }
        }
    }
}


void kernel_main() {
	//term_init();
    gdt_init();
    idt_init();
    pic_init();
    ISR_init();

    asm volatile("sti");

    set_vga_mode_320x200x256();

    vga_terminal_history[0][0] = '$';

    Figure_t cube1 = create_cube(0.3, (Point_3d_t){0, 0, 1}, 0.1);
    Figure_t cube2 = create_cube(0.3, (Point_3d_t){0, 1, -1}, 0.1);
    Figure_t tetrahedron1 = create_tetrahedron(
        1, (Point_3d_t){0.3, 0.5, 1}, 0.1
    );
    Figure_t *figures[] = {&cube1, &cube2, &tetrahedron1};

    while (1) {
        handle_keyboard();
        clear_screen(0);
        switch (tab) {
            case 0:
                draw_terminal();
                break;
            case 1:
                draw_scene(figures, 3);
                break;
            case 2:
                draw_figure(&tetrahedron1);
                break;
        }
        draw_tab(tab);
        render_screen();
    }
     

    term_print("> ");
    char buffer[100];
    while (1) {
        if (kbhit()) {
            kbgets(buffer, sizeof(buffer));
            term_print("> ");
        }
        for (volatile int i = 0; i < 1000000; i++);
    }
}
