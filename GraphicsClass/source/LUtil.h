#pragma once 

#include "LOpenGL.h"
#include <stdio.h>
#include <string>

//Screen Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

extern std::string OBJ_NAME;


bool InitGL();

void Update();

void Input(unsigned char i_Key, int i_MouseX, int i_MouseY);


void SpecialInput(int i_Key, int i_MouseX, int i_MouseY);
void SpecialUpInput(int i_Key, int i_MouseX, int i_MouseY);

void MouseClicks(int button, int state, int x, int y);
void Render();

void myMouseMove(int x, int y);