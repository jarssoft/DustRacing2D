// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef SHADERS30_H
#define SHADERS30_H

static const char * carVsh =
"#version 130\n"
""
"in      vec3  inVertex;\n"
"in      vec3  inNormal;\n"
"in      vec2  inTexCoord;\n"
"in      vec4  inColor;\n"
"uniform vec4  scale;\n"
"uniform vec4  color;\n"
"uniform mat4  vp;\n"
"uniform mat4  model;\n"
"uniform vec4  dd;\n"
"uniform vec4  dc;\n"
"uniform vec4  ac;\n"
"out     vec4  ddRotated;\n"
"out     vec2  texCoord0;\n"
"out     vec2  texCoord1;\n"
"out     vec4  vColor;\n"
"out     mat4  normalRot;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
""
"    // We need this for normal mapping in the fragment shader.\n"
"    normalRot = mat4(mat3(model));\n"
"    ddRotated = vec4(dd.xyz, 0) * normalRot;\n"
""
"    texCoord0 = inTexCoord;\n"
"    texCoord1 = (normalRot * vec4(inTexCoord, 0.0, 0.0)).xy;\n"
""
"    vColor = inColor * vec4(ac.rgb, 1.0) * ac.a;\n"
"}\n";

static const char * carFsh =
"#version 130\n"
""
"in      vec4      ddRotated;\n"
"in      vec2      texCoord0;\n"
"in      vec2      texCoord1;\n"
"in      vec4      vColor;\n"
"in      mat4      normalRot;\n"
"uniform sampler2D tex0;\n"
"uniform sampler2D tex1;\n"
"uniform sampler2D tex2;\n"
"uniform vec4      dd;\n"
"uniform vec4      dc;\n"
"out     vec4      fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 texColor = texture2D(tex0, texCoord0);\n"
"    vec4 sky      = texture2D(tex1, texCoord1);\n"
"    vec4 map      = texture2D(tex2, texCoord0);\n"
""
"    if (texColor.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        vec4  N  = (map - vec4(0.5, 0.5, 0.5, 0.0)) * 2.0;\n"
"        float di = dot(-ddRotated, N) * dc.a;\n"
""
"        // Sky reflection\n"
"        const float keyThreshold = 0.75;\n"
"        if (texColor.r < keyThreshold && texColor.g > keyThreshold && texColor.b < keyThreshold)\n"
"        {\n"
"            fragColor = (sky * texColor.g * vColor + vec4(dc.rgb, 1.0) * di);\n"
"        }\n"
"        else\n"
"        {\n"
"            fragColor = (texColor * vColor + vec4(dc.rgb, 1.0) * di);\n"
"        }\n"
"    }\n"
"}\n";

static const char * fboVsh =
"#version 130\n"
""
"in  vec3 inVertex;\n"
"in  vec2 inTexCoord;\n"
"out vec2 texCoord0;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vec4(inVertex, 1);\n"
"    texCoord0 = inTexCoord;\n"
"}\n";

static const char * fboFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in     vec2       texCoord0;\n"
"out    vec4       fragColor;\n"
""
"void main(void)\n"
"{\n"
"    fragColor = texture2D(tex0, texCoord0);\n"
"}\n";

static const char * tileVsh =
"#version 130\n"
""
"in      vec3  inVertex;\n"
"in      vec3  inNormal;\n"
"in      vec2  inTexCoord;\n"
"in      vec4  inColor;\n"
"uniform vec4  scale;\n"
"uniform mat4  vp;\n"
"uniform mat4  model;\n"
"uniform vec4  dd;\n"
"uniform vec4  dc;\n"
"uniform vec4  ac;\n"
"out     vec2  texCoord0;\n"
"out     vec2  texCoord1;\n"
"out     vec4  vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vp * model * vec4(inVertex, 1);\n"
"    float di = dot(dd, vec4(-inNormal, 1)) * dc.a;\n"
"    vColor = inColor * (\n"
"        vec4(ac.rgb, 1.0) * ac.a +\n"
"        vec4(dc.rgb, 1.0) * di);\n"
""
"    texCoord0 = inTexCoord;\n"
"    texCoord1 = inTexCoord;\n"
"}\n";

static const char * tile3dFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"uniform sampler2D tex1;\n"
"uniform sampler2D tex2;\n"
"in      vec2      texCoord0;\n"
"in      vec4      vColor;\n"
"out     vec4      fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 color0 = texture2D(tex0, texCoord0);\n"
"    vec4 color1 = texture2D(tex1, texCoord0);\n"
"    vec4 color2 = texture2D(tex2, texCoord0);\n"
""
"    if (color0.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        if (color0.r == 0.0 && color0.b == 0.0)\n"
"        {\n"
"            color0 = mix(color1, color2, color0.g);\n"
"        }\n"
"        else if (color0.g == 0.0 && color0.b == 0.0)\n"
"        {\n"
"            color0 = color1;\n"
"        }\n"
""
"        fragColor = color0 * vColor;\n"
"    }\n"
"}\n";

#endif // SHADERS30_H
