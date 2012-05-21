// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "wall.hpp"
#include "layers.hpp"
#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCMathUtil"
#include "MiniCore/Core/MCRectShape"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/Views/MCSurfaceView"
#include <GL/gl.h>

Wall::Wall(MCSurface & surface, int, int, int newWx, int newWy, int newH)
: MCObject(new MCRectShape(*this, new MCSurfaceView(&surface), newWx, newWy), "WALL")
, wx(newWx)
, wy(newWy)
, wx2(newWx / 2)
, wy2(newWy / 2)
, h(newH)
{
    //setMass(0, true); // Set as a stationary object
    setRestitution(1.0f);
}

void Wall::render(MCCamera * pCamera)
{
    MCFloat x = getX();
    MCFloat y = getY();

    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    // Enable depth test because walls are real 3d objects
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(angle(), 0, 0, 1);
    glBindTexture(GL_TEXTURE_2D, surface()->handle());
    glBegin(GL_QUADS);

    x = wx2;
    y = wy2;

    glColor3f(0.75, 0.75, 0.75);

    // Cap
    glNormal3f(0, 0, 1.0f);
    glTexCoord2i(0, 0);
    glVertex3i(-x, +y, h);
    glTexCoord2i(0, 1);
    glVertex3i(-x, -y, h);
    glTexCoord2i(1, 1 );
    glVertex3i(+x, -y, h);
    glTexCoord2i(1, 0 );
    glVertex3i(+x, +y, h);

    // Back
    glNormal3f(0, 1.0f, 0);
    glTexCoord2i(0, 0);
    glVertex3i(-x, +y, 0);
    glTexCoord2i(0, 1);
    glVertex3i(-x, +y, h);
    glTexCoord2i(1, 1 );
    glVertex3i(+x, +y, h);
    glTexCoord2i(1, 0 );
    glVertex3i(+x, +y, 0);

    // Front
    glNormal3f(0, -1.0f, 0);
    glTexCoord2i(0, 0);
    glVertex3i(+x, -y, 0);
    glTexCoord2i(0, 1);
    glVertex3i(+x, -y, h);
    glTexCoord2i(1, 1 );
    glVertex3i(-x, -y, h);
    glTexCoord2i(1, 0 );
    glVertex3i(-x, -y, 0);

    // Left
    glNormal3f(-1.0f, 0, 0);
    glTexCoord2i(0, 0);
    glVertex3i(-x, -y, 0);
    glTexCoord2i(0, 1);
    glVertex3i(-x, -y, h);
    glTexCoord2i(1, 1 );
    glVertex3i(-x, +y, h);
    glTexCoord2i(1, 0 );
    glVertex3i(-x, +y, 0);

    // Right
    glNormal3f(1.0f, 0, 0);
    glTexCoord2i(0, 0);
    glVertex3i(+x, +y, 0);
    glTexCoord2i(0, 1);
    glVertex3i(+x, +y, h);
    glTexCoord2i(1, 1 );
    glVertex3i(+x, -y, h);
    glTexCoord2i(1, 0 );
    glVertex3i(+x, -y, 0);

    glEnd();
    glPopMatrix();
    glDisable(GL_CULL_FACE);
    glPopAttrib();
}

void Wall::renderShadow(MCCamera * pCamera)
{
    // TODO: Real OpenGL shadows ?

    int x = getX();
    int y = getY();

    // Get absolute (screen) coordinates
    if (pCamera)
    {
        const int shadowOffsetX = wx2 / 2;
        const int shadowOffsetY = wy2 / 2;

        x = pCamera->mapXToCamera(x + shadowOffsetX);
        y = pCamera->mapYToCamera(y - shadowOffsetY);
    }

    // Disable texturing, enable alpha
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

    // Translate to screen coordinates
    glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(angle(), 0, 0, 1);

    x = wx2;
    y = wy2;

    // Shadow color
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);

    // Draw a full shadow
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1.0f);
    glVertex3i(-x, +y, 0);
    glVertex3i(-x, -y, 0);
    glVertex3i(+x, -y, 0);
    glVertex3i(+x, +y, 0);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

Wall::~Wall()
{}
