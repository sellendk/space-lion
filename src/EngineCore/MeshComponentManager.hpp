/// <copyright file="MeshComponentManager.hpp">
/// Copyright � 2018 Michael Becher. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Michael Becher</author>

#ifndef MeshComponentManager_hpp
#define MeshComponentManager_hpp

#include "EntityManager.hpp"
#include "BaseMultiInstanceComponentManager.hpp"
#include "BaseResourceManager.hpp"

namespace EngineCore
{
    namespace Graphics
    {
        template<typename ResourceManagerType>
        class MeshComponentManager : public BaseMultiInstanceComponentManager
        {
        public:
            typedef typename ResourceManagerType::VertexLayout          VertexLayoutType;
            typedef typename ResourceManagerType::IndexFormatType       IndexFormatType;
            typedef typename ResourceManagerType::PrimitiveTopologyType PrimitiveTopologyType;

            MeshComponentManager(ResourceManagerType* resource_manager) : BaseMultiInstanceComponentManager(), m_resource_mngr(resource_manager) {}
            ~MeshComponentManager() = default;

            template<typename VertexContainer, typename IndexContainer>
            ResourceID addComponent(
                Entity const&                                            entity,
                std::string const&                                       mesh_description,
                std::shared_ptr<std::vector<VertexContainer>> const&     vertex_data,
                std::shared_ptr<IndexContainer> const&                   index_data,
                std::shared_ptr<std::vector<GenericVertexLayout>> const& generic_vertex_layouts,
                uint32_t const&                                          generic_index_type,
                PrimitiveTopologyType const&                             mesh_type,
                bool                                                     store_seperate = false);

            template<typename VertexContainer, typename IndexContainer>
            ResourceID addComponent(
                Entity const& entity,
                std::string const& mesh_description,
                std::shared_ptr<std::vector<VertexContainer>> const& vertex_data,
                std::shared_ptr<IndexContainer> const& index_data,
                std::shared_ptr<std::vector<VertexLayoutType>> const& vertex_layouts,
                IndexFormatType const& index_type,
                PrimitiveTopologyType const& mesh_type,
                bool                                                  store_seperate = false);

            void addComponent(
                Entity const& entity,
                std::string const& mesh_description,
                ResourceID const& mesh_resource,
                uint32_t    first_index,
                uint32_t    indices_cnt,
                uint32_t    base_vertex
            );

            template<typename VertexContainer, typename IndexContainer>
            void updateComponent(
                Entity const& entity,
                std::shared_ptr<std::vector<VertexContainer>> const& vertex_data,
                std::shared_ptr<IndexContainer> const& index_data
            );

            ResourceID getMeshResourceID(Entity const& entity, size_t sub_idx = 0) const;

            std::tuple<uint32_t, uint32_t, uint32_t> getDrawIndexedParams(size_t component_index) const;

        private:

            struct ComponentData
            {
                ComponentData(
                    Entity const       entity,
                    std::string const& mesh_description,
                    ResourceID const   mesh_rsrc,
                    uint32_t const     first_index,
                    uint32_t const     indices_cnt,
                    uint32_t const     base_vertex)
                    : entity(entity),
                    mesh_description(mesh_description),
                    mesh_resource(mesh_rsrc),
                    first_index(first_index),
                    indices_cnt(indices_cnt),
                    base_vertex(base_vertex)
                {}

                Entity      entity;
                std::string mesh_description;
                ResourceID  mesh_resource;
                uint32_t    first_index;
                uint32_t    indices_cnt;
                uint32_t    base_vertex;
            };

            struct MeshData
            {
                MeshData(
                    ResourceID                           mesh_rsrc,
                    std::vector<VertexLayoutType> const& mesh_vertexLayouts,
                    IndexFormatType                      mesh_indexType,
                    size_t                               vertices_allocated,
                    size_t                               indices_allocated)
                    : mesh_resource(mesh_rsrc),
                      vertices_allocated(vertices_allocated),
                      mesh_vertexLayouts(mesh_vertexLayouts),
                      mesh_indexType(mesh_indexType),
                      vertices_used(0),
                      indices_allocated(indices_allocated),
                      indices_used(0) {}

