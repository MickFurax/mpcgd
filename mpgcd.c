#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void click_mouse(bool down) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

int main() {
    char buffer[512];
    
    FILE *adbPipe = popen("adb shell getevent -l", "r");
    if (!adbPipe) {
        perror("Error: Failed to start ADB process");
        return EXIT_FAILURE;
    }
    printf("Listening for touch events...\n");
    while (fgets(buffer, sizeof(buffer), adbPipe)) {
        if (strstr(buffer, "UP"))
            click_mouse(false);
        else if (strstr(buffer, "DOWN"))
            click_mouse(true);
    }
    pclose(adbPipe);
    return 0;
}

// adb shell dumpsys activity