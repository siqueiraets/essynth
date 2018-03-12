#include <QApplication>
#include "ESMainWindowUI.h"

#include <execinfo.h>
#include <signal.h>

void print_backtrace(void) {
    void *bt[100];
    int bt_size;
    char **bt_syms;
    int i;

    bt_size = backtrace(bt, 100);
    bt_syms = backtrace_symbols(bt, bt_size);

    printf("Backtrace:\n");
    for (i = 0; i < bt_size; i++) {
        printf("\tStack: %d: %s\n", i, bt_syms[i]);
    }
    free(bt_syms);
}

void signal_handler(int) {
    print_backtrace();
    exit(-1);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ESMainWindowUI w;

    struct sigaction action;

    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGSEGV, &action, nullptr);

    w.show();
    return a.exec();
}
