#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "global.h"
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
	Global::setMainWindow(this);

	canvas = new Canvas();
	setCentralWidget( canvas );

	createActions();
	createMenus();

	room_map = new Map();

	setMinimumSize( 160, 160 );
	resize( 800, 600 );
}

MainWindow::~MainWindow()
{

}

Map *MainWindow::getMap()
{
	return room_map;
}

void MainWindow::createMenus()
{
	file_menu = menuBar()->addMenu( tr("&File") );
	file_menu->addAction( new_action );
	file_menu->addAction( open_action );
	file_menu->addSeparator();
	file_menu->addAction( save_as_action );
	file_menu->addAction( export_action );
	file_menu->addSeparator();
	file_menu->addAction( exit_action );

	map_menu = menuBar()->addMenu( tr("&Map") );
	map_menu->addAction( set_tileset_action );
}

void MainWindow::createActions()
{
	new_action = new QAction( tr("&New"), this );
	new_action->setShortcuts( QKeySequence::New );
	new_action->setStatusTip("Creates a new map");
	connect( new_action, &QAction::triggered, this, &MainWindow::newFile );

	open_action = new QAction( tr("&Open"), this );
	open_action->setShortcuts( QKeySequence::Open );
	open_action->setStatusTip("Opens a map file");
	connect( open_action, &QAction::triggered, this, &MainWindow::openFile );

	save_as_action = new QAction( tr("&Save As"), this );
	save_as_action->setShortcuts( QKeySequence::SaveAs );
	save_as_action->setStatusTip("Saves the map");
	connect( save_as_action, &QAction::triggered, this, &MainWindow::saveAs );

	export_action = new QAction( tr("&Export"), this );
	export_action->setShortcut( QKeySequence( tr("Ctrl+E") ) );
	export_action->setStatusTip("Exports the map as an image");
	connect( export_action, &QAction::triggered, this, &MainWindow::exportFile );

	exit_action = new QAction( tr("&Exit"), this );
	exit_action->setShortcuts( QKeySequence::Quit );
	connect( exit_action, &QAction::triggered, this, &MainWindow::exit );

	set_tileset_action = new QAction( tr("&Set Tileset"), this );
	set_tileset_action->setShortcut( QKeySequence( tr("Ctrl+T") ) );
	set_tileset_action->setStatusTip("Sets the map tileset");
	connect( set_tileset_action, &QAction::triggered, this, &MainWindow::setTileset );
}

void MainWindow::newFile()
{
	room_map = new Map();
	qDebug() << room_map->exportAsJson().toJson();
}

void MainWindow::openFile()
{
	QString filename = QFileDialog::getOpenFileName( this, "Select a map file", "", tr("JSON (*.json)") );
	if( ! filename.isEmpty() ) {
		QFile file( filename );
		if ( ! file.open( QIODevice::ReadOnly ) ) {
			QMessageBox::information( this, tr("Unable to open file"), file.errorString() );
			return;
		}

		QByteArray map_data = file.readAll();
		file.close();

		QJsonDocument map_json = QJsonDocument::fromJson( map_data );
		room_map->setTileset( map_json["tileset"].toString() );
		room_map->clearTiles();

		QJsonObject tiles = map_json["tiles"].toObject();
		QJsonObject::iterator i;
		QJsonArray::iterator j;
		for( i = tiles.begin(); i != tiles.end(); ++i ) {
			QJsonArray row = i.value().toArray();
			for( j = row.begin(); j != row.end(); ++j ) {
				room_map->addTile( i.key().toInt(), (*j).toInt() );
			}
		}
	}
}

void MainWindow::saveAs()
{
	QString filename = QFileDialog::getSaveFileName( this, "Saves the map", "", tr("JSON (*.json)") );
	if( ! filename.isEmpty() ) {
		QFile file( filename );
		if ( ! file.open( QIODevice::WriteOnly ) ) {
			QMessageBox::information( this, tr("Unable to open file"), file.errorString() );
			return;
		}

		file.write( room_map->exportAsJson().toJson() );
		file.close();
	}
}

void MainWindow::exportFile()
{
	QString filename = QFileDialog::getSaveFileName(this,
			tr("Export Map as Image"), "",
			tr("PNG File (*.png)"));

	if( ! filename.isEmpty() ) {
		QFile file( filename );
		if ( ! file.open( QIODevice::WriteOnly ) ) {
			QMessageBox::information( this, tr("Unable to open file"), file.errorString() );
			return;
		}
		file.close();

		room_map->exportAsImage().save( filename, "png" );
	}
}

void MainWindow::exit()
{
	qApp->exit();
}

void MainWindow::setTileset()
{
	QString tileset = QFileDialog::getOpenFileName( this, "Select a Tileset Image", "", tr("PNG (*.png)") );

	if( ! tileset.isNull() )
		room_map->setTileset( tileset );
}

