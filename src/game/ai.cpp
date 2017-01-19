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

QString AI::selitys() const
{
    return m_selitys;
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

    if(nextNode->index()==0)
        m_nextSegmentLenght=0;

    if(currentNode->index()==0)
        m_nextSegmentLenght=0;

    if(m_car.nearestCar() != NULL){
        MCVector3dF toNearestCar =  m_car.nearestCar()->location() - m_car.location();
        m_angleToNearestCar = MCTrigonom::radToDeg(std::atan2(toNearestCar.j(), toNearestCar.i()));
        m_distanceToNearestCar = toNearestCar.length();
        m_speedOfNearestCar = m_car.nearestCar()->absSpeed();
    }else{
        m_angleToNearestCar = 0;
        m_distanceToNearestCar = 99999;
        m_speedOfNearestCar = 0;
    }

}

void AI::steerControl()
{
    MCFloat diff  = normalizeAngle(m_targetAngle - m_carAngle);

    if(m_car.index() % 2){
    }else{
        //if(m_distanceToNearestCar<100){
        //}else{
            //very simple polynomial interpolation

            if(m_nextSegmentLenght > 0){
                diff -= normalizeAngle(m_nextSegmentAngle - m_targetAngle) * 0.0001 * m_distanceToTarget;
            }

        //}
    }

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
    m_selitys="vapaana";

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

            // tilannenopeus on iso, kun absoluuttinen nopeus on korkea lähellä mutkaa
            // vaihtelee välillä -0.5
            //float tilannenopeus = absSpeed * absSpeed / (m_distanceToTarget*1);
            float tilannenopeus = absSpeed * absSpeed/400;

            //adjust speed according to the difference between current and target angles
            if(m_distanceToTarget > 30 && false){
                const MCFloat diff = normalizeAngle(m_targetAngle - m_carAngle);

                if(fabs(diff)>10 && tilannenopeus>0.36){
                    brake = true;
                    m_selitys+=QString("t1");
                    //m_selitys+=QString("t ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                if(fabs(diff)>20 && tilannenopeus>0.25){
                    brake = true;
                    m_selitys+=QString("t2");
                   // m_selitys+=QString("t ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                if(fabs(diff)>40 && tilannenopeus>0.16){
                    accelerate = false;
                    m_selitys+=QString("t3");
                    //m_selitys+=QString("t ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                if(fabs(diff)>80 && tilannenopeus>0.10){
                    accelerate = false;
                    m_selitys+=QString("t4");
                   // m_selitys+=QString("t ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }

            }

            //brakes if car is on wrong course before the next segment

            //ignore zero-length segment because it is finishline
            if(m_nextSegmentLenght > 2){

                const MCFloat diff = fabs(normalizeAngle(m_targetAngle - m_nextSegmentAngle));

                if(tilannenopeus>0.45 && diff>20 && false){
                    brake = true;
                    m_selitys=QString("nt1 ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                if(tilannenopeus>0.40 && diff>60){
                    brake = true;
                    m_selitys=QString("nt2 ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                if(tilannenopeus>0.35 && diff>90){
                    brake = true;
                    m_selitys=QString("nt3 ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                if(tilannenopeus>0.30 && diff>100){
                    brake = true;
                    m_selitys=QString("nt4 ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                if(tilannenopeus>0.20 && diff>110){
                    brake = true;
                    m_selitys=QString("nt5 ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1);
                }
                /*
                if(tilannenopeus>0.10 && diff>130){
                    brake = true;
                    m_selitys=QString("nt6 ") + QString::number(tilannenopeus, 'f', 3)+", "+QString::number(fabs(diff), 'f', 1)+", "+QString::number(m_nextSegmentLenght);
                }*/
            }

            //don't push car ahead
            if(m_distanceToNearestCar<100){

                m_selitys="takaluukussa";

                MCFloat diff=fabs(normalizeAngle(m_targetAngle-m_angleToNearestCar));
                float nopeusero = absSpeed - m_speedOfNearestCar;
                if(nopeusero>0){
                    float tilannenopeus = nopeusero * nopeusero / (m_distanceToNearestCar-30);
                    if(tilannenopeus>0.01 && diff < 20){
                        brake = true;
                        m_selitys="vaarassa osua, ";// + QString::number(nopeusero, 'f', 3);
                    }else{
                        m_selitys="ei vaaraa diff=";// + QString::number(diff, 'f', 3)+" tilannenopeus="+ QString::number(nopeusero, 'f', 3);
                    }
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
