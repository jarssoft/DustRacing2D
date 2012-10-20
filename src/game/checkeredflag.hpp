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

#ifndef CHECKEREDFLAG_HPP
#define CHECKEREDFLAG_HPP

#include "overlaybase.hpp"

class MCSurface;

//! Renders checkerflag on top of the game scene.
class CheckeredFlag : public OverlayBase
{
public:

    //! Constructor.
    CheckeredFlag();

    //! \reimp
    virtual void setDimensions(MCUint width, MCUint height);

    //! \reimp
    virtual void render();

private:

    MCSurface & m_surface;
};

#endif // CHECKEREDFLAG_HPP
