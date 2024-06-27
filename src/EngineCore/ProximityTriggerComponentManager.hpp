#ifndef ProximityTriggerComponentManager_hpp
#define ProximityTriggerComponentManager_hpp

#include "functional"

#include "BaseMultiInstanceComponentManager.hpp"
#include "ComponentStorage.hpp"

namespace EngineCore
{
    namespace Common {

        class ProximityTriggerComponentManager : public BaseMultiInstanceComponentManager
        {
        private:
            struct Data
            {
                Entity                entity;           ///< entity that owns the component
                Entity                target;           ///< entity for which the proximity is tracked
                float                 trigger_distance; ///< distance at which the callback will trigger
                bool                  in_proximity;     ///< flag whether target was in proximity during the last check
                std::function<void()> enter_callback;   ///< function that is called when the target enters proximity
                std::function<void()> leave_callback;   ///< function that is called when the target leaves proximity
            };

            Utility::ComponentStorage<Data, 1000, 1000> data_;

        public:
            ProximityTriggerComponentManager() = default;
            ~ProximityTriggerComponentManager() = default;

            size_t addComponent(Entity entity, Entity target, float trigger_distance, std::function<void()> enter_callback, std::function<void()> leave_callback);

            void deleteComponent(Entity entity);

            size_t getComponentCount() const;

            bool checkComponent(size_t index);

            Data const& getComponent(size_t index) const;

            Data& getComponent(size_t index);
        };

    }
}

#endif // !ProximityTriggerComponentManager_hpp
