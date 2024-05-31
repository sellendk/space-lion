#include "MoveToComponentManager.hpp"

size_t EngineCore::Animation::MoveToComponentManager::addComponent(Entity entity, Vec3 target_position, float speed, Space move_orientation)
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

void EngineCore::Animation::MoveToComponentManager::deleteComponent(Entity entity)
{
    //TODO
}

size_t EngineCore::Animation::MoveToComponentManager::getComponentCount() const
{
    return data_.getComponentCount();
}

bool EngineCore::Animation::MoveToComponentManager::checkComponent(size_t index) const
{
    auto indices = data_.getIndices(index);
    return data_.checkComponent(indices.first, indices.second);
}

EngineCore::Animation::MoveToComponentManager::Data const& EngineCore::Animation::MoveToComponentManager::getComponent(size_t index) const
{
    auto indices = data_.getIndices(index);
    return data_(indices.first, indices.second);
}
