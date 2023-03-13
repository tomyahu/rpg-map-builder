#include "canvas.h"
#include <QTimer>
#include <QGraphicsSceneMouseEvent>
#include "global.h"
#include <cmath>

struct VertexData
{
	GLfloat position[2];
	GLfloat texCoord[2];
};

Canvas::Canvas()
{
	tile_size = 32;
	offset_x = 0;
	offset_y = 0;
	offset_v = 10;

	buttons_pressed = new QSet<Qt::MouseButton>();
	keys_pressed = new QSet<int>();

	this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

	QTimer *timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &Canvas::draw);

	timer->start(1.0f);
}

void Canvas::cls()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Canvas::rect(float x0, float y0, float x1, float y1, QColor color)
{
	x0 = xToCanvas( x0 );
	y0 = yToCanvas( y0 );
	x1 = xToCanvas( x1 );
	y1 = yToCanvas( y1 );

	glDepthMask(GL_FALSE);

	VertexData vertices[4] = {
		{{x1, y0},{1.0f, 0.0f}},
		{{x1, y1},{1.0f, 1.0f}},
		{{x0, y1},{0.0f, 1.0f}},
		{{x0, y0},{0.0f, 0.0f}}
	};
	glColor4f( color.redF(), color.greenF(), color.blueF(), color.alphaF() );

	const int v_position = this->default_shader->attributeLocation("vertex_coord");
	const int tex_position = this->default_shader->attributeLocation("texture_coord");

	this->vertex_buffer->allocate(vertices, 4*sizeof(VertexData));

	this->default_shader->enableAttributeArray(v_position);
	this->default_shader->setAttributeBuffer(v_position, GL_FLOAT, 0, 2, sizeof(VertexData));

	this->default_shader->enableAttributeArray(tex_position);
	this->default_shader->setAttributeBuffer(tex_position, GL_FLOAT, 2*sizeof(GLfloat), 2, sizeof (VertexData));

	glDrawArrays(GL_QUADS, 0, 4);
	glDepthMask(GL_TRUE);
}

void Canvas::spr(int i, float x, float y)
{
	float t_row = ( i / 5 );
	float t_col = ( i % 5 );

	float x0 = xToCanvas( x );
	float y0 = yToCanvas( y );

	float x1 = xToCanvas( x + tile_size );
	float y1 = yToCanvas( y + tile_size );

	glEnable(GL_TEXTURE_2D);
	this->default_shader->bind();

	VertexData vertices[4] = {
		{{x1, y0},{(t_col + 1) / 5.0f,		t_row / 3.0f		}},
		{{x1, y1},{(t_col + 1) / 5.0f,		(t_row + 1) / 3.0f	}},
		{{x0, y1},{t_col / 5.0f,			(t_row + 1) / 3.0f	}},
		{{x0, y0},{t_col / 5.0f,			t_row / 3.0f		}}
	};

	const int v_position = this->default_shader->attributeLocation("vertex_coord");
	const int tex_position = this->default_shader->attributeLocation("texture_coord");

	this->vertex_buffer->allocate(vertices, 4*sizeof(VertexData));

	this->default_shader->enableAttributeArray(v_position);
	this->default_shader->setAttributeBuffer(v_position, GL_FLOAT, 0, 2, sizeof(VertexData));

	this->default_shader->enableAttributeArray(tex_position);
	this->default_shader->setAttributeBuffer(tex_position, GL_FLOAT, 2*sizeof(GLfloat), 2, sizeof (VertexData));

	texture->bind();

	glDrawArrays(GL_QUADS, 0, 4);

	this->default_shader->release();
	glDisable(GL_TEXTURE_2D);
}

void Canvas::draw()
{
	QPoint cursor_pos = mapFromGlobal( QCursor::pos() );
	int x = (int)( cursor_pos.x() + offset_x ) / tile_size;
	int y = (int)( cursor_pos.y() + offset_y ) / tile_size;

	Map *map = Global::getMainWindow()->getMap();
	if( buttons_pressed->contains( Qt::LeftButton ) )
		map->addTile( x, y );
	if( buttons_pressed->contains( Qt::RightButton ) )
		map->removeTile( x, y );

	if( keys_pressed->contains( Qt::Key_A ) ) offset_x -= offset_v;
	if( keys_pressed->contains( Qt::Key_D ) ) offset_x += offset_v;
	if( keys_pressed->contains( Qt::Key_W ) ) offset_y -= offset_v;
	if( keys_pressed->contains( Qt::Key_S ) ) offset_y += offset_v;

	update();
}

void Canvas::initializeGL()
{
	initializeOpenGLFunctions();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	this->resizeGL( this->width(), this->height() );

	this->vertex_buffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	Q_ASSERT(this->vertex_buffer->create());
	Q_ASSERT(this->vertex_buffer->bind());

	initDefaultShader();
}

void Canvas::resizeGL(int width, int height)
{
	// Sets viewport
	glViewport(0,0,width,height);
}

void Canvas::paintGL()
{
	cls();
	drawTiles();
	drawSelectedTile();

}

void Canvas::mousePressEvent(QMouseEvent *event)
{
	buttons_pressed->insert( event->button() );
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
	keys_pressed->insert( event->key() );
}

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
	keys_pressed->remove( event->key() );
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
	buttons_pressed->remove( event->button() );
}

void Canvas::initDefaultShader()
{
	this->default_shader = new QOpenGLShaderProgram(this);
	this->default_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/Shaders/default.vsh");
	this->default_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/Shaders/default.fsh");

	Q_ASSERT(this->default_shader->link());
}

void Canvas::drawSelectedTile()
{
	QPoint cursor_pos = mapFromGlobal( QCursor::pos() );
	float x0 = cursor_pos.x() + offset_x;
	float y0 = cursor_pos.y() + offset_y;
	x0 = floor(x0 / tile_size) * tile_size;
	y0 = floor(y0 / tile_size) * tile_size;


	float x1 = x0 + tile_size;
	float y1 = y0 + tile_size;

	rect( x0, y0, x1, y1, QColor(255,0,0,125) );
}

void Canvas::drawTiles()
{
	Map *map = Global::getMainWindow()->getMap();
	if( ! map->isTilesetSet() ) return;

	QImage tileset = *map->getTileset();
	texture = new QOpenGLTexture( tileset );

	QMap<int, QMap<int, int>> *tiles = map->getTiles();
	QMap<int, QMap<int, int>>::iterator i;
	QMap<int, int>::iterator j;
	float x0, y0;
	for( i = tiles->begin(); i != tiles->end(); ++i ) {
		x0 = i.key() * tile_size;
		for( j = i.value().begin(); j != i.value().end(); ++j ) {
			y0 = j.key() * tile_size;
			spr( j.value(), x0, y0 );
		}
	}
}

float Canvas::xToCanvas(float x)
{
	return ( x - offset_x ) / width() * 2 - 1;
}

float Canvas::yToCanvas(float y)
{
	return 1 - ( y - offset_y ) / height() * 2;
}
