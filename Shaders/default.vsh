attribute highp vec4 vertex_coord;
attribute highp vec4 texture_coord;
varying highp vec4 tex_coord;

void main(void)
{
    gl_Position = vertex_coord;
    tex_coord = texture_coord;
}
