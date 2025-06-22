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

int arr_all[] = {A, B, C, D, E, F, G};

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

int check_queue() {
    FILE *fp = popen("lpstat -o | wc -l", "r");
    int count;
    fscanf(fp, "%d", &count);  // Считываем число
    pclose(fp);
	return count;
}

void clear_pin() {
    for (int i = 0;i < CON_ELM;i++)
        gpioWrite(arr_all[i], 0);
}

void set_mode(int st) {
    for (int i = 0;i < CON_ELM;i++)
	    gpioSetMode(arr_all[i], st);
}

void out_displ_numb(int num) {
	if (num>9) num = 9;
    for (int i = 0;i < CON_ELM;i++) {
        if (arr_num[num][i] == arr_all[i]) gpioWrite(arr_all[i], 1);
        else gpioWrite(arr_all[i], 0);
    }

}

void out_queue_num(int var) {
    if (var == 1) {
        set_mode(PI_OUTPUT);
        if (check_queue() == 0) {
            out_displ_numb(check_queue());
            usleep(DELAY_TIME*20);
            clear_pin();
        } else out_displ_numb(check_queue());
    } else clear_pin();
}

void manag_press_button() {
    struct timespec press_start, press_end;

    int time_st = 0;
    long int time_press;
    clock_gettime(CLOCK_MONOTONIC, &press_start);

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &press_end);
        time_press = (press_end.tv_sec - press_start.tv_sec) +
        			 (press_end.tv_nsec - press_start.tv_nsec) / 1e9;

        if (time_press > time_st) {
			time_st = time_press;
			out_displ_numb(time_press);
        }

        if (gpioRead(PIN_BUTTON_READ) == 0) {
            usleep(DELAY_TIME*10);
        	clear_pin();
        	break;
    	}
    }

	if (time_press >= 9) system("sudo poweroff");
    else if (time_press >= 6) system("sudo reboot");
    else if (time_press >= 3) system("cancel -a");
    else out_queue_num(1);

}



void manag_leds() {
    while (check_queue() > 0) {
        out_queue_num(1);
        if (gpioRead(PIN_BUTTON_READ) == 1) manag_press_button();
            usleep(DELAY_TIME*10);
    }
    out_queue_num(0);
}

int main() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialisation failed\n");
        return 1;
    }
	gpioSetMode(PIN_BUTTON_READ, PI_INPUT);
    while (1) {
            if (check_queue() > 0) manag_leds();
            else if (gpioRead(PIN_BUTTON_READ) == 1) manag_press_button();

            if (localtime(&(time_t){time(NULL)})->tm_hour >= 7) usleep(DELAY_TIME);
            else usleep(DELAY_TIME*20);
        }


    gpioTerminate();
    return 0;
}
