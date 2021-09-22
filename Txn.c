/**
 * @brief Transmit data from the Laptop with the Laser Driver
 *          Prerequisites: This code is made only for the windows OS using win32 API.
 *          1. Laser Driver (w/ RS-232) connected to USB port of the Laptop.
 *          2. Both Tx and Rx Device on and 
 *          3. Tx laser is pointing at Rx-PhotoDetector
 *          4. the Rx- reception should Initiate before Tx transmits data
 * @author Rajesh Kumar
 * 
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define synerr() fprintf(stderr, "Serial Communication Data Sending Program... \n\n USAGE: \tS.EXE /F<Input File Name>/P<COM Port>\n")

int fin = -1;
int port = 2;
HANDLE hPort;
DCB dcb;
COMMTIMEOUTS to;
char Byte;


int InitComPort()
{
    char COM[10];
    sprintf(COM, "\\\\.\\COM%d", port);
    printf("\nINFO : Creating Port File at [%s ]\n", COM);
    hPort = CreateFile(COM, GENERIC_READ GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hPort == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "\nERROR: Invalid COM Port Handle Value... \n");
        return 0;
    }
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    deb.StopBits = ONESTOPBIT;
    if (!SetCommState(hPort, &dcb))
    {
        fprintf(stderr, "\nERROR: DCB Initialization Error... \n");
        return 0;
    }
    to.ReadIntervalTimeout = 50;
    to.ReadTotalTimeoutConstant = 50;
    to.READTotalTImeoutMultiplier = 10;
    to.WriteTotalTimeoutConstant = 50;
    to.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hPort, &to))
    {
        fprintf(stderr, "\nERROR: COMMTIMEOUTS Initialization Error... \n");
        return 0;
    }
    return 1;
}

int SendByte(char *data)
{
    DWORD bytesWritten;
    if (!GetCommState(hPort, &dcb))
    {
        fprintf(stderr, "\nERROR: Couldn't Get the State of the COM-Port... \n");
        return 0;
    }
    int retVal = WriteFile(hPort, data, 1, &bytesWritten, NULL);
    return retVal;
}
void exiting(int ret)
{
    system("ECHO. &PAUSE");
    exit(ret);
}

/*--------------------------------------------------------------------*/
/** @brief : Main Program
*/
int main(int argc, char *argv[])
{
    system("TITLE Serial Communication Data Sending Tool");
    system("COLOR F9");
    if (argc == 5)
    {
        int len = 0;
        len = strlen(argv[1]);
        if (len != 2)
        {
            synerr();
            exiting(-1);
        }
        else
        {
            if (((argv[1][1] == 'f') || (argv[1][1] == 'F')) && ((argv[1][0] == '/') || (argv[1][0] == '-')))
            {
                fin = open(argv[2], O_RDONLY);
            }
            else
            {
                fprintf(stderr, "ERROR: Invalid Oprion : %s", argv[1]);
                exiting(-1)
            }
            if (fin == -1)
            {
                fprintf(stderr, "ERROR: INPUT FILE: Can Not Open '%s' forSending Data.\n", argv[2]);
                exiting(-1);
            }
            len = strlen(argv[3]);
            if (len != 2)
            {
                synerr();
                exiting(-1);
            }
            else
            {
                if (((argv[3][1] == 'p') || (argv[3][1] == 'P')) && ((argv[3][0] == '/') || (argv[3][0] == '-')))
                {
                    port = atoi(argv[4]);
                }
                else
                {
                    fprintf(stderr, "ERROR: Invalid Oprion : %s", argv[3]);
                    close(fin);
                    exiting(-1);
                }
                if ((port <= 0) || (port > 256))
                {
                    fprintf(stderr, "ERROR: COM PORT: Invalid COM Port '%s'. \n", argv[3]);
                    close(fin);
                    exiting(-1);
                    if (InitComPort())
                    {
                        printf("\nINFO: Writing to Serial Port...\n\n");
                        while (read(fin, &Byte, 1) == 1)
                        {
                            if (SendByte(&Byte))
                                printf("%c", Byte);
                        }
                    }
                    else
                    {
                        close(fin);
                        exiting(-1);
                    }
                }
            }
        }
    }
    else
    {
        synerr();
        exiting(-1);
    }
    close(fin);
    CloseHandle(hPort);
    exiting(0);
}