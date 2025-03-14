CC = gcc
CFLAGS = -Wall -pthread
EXECUTABLES = MobileUser Backoffice SystemManager AuthRequestManager AuthEngine MonitorEngine

all: $(EXECUTABLES)

MobileUser: MobileUser.c 
	$(CC) $(CFLAGS) MobileUser.c -o MobileUser 

Backoffice: Backoffice.c 
	$(CC) $(CFLAGS) Backoffice.c -o Backoffice    
	
SystemManager: SystemManager.c 
	$(CC) $(CFLAGS) SystemManager.c -o SystemManager 
	
AuthRequestManager: AuthRequestManager.c 
	$(CC) $(CFLAGS) AuthRequestManager.c -o AuthRequestManager

AuthEngine: AuthEngine.c 
	$(CC) $(CFLAGS) AuthEngine.c -o AuthEngine
	
MonitorEngine: MonitorEngine.c 
	$(CC) $(CFLAGS) MonitorEngine.c -o MonitorEngine

clean:
	rm -f $(EXECUTABLES)