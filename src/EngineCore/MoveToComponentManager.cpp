#include "MoveToComponentManager.hpp"

inline size_t EngineCore::Common::MoveToComponentManager::addComponent(Entity entity, Vec3 target_position, float speed, Space move_orientation)
{
    auto index = data_.addComponent(
        {
            entity,
            target_position,
            speed,
            move_orientation
        }
    );

    addIndex(entity.id(), index);

    auto [page_idx, idx_in_page] = data_.getIndices(index);

    return index;
}

void EngineCore::Common::MoveToComponentManager::deleteComponent(Entity entity)
{
    //TODO
}

size_t EngineCore::Common::MoveToComponentManager::getComponentCount() const
{
    return data_.getComponentCount();
}
