// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "race.hpp"
#include "ai.hpp"
#include "car.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "../common/route.hpp"
#include "../common/tracktilebase.hpp"

#include <MCRandom>
#include <MCTrigonom>
#include <MCTypes>

AI::AI(Car & car)
: m_car(car)
, m_track(nullptr)
, m_route(nullptr)
, m_lastDiff(0)
, m_lastTargetNodeIndex(0)
{
}

Car & AI::car() const
{
    return m_car;
}

//keep angles between -180 and 180 degrees
MCFloat normalizeAngle(MCFloat angle){
    bool ok = false;
    while (!ok)
    {
        if (angle > 180)
        {
            angle = angle - 360;
            ok = false;
        }
        else if (angle < -180)
        {
            angle = angle + 360;
            ok = false;
        }
        else
        {
            ok = true;
        }
    }
    return angle;
}

void AI::update(bool isRaceCompleted)
{
    if (m_track)
    {
        if (m_lastTargetNodeIndex != m_car.currentTargetNodeIndex())
        {
            setRandomTolerance();
        }

        m_car.clearStatuses();

        const Route & route = m_track->trackData().route();
        calculateAngles(route.get(m_car.currentTargetNodeIndex()), route.get(m_car.nextTargetNodeIndex()));

        steerControl();

        TrackTile & currentTile = *m_track->trackTileAtLocation(m_car.location().i(), m_car.location().j());
        speedControl(currentTile, isRaceCompleted);

        m_lastTargetNodeIndex = m_car.currentTargetNodeIndex();
    }
}

void AI::setRandomTolerance()
{
    m_randomTolerance = MCRandom::randomVector2d() * TrackTileBase::TILE_W / 8;
}

void AI::calculateAngles(TargetNodePtr currentNode, TargetNodePtr nextNode)
{
    MCVector3dF currentNodeLocation(currentNode->location().x(), currentNode->location().y());
    currentNodeLocation+=MCVector3dF(m_randomTolerance);

    m_carAngle = static_cast<int>(m_car.angle()) % 360;

    MCVector3dF currentCourse(currentNodeLocation);
    currentCourse -= MCVector3dF(m_car.location());
    m_targetAngle = MCTrigonom::radToDeg(std::atan2(currentCourse.j(), currentCourse.i()));
    m_distanceToTarget = currentCourse.length();

    MCVector3dF nextCourse(nextNode->location().x(), nextNode->location().y());
    nextCourse -= currentNodeLocation;
    m_nextSegmentAngle = MCTrigonom::radToDeg(std::atan2(nextCourse.j(), nextCourse.i()));
    m_nextSegmentLenght = nextCourse.length();
}

void AI::steerControl()
{
    MCFloat diff  = normalizeAngle(m_targetAngle - m_carAngle);

    // PID-controller. This makes the computer players to turn and react faster
    // than the human player, but hey...they are stupid.
    MCFloat control = diff * 0.025 + (diff - m_lastDiff) * 0.025;
    const MCFloat maxControl = 1.5;
    control = control < 0 ? -control : control;
    control = control > maxControl ? maxControl : control;

    const MCFloat maxDelta = 3.0;
    if (diff < -maxDelta)
    {
        m_car.steer(Car::Steer::Right, control);
    }
    else if (diff > maxDelta)
    {
        m_car.steer(Car::Steer::Left, control);
    }

    // Store the last difference
    m_lastDiff = diff;
}

void AI::speedControl(TrackTile & currentTile, bool isRaceCompleted)
{
    // TODO: Maybe it'd be possible to adjust speed according to
    // the difference between current and target angles so that
    // computer hints wouldn't be needed anymore..?

    // Braking / acceleration logic
    bool accelerate = true;
    bool brake      = false;

    const float absSpeed = m_car.absSpeed();
    if (isRaceCompleted)
    {
        accelerate = false;
    }
    else
    {
        
        // The following speed limits are experimentally defined.
        float scale = 0.9f;
        
        if(m_car.index() % 2){
            if (currentTile.computerHint() == TrackTile::CH_BRAKE)
            {
                if (absSpeed > 14.0f * scale)
                {
                    brake = true;
                }
            }

            if (currentTile.computerHint() == TrackTile::CH_BRAKE_HARD)
            {
                if (absSpeed > 9.5f * scale)
                {
                    brake = true;
                }
            }

            if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_90)
            {
                if (absSpeed > 7.0f * scale)
                {
                    accelerate = false;
                }
            }

            if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_LEFT ||
                currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_RIGHT)
            {
                if (absSpeed > 8.3f * scale)
                {
                    accelerate = false;
                }
            }

        }else{

            //adjust speed according to the difference between current and target angles
            if(m_distanceToTarget > 100){
                const MCFloat diff = normalizeAngle(m_targetAngle - m_carAngle);

                    if(fabs(diff)>15*0.5 && absSpeed > (12.0f) * scale && m_distanceToTarget < 220+110-50){
                        brake = true;
                    }
                    if(fabs(diff)>30*0.5 && absSpeed > (10.0f) * scale && m_distanceToTarget < 150+75-25){
                        brake = true;
                    }
                    if(fabs(diff)>40*0.5 && absSpeed >  (8.0f) * scale && m_distanceToTarget < 100+50){
                        accelerate = false;
                    }
                    if(fabs(diff)>50*0.5 && absSpeed >  (7.0f) * scale && m_distanceToTarget <  50+25){
                        accelerate = false;
                    }
            }

            //brakes if car is on wrong course before the next segment

            //ignore zero-length segment because it is finishline
            if(m_nextSegmentLenght > 0){

                const MCFloat diff = normalizeAngle(m_targetAngle - m_nextSegmentAngle);

                if(fabs(diff)>15*1.4 && absSpeed > (12.0f) * scale && m_distanceToTarget < 220+110){
                    brake = true;
                }                       
                if(fabs(diff)>30*1.4 && absSpeed > (10.0f) * scale && m_distanceToTarget < 150+75){
                    brake = true;
                }                
                if(fabs(diff)>40*1.4 && absSpeed >  (8.0f) * scale && m_distanceToTarget < 100+50){
                    brake = true;
                }
                if(fabs(diff)>50*1.4 && absSpeed >  (7.0f) * scale && m_distanceToTarget <  50+25){
                    brake = true;
                }
            }
        }

        if (absSpeed < 3.6f * scale)
        {
            accelerate = true;
            brake = false;
        }
    }

    if (brake)
    {
        m_car.brake();
    }
    else if (accelerate)
    {
        m_car.accelerate();
    }
}

void AI::setTrack(Track & track)
{
    m_track = &track;
    m_route = &track.trackData().route();
}
