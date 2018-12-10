#include "colorwidgetshaders.h"
#include "glhelpers.h"

ColorProgram HUE_PROGRAM;
ColorProgram HSV_SATURATION_PROGRAM;
ColorProgram VALUE_PROGRAM;

ColorProgram HSL_SATURATION_PROGRAM;
ColorProgram LIGHTNESS_PROGRAM;

ColorProgram RED_PROGRAM;
ColorProgram GREEN_PROGRAM;
ColorProgram BLUE_PROGRAM;

ColorProgram ALPHA_PROGRAM;

PlainColorProgram PLAIN_PROGRAM;

GLuint COLOR_WIDGET_VAO;
GLuint COLOR_WIDGET_VBO;

void iniColorProgram(QGL33c* gl,
                     ColorProgram& program,
                     const std::string& vShaderPath,
                     const std::string& fShaderPath) {
    iniProgram(gl, program.fID, vShaderPath, fShaderPath);
    gl->glUseProgram(program.fID);
    assertNoGlErrors();
    program.fHSVColorLoc = gl->glGetUniformLocation(
                program.fID, "HSVColor");
    assertNoGlErrors();
    program.fRGBColorLoc = gl->glGetUniformLocation(
                program.fID, "RGBColor");
    assertNoGlErrors();
    program.fHSLColorLoc = gl->glGetUniformLocation(
                program.fID, "HSLColor");
    assertNoGlErrors();
    program.fCurrentValueLoc = gl->glGetUniformLocation(
                program.fID, "currentValue");
    assertNoGlErrors();
    assert(program.fCurrentValueLoc >= 0);
    program.fHandleWidthLoc = gl->glGetUniformLocation(
                program.fID, "handleWidth");
    assertNoGlErrors();
    assert(program.fHandleWidthLoc >= 0);
    program.fLightHandleLoc = gl->glGetUniformLocation(
                program.fID, "lightHandle");
    assertNoGlErrors();
    assert(program.fLightHandleLoc >= 0);
    program.fMeshSizeLoc = gl->glGetUniformLocation(
                program.fID, "meshSize");
    assertNoGlErrors();
}

void iniPlainColorProgram(QGL33c *gl, const std::string& colorShadersPath) {
    iniProgram(gl, PLAIN_PROGRAM.fID, GL_PLAIN_VERT,
               colorShadersPath + "plain.frag");
    PLAIN_PROGRAM.fRGBAColorLoc = gl->glGetUniformLocation(
                PLAIN_PROGRAM.fID, "RGBAColor");
    assertNoGlErrors();
    assert(PLAIN_PROGRAM.fRGBAColorLoc >= 0);

    PLAIN_PROGRAM.fMeshSizeLoc = gl->glGetUniformLocation(
                PLAIN_PROGRAM.fID, "meshSize");
    assertNoGlErrors();
    assert(PLAIN_PROGRAM.fMeshSizeLoc >= 0);
}

void iniColorPrograms(QGL33c *gl) {
    std::string colorShadersPath =
            "/home/ailuropoda/Dev/AniVect/src/app/GUI/"
            "ColorWidgets/ColorWidgetShaders/";
    iniColorProgram(gl, HUE_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "hue.frag");
    iniColorProgram(gl, HSV_SATURATION_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "hsv_saturation.frag");
    iniColorProgram(gl, VALUE_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "value.frag");
    iniColorProgram(gl, HSL_SATURATION_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "hsl_saturation.frag");
    iniColorProgram(gl, LIGHTNESS_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "lightness.frag");
    iniColorProgram(gl, RED_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "red.frag");
    iniColorProgram(gl, GREEN_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "green.frag");
    iniColorProgram(gl, BLUE_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "blue.frag");

    iniColorProgram(gl, ALPHA_PROGRAM, GL_PLAIN_VERT,
                    colorShadersPath + "alpha.frag");

    iniPlainColorProgram(gl, colorShadersPath);
}
