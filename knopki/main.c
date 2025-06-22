#include <wiringPi.h>
#include <stdio.h>

#define N 4

void init_pin(int* arr, int mode);

int main() {
	int arr_pin_write[N] = {5, 21, 20, 16};
	int arr_pin_read[N] = {26, 19, 13, 6};

    if (wiringPiSetup() == -1) {
        printf("Ошибка инициализации WiringPi!\n");
        return 1;
    }

	init_pin(arr_pin_read, 0);
	init_pin(arr_pin_write, 1);

	int status = 0;

//	for (int i = 0; i < N;i++) {
//		digitalWrite(arr_pin_write[i], 1);
//		for (int j = 0; j < N;j++) {
//			status = digitalRead(arr_pin_read[j]);
//			if (status == 1) printf("click button: %d\n",(i*N+j));
//		}
//		digitalWrite(arr_pin_write[i], 1);
//		delay(5);
//	}
	return 0;
}

void init_pin(int* arr, int mode) {
	if (mode == 1) mode = INPUT;
	else mode = OUTPUT;

	for (int i = 0; i < N;i++) {
		pinMode(arr[i], mode);
	}
}
