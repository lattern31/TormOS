#include "shell.h"

#include "vga/gui.h"
#include "nparse.h"
#include "string.h"
#include "common.h"
#include "timer.h"

char* get_args(char *command) {
    uint8_t i = 0;
    while (command[i] != ' ') {
        if (command[i] == '\0') {
            return INVALID_PTR;
        }
        i++;
    }
    return &command[i+1];
}

void handle_calc_command(StateTerminal_t *terminal, char *args_ptr) {
    if (args_ptr == INVALID_PTR) {
        vga_terminal_print(terminal, "no args");
        return;
    }

    //  need to refactore
    double numbers[16] = {0};
    double *numbers_ptr = numbers;
    char operations[15] = {0};
    char *op_ptr = operations;
    char number_buffer[16] = {0};
    char *n_ptr = number_buffer;

    char first_c = args_ptr[0];
    if (first_c == '-' || first_c == '.' || (first_c >= '0' && first_c <= '9')) {
        *n_ptr++ = first_c;
    } else {
        vga_terminal_print(terminal, "invalid argument");
        return;
    }
    int i = 1; //  also should look at previous symbol
               //  if prev was space all crashes dumbass
    char prev_c = first_c;
    while (args_ptr[i] != '\0') {
        char c = args_ptr[i];
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
                    vga_terminal_print(terminal, tmp);
                    vga_terminal_print(terminal, "invalid arg");
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
    vga_terminal_print(terminal, buf);
}

void handle_sleep_command(StateTerminal_t *terminal, char *args_ptr) {
    if (args_ptr == INVALID_PTR) {
        vga_terminal_print(terminal, "no args");
        return;
    }
    double time = parsenumber(args_ptr);
    timer_sleep((uint32_t)time);
}

void handle_tabnew_command(StateTerminal_t *terminal, char *args_ptr) {
    if (args_ptr == INVALID_PTR) {
        vga_terminal_print(terminal, "bad argument");
        return;
    }
    int cnt = 0;
    while (tab_stack[cnt] != INVALID_PTR) {
        cnt++;
    }
    if (cnt >= MAXTAB) {
        vga_terminal_print(terminal, "max tabs");
        return;
    }
    if (strcmp(args_ptr, "TERMINAL")) {
        tab_stack[cnt] = create_tab(TERMINAL);
    } else if (strcmp(args_ptr, "SCENE")) {
        tab_stack[cnt] = create_tab(SCENE);
    } else if (strcmp(args_ptr, "INFO")) {
        tab_stack[cnt] = create_tab(INFO);
    }
};

void handle_command(StateTerminal_t *terminal) {
    char *args_ptr = get_args(terminal->command);
    uint8_t is_cmd_ran = 1; // need to differ cmd print and users
    if (startswith(terminal->command, "calc")) {
        handle_calc_command(terminal, args_ptr);
    } else if (startswith(terminal->command, "help")) {
        vga_terminal_print(terminal, HELP_MSG); 
    } else if (startswith(terminal->command, "sleep")) {
        handle_sleep_command(terminal, args_ptr);
    } else if (startswith(terminal->command, "tabnew")) {
        handle_tabnew_command(terminal, args_ptr);
    } else {
        is_cmd_ran = 0;
    }

    //  reseting back 
    vga_terminal_print(terminal, is_cmd_ran ? "\n$" : "$");

    int i = 0;
    while (terminal->command[i] != '\0') {
        terminal->command[i] = '\0';
        i++;
    }
    terminal->cmd_ptr = &terminal->command[0];
}
