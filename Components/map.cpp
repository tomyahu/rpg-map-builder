#include "map.h"
#include <limits>
#include <algorithm>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QErrorMessage>
#include <QPainter>

Map::Map()
{
	tiles = QMap<int, QMap<int, int>>();
	tile_patterns = QMap<QString, int>();
	tileset = nullptr;

	QMap<QString, int> patterns = QMap<QString, int>();
	patterns["11*100*00"] = 0;
	patterns["*1*000000"] = 1;
	patterns["*1100100*"] = 2;
	patterns["100000000"] = 3;
	patterns["001000000"] = 4;
	patterns["*00100*00"] = 5;
	patterns["00*00100*"] = 7;
	patterns["000000100"] = 8;
	patterns["000000001"] = 9;
	patterns["*0010011*"] = 10;
	patterns["000000*1*"] = 11;
	patterns["00*001*11"] = 12;


	QMap<QString, int>::iterator i;
	for( i = patterns.begin(); i != patterns.end(); ++i ) {
		QString pattern = i.key();
		int j;
		QVector<QString> sub_patterns = QVector<QString>();
		QVector<QString>::iterator k;
		sub_patterns.append("");

		for( j = 0; j < pattern.length(); ++j ) {
			if( pattern[j] == '*' ) {
				QVector<QString> aux;
				for( k = sub_patterns.begin(); k != sub_patterns.end(); ++k ) {
					aux.append( *k + '0' );
					aux.append( *k + '1' );
				}
				sub_patterns = aux;

			} else {
				for( k = sub_patterns.begin(); k != sub_patterns.end(); ++k ) {
					k->append( pattern[j] );
				}
			}
		}

		for( k = sub_patterns.begin(); k != sub_patterns.end(); ++k ) {
			tile_patterns[ *k ] = i.value();
		}
	}
}

QJsonDocument Map::exportAsJson()
{
	QJsonObject record_object;
	record_object.insert( "tileset", tileset_path );

	QJsonObject tiles_objects;
	QMap<int, QMap<int, int>>::iterator i;
	QMap<int, int>::iterator j;
	for( i = tiles.begin(); i != tiles.end(); ++i ) {
		QJsonArray tile_row_array;
		for( j = i.value().begin(); j != i.value().end(); ++j ) {
			if( j.value() == 13 ) tile_row_array.append( j.key() );
		}

		tiles_objects.insert( QString::number(i.key()), tile_row_array );
	}
	record_object.insert( "tiles", tiles_objects );

	QJsonDocument document(record_object);
	return document;
}

QImage Map::exportAsImage()
{
	int tile_size = tileset->height() / 3;

	int min_x = std::numeric_limits<float>::max();
	int min_y = std::numeric_limits<float>::max();
	int max_x = std::numeric_limits<float>::min();
	int max_y = std::numeric_limits<float>::min();

	QMap<int, QMap<int, int>>::iterator it_i;
	QMap<int, int>::iterator it_j;
	for( it_i = tiles.begin(); it_i != tiles.end(); ++it_i ) {
		min_x = std::min( min_x, it_i.key() );
		max_x = std::max( max_x, it_i.key() );
		for( it_j = it_i.value().begin(); it_j != it_i.value().end(); ++it_j ) {
			min_y = std::min( min_y, it_j.key() );
			max_y = std::max( max_y, it_j.key() );
		}
	}

	min_x -= 1;
	min_y -= 1;
	max_x += 2;
	max_y += 2;

	QImage *image = new QImage( (max_x - min_x) * tile_size, (max_y - min_y) * tile_size, QImage::Format_RGBA64);
	QPainter *image_painter = new QPainter(image);

	int i, j;
	for( i = min_x; i < max_x; ++i ) {
		for( j = min_y; j < max_y; ++j ) {
			QPair<QPoint, QPoint> points;
			if( tiles.contains(i) && tiles[i].contains(j) ) points = this->getRect( tiles[i][j] );
			else points = this->getRect( 6 );

			image_painter->drawImage(
						QPoint( (i - min_x) * tile_size, ( j - min_y ) * tile_size),
						QImage(tileset_path),
						QRect( points.first, points.second )
			);
		}
	}
	image_painter->end();

	return *image;
}

void Map::addTile(int x, int y)
{
	if( ! tiles.contains(x) ) tiles.insert(x, QMap<int, int>());

	tiles[x][y] = 13;

	int i, j;
	for( i = y-1; i <= y+1; i++ ) {
		for( j = x-1; j <= x+1; j++ ) {
			if( j == x && i == y ) continue;
			updateTile(j, i);
		}
	}
}

void Map::removeTile(int x, int y)
{
	if( ! tiles.contains(x) ) return;

	tiles[x].remove(y);
	if( tiles[x].empty() ) tiles.remove(x);

	int i, j;
	for( i = y-1; i <= y+1; i++ ) {
		for( j = x-1; j <= x+1; j++ ) {
			updateTile(j, i);
		}
	}
}

void Map::clearTiles()
{
	tiles.clear();
}

bool Map::isTilesetSet()
{
	return tileset != nullptr;
}

QImage *Map::getTileset()
{
	return tileset;
}

QMap<int, QMap<int, int> > *Map::getTiles()
{
	return &tiles;
}

void Map::setTileset(QString new_tileset_path)
{
	tileset_path = new_tileset_path;
	QImage *tileset_aux = new QImage( tileset_path );
	if( ( tileset_aux->height()*5 != tileset_aux->width()*3 ) ) return ( new QErrorMessage() )->showMessage( "Invalid Tileset" );

	tileset = tileset_aux;
}

QPair<QPoint, QPoint> Map::getRect(int i )
{
	int tile_size = tileset->height() / 3;

	return Map::getRect(i, tile_size);
}

QPair<QPoint, QPoint> Map::getRect(int i, int tile_size)
{
	int t_row = i / 5;
	int t_col = i % 5;

	return QPair<QPoint, QPoint>(
				QPoint(t_col * tile_size, t_row * tile_size),
				QPoint(t_col * tile_size + tile_size, t_row * tile_size + tile_size)
				);
}

void Map::updateTile(int x, int y)
{
	if( ! tiles.contains(x) ) tiles.insert(x, QMap<int, int>());
	if( tiles[x][y] == 13 ) return;

	QString pattern = "";
	int i, j;
	for( i = y-1; i <= y+1; i++ ) {
		for( j = x-1; j <= x+1; j++ ) {
			if( tiles.contains(j) && tiles[j].contains(i) && tiles[j][i] == 13 ){
				pattern.append('1');

			} else {
				pattern.append('0');
			}
		}
	}

	if( tile_patterns.contains(pattern) ) tiles[x][y] = tile_patterns[pattern];
	if( pattern == "000000000" ) tiles[x].remove(y);
}
