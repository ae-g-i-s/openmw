#include "aiescort.hpp"

#include <components/esm/aisequence.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "../mwworld/cellstore.hpp"
#include "../mwworld/class.hpp"

#include "steering.hpp"
#include "movement.hpp"

/*
    TODO: Test vanilla behavior on passing x0, y0, and z0 with duration of anything including 0.
    TODO: Different behavior for AIEscort a d x y z and AIEscortCell a c d x y z.
    TODO: Take account for actors being in different cells.
*/

namespace MWMechanics
{
    AiEscort::AiEscort(const std::string &actorId, int duration, float x, float y, float z)
    : mActorId(actorId), mX(x), mY(y), mZ(z), mRemainingDuration(duration)
    , mCellX(std::numeric_limits<int>::max())
    , mCellY(std::numeric_limits<int>::max())
    {
        mMaxDist = 470;

        // The CS Help File states that if a duration is given, the AI package will run for that long
        // BUT if a location is givin, it "trumps" the duration so it will simply escort to that location.
        if(mX != 0 || mY != 0 || mZ != 0)
            mRemainingDuration = 0;
    }

    AiEscort::AiEscort(const std::string &actorId, const std::string &cellId,int duration, float x, float y, float z)
    : mActorId(actorId), mCellId(cellId), mX(x), mY(y), mZ(z), mRemainingDuration(duration)
    , mCellX(std::numeric_limits<int>::max())
    , mCellY(std::numeric_limits<int>::max())
    {
        mMaxDist = 470;

        // The CS Help File states that if a duration is given, the AI package will run for that long
        // BUT if a location is given, it "trumps" the duration so it will simply escort to that location.
        if(mX != 0 || mY != 0 || mZ != 0)
            mRemainingDuration = 0;
    }

    AiEscort::AiEscort(const ESM::AiSequence::AiEscort *escort)
        : mActorId(escort->mTargetId), mX(escort->mData.mX), mY(escort->mData.mY), mZ(escort->mData.mZ)
        , mCellX(std::numeric_limits<int>::max())
        , mCellY(std::numeric_limits<int>::max())
        , mCellId(escort->mCellId)
        , mRemainingDuration(escort->mRemainingDuration)
    {
    }


    AiEscort *MWMechanics::AiEscort::clone() const
    {
        return new AiEscort(*this);
    }

    bool AiEscort::execute (const MWWorld::Ptr& actor,float duration)
    {
        // If AiEscort has ran for as long or longer then the duration specified
        // and the duration is not infinite, the package is complete.
        if(mRemainingDuration != 0)
        {
            mRemainingDuration -= duration;
            if (duration <= 0)
                return true;
        }

        const MWWorld::Ptr follower = MWBase::Environment::get().getWorld()->getPtr(mActorId, false);
        const float* const leaderPos = actor.getRefData().getPosition().pos;
        const float* const followerPos = follower.getRefData().getPosition().pos;
        double differenceBetween[3];

        for (short counter = 0; counter < 3; counter++)
            differenceBetween[counter] = (leaderPos[counter] - followerPos[counter]);

        float distanceBetweenResult =
            (differenceBetween[0] * differenceBetween[0]) + (differenceBetween[1] * differenceBetween[1]) + (differenceBetween[2] *
                differenceBetween[2]);

        if(distanceBetweenResult <= mMaxDist * mMaxDist)
        {
            if(pathTo(actor,ESM::Pathgrid::Point(mX,mY,mZ),duration)) //Returns true on path complete
                return true;
            mMaxDist = 470;
        }
        else
        {
            // Stop moving if the player is to far away
            MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(actor, "idle3", 0, 1);
            actor.getClass().getMovementSettings(actor).mPosition[1] = 0;
            mMaxDist = 330;
        }

        return false;
    }

    int AiEscort::getTypeId() const
    {
        return TypeIdEscort;
    }

    void AiEscort::writeState(ESM::AiSequence::AiSequence &sequence) const
    {
        std::auto_ptr<ESM::AiSequence::AiEscort> escort(new ESM::AiSequence::AiEscort());
        escort->mData.mX = mX;
        escort->mData.mY = mY;
        escort->mData.mZ = mZ;
        escort->mTargetId = mActorId;
        escort->mRemainingDuration = mRemainingDuration;
        escort->mCellId = mCellId;

        ESM::AiSequence::AiPackageContainer package;
        package.mType = ESM::AiSequence::Ai_Escort;
        package.mPackage = escort.release();
        sequence.mPackages.push_back(package);
    }
}

