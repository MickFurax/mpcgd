#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

static void click_mouse(bool down) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

bool is_geometry_dash_active() {
    char window_title[256];
    HWND foreground_window = GetForegroundWindow();
    if (foreground_window) {
        GetWindowTextA(foreground_window, window_title, sizeof(window_title));
        return strstr(window_title, "Geometry Dash") != NULL;
    }
    return false;
}

bool touch_pad_active() {
    FILE *adbPipe = popen("adb shell dumpsys activity", "r");
    if (!adbPipe) {
        perror("Error: Failed to start ADB process");
        return EXIT_FAILURE;
    }
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), adbPipe)) 
        if (strstr(buffer, "Recent #0:"))
            break;
    pclose(adbPipe);
    const char *keywords[] = {"fluidsimfree", "com.wind", "com.mpcgd"};
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i)
        if (strstr(buffer, keywords[i]))
            return true;
    return false;
}

static FILE *adbPipe = NULL;

static void handle_signal(int signal_number) {
    if (signal_number == SIGINT) {
        printf("Exiting...\n", signal_number);
        if (adbPipe)
            pclose(adbPipe);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    char buffer[512];
    
    adbPipe = popen("adb shell getevent -l 2>NUL", "r");
    if (!adbPipe) {
        perror("Error: Failed to start ADB process");
        return EXIT_FAILURE;
    }
    signal(SIGINT, handle_signal);
    printf("Listening for touch events...\n");
    while (fgets(buffer, sizeof(buffer), adbPipe)) {
        if (is_geometry_dash_active()) {
            if (strstr(buffer, "UP"))
                click_mouse(false);
            else if (strstr(buffer, "DOWN"))
                click_mouse(true);
        }
    }
    pclose(adbPipe);
    return 0;
}
