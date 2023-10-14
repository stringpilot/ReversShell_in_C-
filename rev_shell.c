#include <windows.h>
#include <stdio.h>

int main() {
    // Define the PowerShell command
    const char* command = "powershell -Command \"Write-Host 'Hello from PowerShell'\"";

    // Create pipes for standard output and standard error
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE stdOut, stdErr;
    CreatePipe(&stdOut, &stdOut, &sa, 0);
    CreatePipe(&stdErr, &stdErr, &sa, 0);

    // Create the process
    PROCESS_INFORMATION procInfo;
    STARTUPINFO startInfo;
    ZeroMemory(&startInfo, sizeof(STARTUPINFO));
    startInfo.cb = sizeof(STARTUPINFO);
    startInfo.hStdOutput = stdOut;
    startInfo.hStdError = stdErr;
    startInfo.dwFlags |= STARTF_USESTDHANDLES;

    if (CreateProcess(NULL, (LPSTR)command, NULL, NULL, TRUE, 0, NULL, NULL, &startInfo, &procInfo)) {
        // Close unused pipe ends
        CloseHandle(stdOut);
        CloseHandle(stdErr);

        // Read and print the output
        char buffer[4096];
        DWORD bytesRead;

        while (ReadFile(stdOut, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
            fwrite(buffer, 1, bytesRead, stdout);
        }

        // Wait for the process to finish
        WaitForSingleObject(procInfo.hProcess, INFINITE);
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
    } else {
        // Handle the failure to create the process
        printf("Failed to execute PowerShell command. Error %d\n", GetLastError());
        return 1;
    }

    return 0;
}
