#include<iostream>
#include<fstream>
#include<string>

//proprietary to windows
#include<windows.h>
#include<conio.h>

using namespace std;

//inside dlcfg.cfg
string MAX_FPS = "60";
string MAX_WIDTH = "1920";
string MAX_HEIGHT = "1080";
string outputDir = "";    //output path

string mainDir = "";    //youtube-dl path - includes full directory path with end backslash

enum EMenu
{
	DEF, AUDIO, VIDEO, UPDATE
};

void getCurDir()
{
	char* dirBuff = new char[MAX_PATH];
	DWORD handler = GetModuleFileName(NULL, dirBuff, MAX_PATH);
	if(handler == 0)
	{
		cout << "GetModuleFileName has failed";
	}
	mainDir = dirBuff;

	int lastBS = 0;
	for(int i = 0; i < mainDir.length(); i++)
	{
		if(mainDir[i] == '\\'){
			lastBS = i;
		}
	}
		
	mainDir.erase(mainDir.begin() + lastBS + 1, mainDir.end());    //keeps the last backslash
}

void getSettings()
{
	string filePath;
	filePath = mainDir + "dlcfg.cfg";

	string line;
	fstream file(filePath.c_str(), ios::in);
	if(file.is_open())
	{
		for(int i = 0; i < 4; i++)
		{
			getline(file, line);
			switch(i)
			{
				case 0: MAX_FPS = line;
					break;
				case 1: MAX_WIDTH = line;
					break;
				case 2: MAX_HEIGHT = line;
					break;
				case 3: outputDir = line;
					break;
			}
		}
		file.close();
	}
}

void optionsMenu()
{
	getSettings();
	system("cls");

	cout << "Enter desired video FPS: ";
	cin >> MAX_FPS;
	cout << endl;
	cout << "Enter desired video resolution width: ";
	cin >> MAX_WIDTH;
	cout << endl;
	cout << "Enter desired video resolution height: ";
	cin >> MAX_HEIGHT;
	cout << endl;
	cout << "Enter desired video download path: ";
	cin >> outputDir;
	cout << endl;

	string filePath;
	filePath = mainDir + "dlcfg.cfg";

	fstream file(filePath.c_str(), ios::out | ios::trunc);

	if(file.is_open())
	{
		file << MAX_FPS << endl;
		file << MAX_WIDTH << endl;
		file << MAX_HEIGHT << endl;
		file << outputDir;

		file.close();
		cout << endl << "Settings saved." << endl;
	}
}

void openProgram(string argList)
{
	string programPath = mainDir + "youtube-dl.exe";

	HINSTANCE hRet = ShellExecute(
		HWND_DESKTOP, //Parent window
		"open",       //Operation to perform
		programPath.c_str(),       //Path to program
		argList.c_str(),         //Parameters
		NULL,         //Default directory
		SW_MINIMIZE);     //How to open
}

void logURL(string URL)
{
	string writeBlock[10];
	writeBlock[0] = URL;

	string logPath;
	logPath = mainDir + "log.txt";

	fstream logRead(logPath.c_str(), ios::in);
	if(logRead.is_open())
	{
		for(int i = 1; i < 10; i++)
		{
			getline(logRead, writeBlock[i]);
		}
		logRead.close();
	}

	fstream logWrite(logPath.c_str(), ios::out | ios::trunc);
	if(logWrite.is_open())
	{
		for(int i = 0; i < 10; i++)
		{
			logWrite << writeBlock[i] << endl;
		}
		logWrite.close();
	}


}

void ytdlExists()
{
	ifstream f(mainDir + "youtube-dl.exe");
	if(!f.good())
	{
		std::cout << "Error: couldn't find youtube-dl.exe\nPress any key to exit...";
		std::cin.get();
		exit(0);
	}
}

int main(int argc, char** argv)
{
	string argList;

	getCurDir();
	ytdlExists();

	while(1)
	{
		getSettings();

		int selection = 0;
		EMenu menuSelected;
		do
		{
			menuSelected = DEF;
			cout << "Press 'v' for video, 'a' for audio, 'o' for options, 'u' to update, 'esc' to exit: ";

			selection = _getch();
			switch(selection)
			{
				case 'a':
				case 'A': menuSelected = AUDIO;
					break;
				case 'v':
				case 'V': menuSelected = VIDEO;
					break;
				case 'o':
				case 'O': optionsMenu();
					break;
				case 'u':
				case 'U': menuSelected = UPDATE;
					break;
				case 27: exit(0);
					break;
				default: break;
			}

			system("cls");
			if(menuSelected == UPDATE)
			{
				openProgram(" -U");
			}
			else if(menuSelected != DEF)
			{
				break;
			}
		} while(1);

		string URL;
		cout << "Enter URL: ";
		cin >> URL;

		logURL(URL);

		system("cls");
		cout << "Proceding to download...";

		//required format should be: 
		//for video: -f "bestvideo[fps<=60][height<=1080][width<=1920]"+bestaudio/best
		//for audio: -f bestaudio


		switch(menuSelected)
		{
			case VIDEO:
				argList = URL + " -f ";
				argList += "\"bestvideo[fps<=";
				argList += MAX_FPS;
				argList += "][height<=";
				argList += MAX_HEIGHT;
				argList += "][width<=";
				argList += MAX_WIDTH;
				argList += "]\"+bestaudio/best";
				argList += " -o ";
				argList += outputDir;
				argList += "\\%(title)s.%(ext)s";
				break;
			case AUDIO:
				argList = URL + " -f ";
				argList += "bestaudio";
				argList += " -o ";
				argList += outputDir;
				argList += "\\%(title)s.%(ext)s";
				break;
		}

		openProgram(argList);

		system("cls");
	}

}