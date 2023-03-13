#include "global.h"

Global::Global()
{

}

MainWindow *Global::getMainWindow()
{
	return main_window;
}

void Global::setMainWindow(MainWindow *new_main_window)
{
	Global::main_window = new_main_window;
}

MainWindow *Global::main_window;
