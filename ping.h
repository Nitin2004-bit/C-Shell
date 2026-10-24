#ifndef PING_H
#define PING_H
void ping(char* command);
void hand_sig(int signal);
bool check_process(int procid);
// void handle_sigtstp(int signo);
void Unblocksignals();
void PrintSignalMask();
#endif