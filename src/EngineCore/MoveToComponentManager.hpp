#ifndef MoveToComponentManager_hpp
#define MoveToComponentManager_hpp

// space-lion includes
#include "BaseSingleInstanceComponentManager.hpp"
#include "ComponentStorage.hpp"

namespace EngineCore
{
    namespace Common
    {

        class MoveToComponentManager : public BaseSingleInstanceComponentManager
        {
        private:
            enum class Space
            {
                GLOBAL,
                LOCAL
            };

            struct Data
            {
                Entity entity;           ///< entity that owns the component
                Vec3   target_position;  ///< target position of the movement
                float  speed;            ///< movement speed in m/s
                Space  move_orientation; ///< space in which the movement occurs (local or global)
            };

            Utility::ComponentStorage<Data, 1000, 1000> data_;

        public:
            MoveToComponentManager() = default;
            ~MoveToComponentManager() = default;

            inline size_t addComponent(Entity entity, Vec3 target_position, float speed = 1.0f, Space move_orientation = Space::LOCAL);

            void deleteComponent(Entity entity);

            size_t getComponentCount() const;
        };

    }
}

#endif // !MoveToComponentManager_hpp
