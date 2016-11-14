#ifndef DEBUG
#define DEBUG
#endif

#include <gl/glut.h>
#include <stdio.h>
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment(lib, "fmod_vc.lib")
#include "OpenFileDialog.h"

#define EXIT_SUCCESS 0

using namespace FMOD;

int	gWidth = 300, gHeight = 300;
int	gMenuChoice;

FMOD::System     *gFmodSystem;
FMOD::Sound      *gFmodSound;
FMOD::Channel    *gFmodChannel = 0;
FMOD_RESULT       gFmodResult;
unsigned int      fmod_version;

enum MenuItem {
	OPEN_MUSIC_FILE,
	STOP_MUSIC,
	EXIT,
	NO_ACTION
};


void displayFunc(void);
void reshapeFunc(int w, int h);
void addMenuEntry(void);
void selectMenuFunc(int entryID);
const char* getMusicFilePath(void);
void FMOD_ERRCHECK(FMOD_RESULT gFmodResult);
void FMOD_playSound(const char* filePath);
void FMOD_releaseSound();
void FMOD_shutdownSystem();

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(gWidth, gHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("JUDY (DJ your day)");

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);

	GLint MyMainMenuID = glutCreateMenu(selectMenuFunc);
	addMenuEntry();
	gMenuChoice = NO_ACTION;

	glutMainLoop();

	return EXIT_SUCCESS;
}

void addMenuEntry(void) {
	glutAddMenuEntry("음악파일열기", OPEN_MUSIC_FILE);
	glutAddMenuEntry("음악 정지", STOP_MUSIC);
	glutAddMenuEntry("Exit", EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void selectMenuFunc(int entryID) {
	gMenuChoice = entryID;

	switch (gMenuChoice) {
	case OPEN_MUSIC_FILE:
		FMOD_playSound(getMusicFilePath());
		break;
	case STOP_MUSIC:
		FMOD_releaseSound();
	case EXIT:
		FMOD_releaseSound();
		FMOD_shutdownSystem();
		exit(EXIT_SUCCESS);
		break;
	default:
		break;
	}
}

void displayFunc(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(255, 255, 255);

	glViewport(0, 0, gWidth, gHeight); {
		glutWireCube(0.3);
	}

	glFlush();
}

void reshapeFunc(int w, int h) {
	gWidth = w;
	gHeight = h;
}

const char* getMusicFilePath(void) {
	char *filePath = (char *)calloc(256, sizeof(char));

	OpenFileDialog* fileDialog = new OpenFileDialog();
	fileDialog->InitialDir = _T("C:\\Users\\");
	fileDialog->Title = _T("Open Music File");

	// 파일 경로 복사 TCHAR*, char* 조사 필요
	// _tcscpy_s(arrFilePath, fileDialog->FileName);
	// strcat(filePath, fileDialog->FileName);
	WideCharToMultiByte(CP_ACP, 0, fileDialog->FileName, 256, filePath, 256, NULL, NULL);

#ifdef DEBUG
	if (fileDialog->ShowDialog()) {
		MessageBox(0, fileDialog->FileName, _T("선택된 음악 파일"),
			MB_OK | MB_ICONINFORMATION);
	}

	printf("선택된 음악 파일: %s\n", filePath);
#endif

	delete fileDialog;
	//return filePath;
	return "test.mp3";
}

void FMOD_ERRCHECK(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		printf("fmod error\n");
	}
}

void FMOD_playSound(const char* filePath) {
	/*
	Create a System object and initialize.
	*/
	gFmodResult = FMOD::System_Create(&gFmodSystem);
	FMOD_ERRCHECK(gFmodResult);

	gFmodResult = gFmodSystem->getVersion(&fmod_version);
	FMOD_ERRCHECK(gFmodResult);

	if (fmod_version < FMOD_VERSION)
	{
		printf("FMOD lib fmod_version %08x doesn't match header fmod_version %08x", fmod_version, FMOD_VERSION);
	}

	gFmodResult = gFmodSystem->init(32, FMOD_INIT_NORMAL, NULL);
	FMOD_ERRCHECK(gFmodResult);

	gFmodResult = gFmodSystem->createStream(filePath, FMOD_LOOP_NORMAL | FMOD_2D, 0, &gFmodSound);
	FMOD_ERRCHECK(gFmodResult);

	/*
	Play the sound.
	*/
	gFmodResult = gFmodSystem->playSound(gFmodSound, 0, false, &gFmodChannel);
	FMOD_ERRCHECK(gFmodResult);
}

void FMOD_releaseSound() {
	/*
	Shut down
	*/
	gFmodResult = gFmodSound->release();  /* Release the parent, not the sound that was retrieved with getSubSound. */
	FMOD_ERRCHECK(gFmodResult);
}

void FMOD_shutdownSystem() {
	gFmodResult = gFmodSystem->close();
	FMOD_ERRCHECK(gFmodResult);
	gFmodResult = gFmodSystem->release();
	FMOD_ERRCHECK(gFmodResult);
}