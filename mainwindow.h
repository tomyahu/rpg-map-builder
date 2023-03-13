#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include "Components/map.h"
#include "Widgets/canvas.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	Map *getMap();

private:
	Map *room_map;
	Canvas *canvas;

	void createMenus();
	QMenu *file_menu;
	QMenu *map_menu;

	void createActions();
	QAction *new_action;
	QAction *open_action;
	QAction *save_as_action;
	QAction *export_action;
	QAction *exit_action;
	QAction *set_tileset_action;


private slots:
	void newFile();
	void openFile();
	void saveAs();
	void exportFile();
	void exit();
	void setTileset();

};
#endif // MAINWINDOW_H
