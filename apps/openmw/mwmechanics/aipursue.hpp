#ifndef GAME_MWMECHANICS_AIPURSUE_H
#define GAME_MWMECHANICS_AIPURSUE_H

#include "aipackage.hpp"

#include "../mwbase/world.hpp"

#include "pathfinding.hpp"

namespace ESM
{
namespace AiSequence
{
    struct AiPursue;
}
}

namespace MWMechanics
{
    /// \brief Makes the actor very closely follow the actor
    /** Used for arresting players. Causes the actor to run to the pursued actor and activate them, to arrest them.
        Note that while very similar to AiActivate, it will ONLY activate when evry close to target (Not also when the
        path is completed). **/
    class AiPursue : public AiPackage
    {
        public:
            ///Constructor
            /** \param actor Actor to pursue **/
            AiPursue(const MWWorld::Ptr& actor);

            AiPursue(const ESM::AiSequence::AiPursue* pursue);

            virtual AiPursue *clone() const;
            virtual bool execute (const MWWorld::Ptr& actor,float duration);
            virtual int getTypeId() const;

            MWWorld::Ptr getTarget() const;

            virtual void writeState (ESM::AiSequence::AiSequence& sequence) const;

        private:

            int mTargetActorId; // The actor to pursue
            int mCellX;
            int mCellY;
    };
}
#endif
