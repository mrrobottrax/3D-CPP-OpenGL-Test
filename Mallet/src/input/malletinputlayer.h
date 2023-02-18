#pragma once

#include <managers.h>

void SetupInputCallbacks(GLFWwindow*);
//void AddInputCommands();

void MalletKeyCallback(GLFWwindow*, int, int, int, int);
void MalletMouseCallback(GLFWwindow*, int, int, int);
void MalletMousePosCallback(GLFWwindow*, double, double);

//// INPUT COMMANDS ~~~~~~~~~~~~~~~~~~~
//#define MAX_BUTTONS_MALLET 1
//enum ButtonCodeMallet
//{
//	Pan,
//};
//
//extern Button mallet_buttons[MAX_BUTTONS_MALLET];