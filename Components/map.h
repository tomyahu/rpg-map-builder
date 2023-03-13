#ifndef MAP_H
#define MAP_H

#include <QJsonDocument>
#include <QImage>
#include <QPair>
#include <QPoint>

class Map
{
public:
	Map();

	QJsonDocument exportAsJson();
	QImage exportAsImage();
	void addTile(int x, int y);
	void removeTile(int x, int y);
	void clearTiles();
	QPair<QPoint, QPoint> getRect(int i);

	// gettes
	bool isTilesetSet();
	QImage *getTileset();
	QMap<int, QMap<int, int>> *getTiles();

	// setters
	void setTileset(QString tileset_path);

	// static
	static QPair<QPoint, QPoint> getRect(int i, int tile_size);

private:
	QImage *tileset;
	QString tileset_path;
	QMap<int, QMap<int, int>> tiles;
	QMap<QString, int> tile_patterns;

	void updateTile(int x, int y);
};

#endif // MAP_H
