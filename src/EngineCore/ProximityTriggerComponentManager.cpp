#include "ProximityTriggerComponentManager.hpp"

size_t EngineCore::Common::ProximityTriggerComponentManager::addComponent(Entity entity, Entity target, float trigger_distance, std::function<void()> enter_callback, std::function<void()> leave_callback)
{
    auto index = data_.addComponent(
        {
            entity,
            target,
            trigger_distance,
            false,
            enter_callback,
            leave_callback
        }
    );

    addIndex(entity.id(), index);

    auto [page_idx, idx_in_page] = data_.getIndices(index);

    return index;
}

void EngineCore::Common::ProximityTriggerComponentManager::deleteComponent(Entity entity)
{
    //TODO
}

size_t EngineCore::Common::ProximityTriggerComponentManager::getComponentCount() const
{
    return data_.getComponentCount();
}

bool EngineCore::Common::ProximityTriggerComponentManager::checkComponent(size_t index)
{
    auto indices = data_.getIndices(index);
    return data_.checkComponent(indices.first, indices.second);
}

EngineCore::Common::ProximityTriggerComponentManager::Data const& EngineCore::Common::ProximityTriggerComponentManager::getComponent(size_t index) const
{
    auto indices = data_.getIndices(index);
    return data_(indices.first, indices.second);
}

EngineCore::Common::ProximityTriggerComponentManager::Data& EngineCore::Common::ProximityTriggerComponentManager::getComponent(size_t index)
{
    auto indices = data_.getIndices(index);
    return data_(indices.first, indices.second);
}
