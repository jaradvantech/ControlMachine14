#ifndef PLCOMMUNICATIONS_H_
#define PLCOMMUNICATIONS_H_

#include <connexionPLCdatahandler.h>

extern TS7Client *ClientPlc14;

bool Check(TS7Client *Client, int Result, const char * function);
bool CliConnectPLC(TS7Client *Client, StructInfoPlc Config);
void CliDisconnectPLC(TS7Client *Client);
void attemptConnection(void);
void * PLCAdquisitionLoop(void *Arg);
void configurePLC();

#endif
