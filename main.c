#include <cdk.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "seller_portal/portal.h"
#include "customer_portal/portal.h"

#define MAX_LINE_LENGTH 1024 
#define MAX_FIELDS 50         

int parse_line(char *line, char *fields[]) {
    int i = 0;
    char *token = strtok(line, ","); 

    while (token != NULL) {
        fields[i++] = token;
        token = strtok(NULL, ","); //seperate by comma
    }

    return i;  
}

void parse_csv(const char *filename, char *user_input1, char *user_input2) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    char *fields[MAX_FIELDS];

    // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline character
        int num_fields = parse_line(line, fields);
        if (strcmp(user_input1, fields[0]) == 0){
            if (strcmp(user_input2, fields[1]) == 0){
                if (*fields[2]=='S'){
                    run_tui();
                    break;
                }
                else{
                    start_customer_portal();
                    break;
                }
            }
        }
    }

    fclose(file);
}

int main() {

    CDKSCREEN *cdk_screen;
    CDKBUTTON *button;

    initscr();
    cdk_screen = initCDKScreen(stdscr);
    initCDKColor();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    refreshCDKScreen(cdk_screen);
    button = newCDKButton(cdk_screen, CENTER, CENTER, "      LOGIN       ", NULL, TRUE, FALSE);
    setCDKButtonBackgroundAttrib(button, COLOR_PAIR(1) | A_BOLD);
    drawCDKButton(button, TRUE);
    activateCDKButton(button, NULL);

    if (button->exitType == vNORMAL) {
        destroyCDKButton(button);
        refreshCDKScreen(cdk_screen);

        CDKENTRY *entry1, *entry2;
        char *user_input1, *user_input2;

        entry1 = newCDKEntry(cdk_screen, CENTER, CENTER, NULL, "Enter Username: ", A_NORMAL, '_', vMIXED, 40, 0, 256, TRUE, FALSE);
        user_input1 = activateCDKEntry(entry1, NULL);

        entry2 = newCDKEntry(cdk_screen, CENTER, CENTER, NULL, "Enter Password: ", A_NORMAL, '_', vMIXED, 40, 0, 256, TRUE, FALSE);
        user_input2 = activateCDKEntry(entry2, NULL);

        parse_csv("Users.csv",user_input1,user_input2);

        destroyCDKEntry(entry1);
        destroyCDKEntry(entry2);
    }
    destroyCDKScreen(cdk_screen);
    endCDK();
    endwin();

    return 0;
}
