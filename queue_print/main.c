#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <time.h>

#define A 13
#define B 6
#define C 16
#define D 20
#define E 21
#define F 19
#define G 26

#define PIN_BUTTON_READ 12

#define SK -1

#define CON_ELM 7
#define DELAY_TIME 50000 // 0,05


int check_queue() {
    FILE *fp = popen("lpstat -o | wc -l", "r");
    if (!fp) {
        perror("Failed to execute lpstat");
        return -1;  // Возвращаем -1 в случае ошибки
    }

    char num[16];  // Достаточный размер для чисел
    if (!fgets(num, sizeof(num), fp)) {
        pclose(fp);
        return -1;  // Ошибка чтения
    }

    int status = pclose(fp);
    if (status == -1) {
        perror("Failed to close pipe");
        return -1;
    } else if (WEXITSTATUS(status) != 0) {
        fprintf(stderr, "lpstat or wc command failed\n");
        return -1;
    }
    	if (atoi(num) >9) return 9;
    	else return atoi(num);
}

void clear_pin(int* arr_all) {
    for (int i = 0;i < CON_ELM;i++)
            gpioWrite(arr_all[i], 0);
}

void set_mode(int* arr_all, int st) {
    for (int i = 0;i < CON_ELM;i++)
    gpioSetMode(arr_all[i], st);
}

void out_displ_numb(int arr[][9], int* arr_all,int num) {
    for (int i = 0;i < CON_ELM;i++) {
            if (arr[num][i] == arr_all[i]) gpioWrite(arr_all[i], 1);
            else gpioWrite(arr_all[i], 0);
    }

}

void out_queue_num(int var) {
    int arr_all_num[] = {A, B, C, D, E, F, G};

	if (var == 1) {
	    int arr_num[10][9] = {
	        { A, B, C, D, E, F, SK, SK},  // num_0
	        {SK, B, C, SK, SK, SK, SK, SK},  // num_1
	        { A, B, SK, D, E, SK, G},  // num_2
	        { A, B, C, D, SK, SK, G, SK},  // num_3
	        {SK, B, C, SK, SK, F, G, SK},  // num_4
	        { A, SK, C, D, SK, F, G},  // num_5
	        { A, SK, C, D, E, F, G},  // num_6
	        { A, B, C, SK, SK, SK, SK, SK},  // num_7
	        { A, B, C, D, E, F, G, SK},  // num_8
	        { A, B, C, D, SK, F, G}   // num_9
	    };

	    set_mode(arr_all_num, PI_OUTPUT);
	    if (check_queue() == 0) {
	    	out_displ_numb(arr_num, arr_all_num, check_queue());
	    	usleep(DELAY_TIME*20);
	    	clear_pin(arr_all_num);
	    } else out_displ_numb(arr_num, arr_all_num, check_queue());
	} else clear_pin(arr_all_num);
}

void manag_press_button() {
    struct timespec press_start, press_end;

    clock_gettime(CLOCK_MONOTONIC, &press_start);

    while (1) {
        if (gpioRead(PIN_BUTTON_READ) == 0) break;
    }
    clock_gettime(CLOCK_MONOTONIC, &press_end);
    long int time_press = (press_end.tv_sec - press_start.tv_sec) +
                 (press_end.tv_nsec - press_start.tv_nsec) / 1e9;

    if (time_press >= 8) system("sudo reboot");
    else if (time_press >= 3) system("cancel -a");
    else out_queue_num(1);

}



void manag_leds() {
        while (check_queue() > 0) {
            out_queue_num(1);
            if (gpioRead(PIN_BUTTON_READ) == 1) {manag_press_button();printf("b\n");}
	        usleep(DELAY_TIME*10);
        }
        out_queue_num(0);
}

int main() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialisation failed\n");
        return 1;
    }

    while (1) {
	    if (check_queue() > 0) manag_leds();
	    else if (gpioRead(PIN_BUTTON_READ) == 1) manag_press_button();
	    usleep(DELAY_TIME);
	}


    gpioTerminate();
    return 0;
}
