#include "csapp.h"

using namespace std;
void sigint_handler(int sig)
{
    // do nothing
}
unsigned int snooze(unsigned int secs)
{
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
        unix_error("signal error");
    unsigned rc = sleep(secs);
    printf("\nSlept for %d of %d secs.\n", secs - rc, secs);
    return rc;
}
bool is_number(const char *s)
{
    int len = strlen(s);
    for (int i = 0; i < len; i++)
        if (s[i] < '0' or s[i] > '9')
            return false;
    return true;
}
void do_snooze(int argc, char *argv[])
{
    bool _exit = false;
    if (argc == 1)
    {
        unix_error("ERROR:need one number argument\n");
    }
    if (argc > 2)
    {
        unix_error("ERROR:need only one number argument\n");
    }

    if (!is_number(argv[1]))
    {
        unix_error("ERROR:need number argument\n");
    }
    //-----------
    int num = atoi(argv[1]);
    snooze(num);
}
int main(int argc, char *argv[])
{
    do_snooze(argc, argv);
}
