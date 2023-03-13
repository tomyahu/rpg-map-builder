#ifndef GLOBAL_H
#define GLOBAL_H

#include "mainwindow.h"

class Global
{
public:
	Global();

	static MainWindow* getMainWindow();
	static void setMainWindow( MainWindow *new_main_window );

	static MainWindow *main_window;

private:
};

#endif // GLOBAL_H
