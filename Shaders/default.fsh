uniform sampler2D qt_Texture0;
varying highp vec4 tex_coord;

void main(void)
{
    gl_FragColor = texture2D(qt_Texture0, tex_coord.xy);
}
