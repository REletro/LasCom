/**
 * @brief Receiver data using the  the Laser Driver
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

#define synerr() fprintf(stderr, "Serial Communication Data Receiving Program... \n\n USAGE: \tR.EXE /F<Input File Name>/P<COM Port>\n")
FILE *fout = NULL;
int port = 2;
HANDLE hPort;
DCB dcb;
COMMTIMEOUTS to;
int InitComPort()
{
    char COM[10];
    sprintf(COM, "\\\\.\\COM%d", port);
    printf("\nINFO: Creating Port FIle at [%s]\n", COM);
    hPort = CreateFile(COM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hPort == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "\nERROR: Invalid COM Port Handle Value.. \n");
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
void ReceiveData()
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
            printf("\nINFO: Data Receiving Started \n\n");
            if (WaitCommEvent(hPort, &dwCommModemStatus, NULL))
            {
                do
                {
                    if (ReadFile(hPort, &data, 1, &dwBytesTransferred, 0))
                    {
                        fprintf(fout, "%c", data);
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
    system("TITLE Serial Communication Data Receiving TOOL");
    system("COLOR FC");
    int len = 0;
    if (argc == 5)
    {
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
                fout = fopen(argv[2], "w");
            }
            else
            {
                fprintf(stderr, "ERROR: Invalid Opion: %s", argv[1]);
                exiting(-1);
            }

            if (fout == NULL)
            {
                fprintf(stderr, "ERROR: OUTPUT FILE: Can Not open '%s'for Saving Data.\n", argv[2]);
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
                    fprintf(stderr, "ERROR: Invalid Option: %s", argv[3]);
                    fclose(fout);
                    exiting(-1);
                }
                if ((port <= 0) || (port > 256))
                {
                    fprintf(stderr, "ERROR: COM PORT: Invalid COM Port '%s'.\n", argv[3]);
                    fclose(fout);
                    exiting(-1);
                }
                if (InitComPort())
                {
                    ReceiveData();
                }
                else
                {
                    fclose(fout);
                    exiting(-1);
                }
            }
        }
    }
    else
    {
        synerr();
        exiting(-1);
    }
    fclose(fout);
    CloseHandle(hPort);
    exiting(0);
}