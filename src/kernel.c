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

void handle_calc_command(char *command) {
    //  need to refactore
    int i = 4; //  after word calc
    if (command[i] == '\0') {
        vga_terminal_print("bad argument");
        return;
    }
    while (command[i] == ' ') {
        i++;
    }
    if (command[i] == '\0') {
        vga_terminal_print("bad argument");
        return;
    }

    double numbers[16] = {0};
    double *numbers_ptr = numbers;
    char operations[15] = {0};
    char *op_ptr = operations;
    char number_buffer[16] = {0};
    char *n_ptr = number_buffer;

    char first_c = command[i];
    if (first_c == '-' || first_c == '.' || (first_c >= '0' && first_c <= '9')) {
        *n_ptr++ = first_c;
    } else {
        vga_terminal_print("invalid argument");
        return;
    }

    i++; //  also should look at previous symbol
         //  if prev was space all crashes dumbass
    char prev_c = first_c;
    while (command[i] != '\0') {
        char c = command[i];
        if (c == ' ') {
            i++;
            continue;
        }

        switch (c) {
            case '-':
                if (*n_ptr == '-') { 
                    // if prev minus went to n minus again goes to n 
                    *n_ptr++ = c;
                    break;
                }
                if (prev_c == '+' || prev_c == '-' || prev_c == '*' || prev_c == '/') {
                    // if prev was op minus goes to number
                    *n_ptr++ = c;
                    break;
                }
            case '+':
            case '*':
            case '/':
                if (!(prev_c >= '0' && prev_c <= '9')) {
                    char tmp[] = {prev_c, ' ', c, ' ', '\0'};
                    vga_terminal_print(tmp);
                    vga_terminal_print("invalid arg");
                    return;
                }

                *op_ptr++ = c;

                //  make unary ops and parse number
                double number = parsenumber(number_buffer);
                *numbers_ptr++ = number;

                for (int j = 0; j < 16; j++) {
                    number_buffer[j] = '\0';
                }
                n_ptr = number_buffer;

                break;

            default:
                if (c >= '0' && c <= '9') {
                    *n_ptr++ = c;
                }
                break;
        }
        i++;
        prev_c = c;
    }

    double number = parsenumber(number_buffer);

    *numbers_ptr++ = number;


    int q = 0;
    while (operations[q] != '\0') {
        if (operations[q] == '*' || operations[q] == '/') {
            double result;
            if (operations[q] == '*') {
                result = numbers[q] * numbers[q+1];
            } else if (operations[q] == '/') {
                result = numbers[q] / numbers[q+1];
            }

            numbers[q] = result;
            int w = q+1;
            while (w < 14) {
                numbers[w] = numbers[w+1];
                w++;
            }
            w = q;
            while (w < 14) {
                operations[w] = operations[w+1];
                w++;
            }
            q = 0; continue;
        }
        q++;
    }

    q = 0;
    while (operations[q] != '\0') {
        if (operations[q] == '+' || operations[q] == '-') {
            double result;
            if (operations[q] == '+') {
                result = numbers[q] + numbers[q+1];
            } else if (operations[q] == '-') {
                result = numbers[q] - numbers[q+1];
            }

            numbers[q] = result;
            int w = q+1;
            while (w < 14) {
                numbers[w] = numbers[w+1];
                w++;
            }
            w = q;
            while (w < 14) {
                operations[w] = operations[w+1];
                w++;
            }
            q = 0;
            continue;
        }
        q++;
    }

    char buf[16];
    dbl_to_str(buf, numbers[0]);
    vga_terminal_print(buf);
}

void handle_command(char *command) {
    //  actually handling command here
    if (startswith(command, "calc")) {
        handle_calc_command(command);
    } else if (startswith(command, "yo")) {
        char buf[16];
        dbl_to_str(buf, -1.4);
        vga_terminal_print(buf); 
    } else {
        vga_terminal_print(command); 
    }

    //  reset back to user input
    vga_terminal_print("\n");
    vga_terminal_history[vga_terminal_index[0]][0] = '$';
    vga_terminal_index[1] = 1;

    //  reset command buffer
    int i = 0;
    while (vga_terminal_command[i] != '\0') {
        vga_terminal_command[i] = '\0';
        i++;
    }
    vga_terminal_command_pointer = vga_terminal_command;
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

            vga_terminal_print(buffer); //  print inputed char 
            if (str[0] == '\n') {
                handle_command(vga_terminal_command);
            } else {
                *vga_terminal_command_pointer = str[0];
                vga_terminal_command_pointer++;
            }
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