                ResourceID                    mesh_resource;
                std::vector<VertexLayoutType> mesh_vertexLayouts;
                IndexFormatType               mesh_indexType;
                size_t                        vertices_allocated;
                size_t                        vertices_used;
                size_t                        indices_allocated;
                size_t                        indices_used;
            };

            std::vector<ComponentData> m_component_data;
            std::vector<MeshData>      m_mesh_data;
            mutable std::shared_mutex  m_data_mutex;

            ResourceManagerType*       m_resource_mngr;
        };

        template<typename ResourceManagerType>
        template<typename VertexContainer, typename IndexContainer>
        inline ResourceID MeshComponentManager<ResourceManagerType>::addComponent(
            Entity const&                                            entity,
            std::string const&                                       mesh_description,
            std::shared_ptr<std::vector<VertexContainer>> const&     vertex_data,
            std::shared_ptr<IndexContainer> const&                   index_data,
            std::shared_ptr<std::vector<GenericVertexLayout>> const& generic_vertex_layouts,
            uint32_t const&                                          generic_index_type,
            PrimitiveTopologyType const&                             mesh_type,
            bool                                                     store_seperate)
        {
            // convert generic vertex and index descriptions to API-sepecific data types
            std::shared_ptr<std::vector<typename ResourceManagerType::VertexLayout>> vertex_layouts
                = std::make_shared<std::vector<typename ResourceManagerType::VertexLayout>>();
            unsigned int base_input_slot = 0;
            for (auto& generic_vertex_layout : (*generic_vertex_layouts))
            {
                vertex_layouts->push_back(m_resource_mngr->convertGenericGltfVertexLayout(generic_vertex_layout, base_input_slot));
                ++base_input_slot;
            }
            auto index_type = m_resource_mngr->convertGenericIndexType(generic_index_type);

            return addComponent(entity, mesh_description, vertex_data, index_data, vertex_layouts, index_type, mesh_type, store_seperate);
        }

