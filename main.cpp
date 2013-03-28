
#define GL_GLEXT_PROTOTYPES 1
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <math.h>

GLuint shader;
GLuint vert, frag;
GLuint VAO, VBO_pos, VBO_col;
GLint pos_attrib, col_attrib;
GLint trans_uniform;

char * vert_source =
        "attribute vec3 in_pos;\
         attribute vec3 rgb_color;\
         varying vec4 out_col;\
         uniform mat3 trans;\
         void main(void) {\
         gl_Position = vec4( trans * in_pos, 1.0 );\
         out_col = vec4(rgb_color, 1.0);}";

char * frag_source =
        "varying vec4 out_col;\
         void main(void) {\
         gl_FragColor = out_col; }";

float mtr[9] = {  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1};

static void _Timer(int value)
{
    static float angle = 0;
    angle += 0.1;

    mtr[0] = cos( angle );
    mtr[8] = cos( angle );
    mtr[2] = -sin( angle );
    mtr[6] = sin( angle );

  /* send redisplay event */
  glutPostRedisplay();

  /* call this function again in 10 milliseconds */
  glutTimerFunc(30, _Timer, 0);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glUniformMatrix3fv( trans_uniform, 1, 1, mtr );

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glEnableVertexAttribArray(pos_attrib);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_col);
    glEnableVertexAttribArray(col_attrib);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glFlush();
}

void check_shader( GLint shader, GLenum pname=GL_COMPILE_STATUS )
    {
        int result;
        glGetShaderiv( shader, pname, &result );

        fprintf( stderr, "result %d\n", result );
        if ( result == GL_FALSE )
        {
            int loglen;
            glGetProgramiv( shader, GL_INFO_LOG_LENGTH, &loglen );
            if( loglen )
            {
                char *chlog = new char[loglen];
                glGetShaderInfoLog( shader, loglen, &loglen, chlog );
                fprintf( stderr, "shader compile error: %s", chlog );
                delete chlog;
            }
            exit(1);
        }
    }

void initshader() {

    vert = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vert, 1, (const GLchar**)&vert_source, 0 );
    glCompileShader(vert);
    check_shader(vert);

    frag = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( frag, 1, (const GLchar**)&frag_source, 0 );
    glCompileShader(frag);
    check_shader(frag);

    shader = glCreateProgram();

    glAttachShader( shader, vert );
    glAttachShader( shader, frag );

    glLinkProgram( shader );

    trans_uniform = glGetUniformLocation( shader, "trans" );
}

void init_object() {

    float array_pos[] = {-0.5, -0.5,   0.5, -0.5,   0.0, 0.5};
    float array_col[] = {1, 0, 0,    0, 1, 0,    0, 0, 1};

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO_pos);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, array_pos, GL_STATIC_DRAW);

    pos_attrib = glGetAttribLocation(shader, "in_pos");
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, 0, 0, 0);

    glGenBuffers(1, &VBO_col);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, array_col, GL_STATIC_DRAW);

    col_attrib = glGetAttribLocation(shader, "rgb_color");
    glVertexAttribPointer(col_attrib, 3, GL_FLOAT, 0, 0, 0);
}

void Initialize() {
    glClearColor(0.8, 0.9, 0.8, 0.51);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initshader();

    init_object();
}

int main(int argc, char ** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 200);
    glutCreateWindow("Our first GLUT application!");
    glutDisplayFunc(Display);
    Initialize();
    glutTimerFunc(10, _Timer, 0);
    glutMainLoop();

    return 0;
}
