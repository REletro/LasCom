/**
 * @brief Initiate Communication with the Laser Driver
 *          Prerequisites: This code is made only for the windows 7 Ultimate OS using win32 API.
 *          1. Laser Driver (w/ RS-232) connected to USB port of the Laptop.
 *          2. Both Tx and Rx Device on and 
 *          3. Tx laser is pointing at Rx-PhotoDetector
 * @author Rajesh Kumar
 * 
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

HANDLE hPort;
DCB dcb;
COMMTIMEOUTS to;

/*---------------Function  Declarations----------*/
/** @brief : Initialize the communication protocol parameters for the COM Port 
*/
int InitStruct(int port)
{
    char COM[10];
    sprintf(COM, "\\\\.\\COM%d", port);
    printf("\nINFO: Creating Port FIle at [%s]\n", COM);
    hPort = CreateFile(COM, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (hPort == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "\nERROR: Invalid Handle Value.. \n");
        return 0;
    }

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(hPort, &dcb))
    {
        fprintf(stderr, "\nERROR: DCB Initialization Error...\n");
        return 0;
    }
    to.ReadIntervalTimeout = 50;
    to.ReadTotalTimeoutConstant = 50;
    to.READTotalTImeoutMultiplier = 10;
    to.WriteTotalTimeoutConstant = 50;
    to.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hPort, &to))
    {
        fprintf(stderr, "\nERROR:COMMTIMEOUTS Initialization Error..\n");
        return 0;
    }
    return 1;
}
/*--------------------------------------------------------------------*/
/** @brief : Read bytes from the COM port at the Laser Driver connection Port 
*/
void ReadByte()
{
    char data = 'R';
    DWORD dwBytesTransferred;
    DWORD dwCommModemStatus;
    if (!GetCommState(hPort, &dcb))
    {
        fprintf(stderr, "\nERROR: Couldn't Get the State of the COM-Port..\n");
    }
    else
    {
        if (SetCommMask(hPort, EV_RXCHAR))
        {
            if (WaitCommEvent(hPort, &dwCommModemStatus, NULL))
            {
                do
                {
                    if (ReadFile(hPort, &data, 1, &dwBytesTransferred, 0))
                    {
                        printf("%c", data);
                    }
                    else
                        fprintf(stderr, "\nERROR: Bytes Stream Sending NULL Values...\n");
                } while (dwBytesTransferred);
            }
        }
        else
            fprintf(stderr, "\nERROR:Couldn't Set COM Mask...\n");
    }
}
/*--------------------------------------------------------------------*/
/** @brief : Main Program
*/
int main(int argc, char *argv[])
{
    int port = 2;
    system("TITLE Reading Data from Serial Port");
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    port %= 257;
    if (port == 0)
        port = 2;
    system("CLS");
    if (!InitStruct(port))
    {
        system("echo. &echo. &PAUSE");
        return -1;
    }else{printf("\nINFO:Data Receiving.. \n\n")};
    while (1)
    {
        ReadByte();
    }
    CloseHandle(hPort);
    system("echo. &echo. &PAUSE");
    return 0;
}