        template<typename ResourceManagerType>
        template<typename VertexContainer, typename IndexContainer>
        inline ResourceID MeshComponentManager<ResourceManagerType>::addComponent(
            Entity const& entity,
            std::string const& mesh_description,
            std::shared_ptr<std::vector<VertexContainer>> const& vertex_data,
            std::shared_ptr<IndexContainer> const& index_data,
            std::shared_ptr<std::vector<VertexLayoutType>> const& vertex_layouts,
            IndexFormatType const& index_type,
            PrimitiveTopologyType const& mesh_type,
            bool store_seperate)
        {
            // get vertex buffer data pointers and byte sizes
            size_t vbs_byteSize = 0;
            for (auto& vb : (*vertex_data)) {
                vbs_byteSize += (sizeof(VertexContainer::value_type) * vb.size());
            }
            // compute overall byte size of index buffer
            size_t ib_byte_size = sizeof(IndexContainer::value_type) * index_data->size();

            // get vertex byte size, i.e. bytes used across all vertex buffer by a single vertex
            size_t vertex_byteSize = m_resource_mngr->computeVertexByteSize(*vertex_layouts);
            // get index byte size, i.e. byte size used by a single index in the index buffer
            size_t index_byteSize = m_resource_mngr->computeIndexByteSize(index_type);

            // computer number of requested vertices and indices
            size_t req_vertex_cnt = vbs_byteSize / vertex_byteSize;
            size_t req_index_cnt = ib_byte_size / index_byteSize;

            std::unique_lock<std::shared_mutex> lock(m_data_mutex);

            auto it = m_mesh_data.begin();
            if (!store_seperate)
            {
                // check for existing mesh batch with matching vertex layout and index type and enough available space
                for (; it != m_mesh_data.end(); ++it)
                {
                    bool layout_check = true;

                    if (vertex_layouts->size() == it->mesh_vertexLayouts.size())
                    {
                        for (size_t i = 0; i < vertex_layouts->size(); ++i)
                        {
                            layout_check &= ((*vertex_layouts)[i] == it->mesh_vertexLayouts[i]);
                        }
                    }
                    else
                    {
                        layout_check = false;
                    }

                    bool idx_type_check = (index_type == it->mesh_indexType);

                    if (layout_check && idx_type_check)
                    {
                        // check whether there is enough space left in batch
                        size_t ava_vertex_cnt = (it->vertices_allocated - it->vertices_used);
                        size_t ava_index_cnt = (it->indices_allocated - it->indices_used);

                        if ((req_vertex_cnt < ava_vertex_cnt) && (req_index_cnt < ava_index_cnt))
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                it = m_mesh_data.end();
            }

            // Create a new gpu mesh object if no matching mesh was found
            if (it == m_mesh_data.end())
            {
                std::string mesh_name("MeshComponanteManager_BatchMesh_" + std::to_string(m_mesh_data.size()));

                ResourceID new_mesh = m_resource_mngr->allocateMeshAsync(
                    mesh_name,
                    1500000,
                    2000000,
                    vertex_layouts,
                    index_type,
                    mesh_type);

                m_mesh_data.emplace_back(MeshData(new_mesh, *vertex_layouts, index_type, 1500000, 2000000));

                it = m_mesh_data.end();
                --it;
            }

            addIndex(entity.id(), m_component_data.size());
            m_component_data.push_back(ComponentData(
                entity,
                mesh_description,
                it->mesh_resource,
                static_cast<uint32_t>(it->indices_used),
                static_cast<uint32_t>(req_index_cnt),
                static_cast<uint32_t>(it->vertices_used)));

            // update mesh async (in case a new mesh was created, the data update needs to wait async!)
            m_resource_mngr->updateMeshAsync(
                it->mesh_resource,
                it->vertices_used,
                it->indices_used,
                vertex_data,
                index_data);

            it->vertices_used += req_vertex_cnt;
            it->indices_used += req_index_cnt;

            return it->mesh_resource;
        }

        template<typename ResourceManagerType>
        template<typename VertexContainer, typename IndexContainer>
        inline void MeshComponentManager<ResourceManagerType>::updateComponent(
            Entity const& entity,
            std::shared_ptr<std::vector<VertexContainer>> const& vertex_data,
            std::shared_ptr<IndexContainer> const& index_data)
        {
            auto entity_mesh_resource_id = getIndex(entity);
            auto cd = m_component_data[entity_mesh_resource_id[0]];

            //auto it = m_mesh_data.begin();
            //while (it != m_mesh_data.end()) {
            //    if (it->mesh_resource == entity_mesh_resource) break;
            //    ++it;
            //}

            // TODO: size check

            // update mesh async
            m_resource_mngr->updateMeshAsync(
                cd.mesh_resource,
                cd.base_vertex,
                cd.first_index,
                vertex_data,
                index_data);
        }


        template<typename ResourceManagerType>
        inline void MeshComponentManager<ResourceManagerType>::addComponent(
            Entity const& entity,
            std::string const& mesh_description,
            ResourceID const& mesh_resource,
            uint32_t first_index,
            uint32_t indices_cnt,
            uint32_t base_vertex)
        {
            std::unique_lock<std::shared_mutex> lock(m_data_mutex);

            addIndex(entity.id(), m_component_data.size());
            m_component_data.push_back(ComponentData(
                entity,
                mesh_description,
                mesh_resource,
                first_index,
                indices_cnt,
                base_vertex));

        }

        template<typename ResourceManagerType>
        inline ResourceID MeshComponentManager<ResourceManagerType>::getMeshResourceID(Entity const& entity, size_t sub_idx) const
        {
            auto retval = ResourceID();

            auto query = getIndex(entity);
            
            std::shared_lock<std::shared_mutex> lock(m_data_mutex);

            if (query.size() > sub_idx) {
                retval = m_component_data[query[sub_idx]].mesh_resource;
            }

            return retval;
        }

        template<typename ResourceManagerType>
        inline std::tuple<uint32_t, uint32_t, uint32_t> MeshComponentManager<ResourceManagerType>::getDrawIndexedParams(size_t component_index) const
        {
            std::shared_lock<std::shared_mutex> lock(m_data_mutex);

            ComponentData const& data = m_component_data[component_index];

            return { data.indices_cnt, data.first_index, data.base_vertex };
        }
    }
}

#endif