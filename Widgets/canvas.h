#ifndef CANVAS_H
#define CANVAS_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QOpenGLTexture>

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	Canvas();

	void cls();
	void rect(float x0, float y0, float x1, float y1, QColor color);
	void spr(int i, float x, float y);

public slots:
	void draw();

protected:
	// initializeGl
	// initializes openGL in the canvas
	virtual void initializeGL() override;

	// resizeGL
	// is called when the canvas is resized (probably because of the user resizing the application)
	virtual void resizeGL(int width, int height) override;

	// paintGL
	// draws the assets on the widget
	virtual void paintGL() override;

	// mousePressEvent
	// function called when the mouse is pressed
	virtual void mousePressEvent(QMouseEvent *event) override;

	// mouseReleaseEvent
	// function called when the mouse is released
	virtual void mouseReleaseEvent(QMouseEvent *event) override;

	// keyPressEvent
	// function called when a keyboard key is pressed
	virtual void keyPressEvent(QKeyEvent *event) override;

	// keyReleaseEvent
	// function called when a keyboard key is released
	virtual void keyReleaseEvent(QKeyEvent *event) override;


private:
	QSet<Qt::MouseButton> *buttons_pressed;
	QSet<int> *keys_pressed;
	int tile_size;
	float offset_x;
	float offset_y;
	float offset_v;

	// vertex_buffer
	// the buffer with the vertices to send to the gpu
	QOpenGLBuffer *vertex_buffer;

	// default_shader
	// the default shader of the canvas to draw entities
	QOpenGLShaderProgram *default_shader;

	QOpenGLTexture *texture;

	// initDefaultShader
	// initializes the default shader
	void initDefaultShader();

	void drawSelectedTile();
	void drawTiles();

	float xToCanvas(float x);
	float yToCanvas(float y);

};

#endif // CANVAS_H
