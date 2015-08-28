#ifndef RenderJobs_hpp
#define RenderJobs_hpp

#include <memory>
#include <vector>
#include <string>

#include "EntityManager.h"
#include "GLSLProgram.h"
#include "core\material.h"
#include "mesh.h"
#include "MTQueue.hpp"
#include "TransformComponent.h"
#include "CameraComponent.h"

struct RenderJob
{
	RenderJob(Entity e, std::string material_path, std::string mesh_path)
		: entity(e), material_path(material_path), mesh_path(mesh_path) {}

	Entity entity;
	std::string material_path;
	std::string mesh_path;
};

/**
 * Manages all render jobs within a tree datastructure optimized for rendering.
 * Theoretically a render job is a per-frame task issued to the renderer by each
 * entity that has a visual representation. It primarily contains information 
 * about the graphics resources that are used during rendering.
 * In practise, each path from the root node of the tree datastructure to a
 * leave node represents a single render job.
 * Render jobs are processed by traversing the tree in what is basically
 * a depth-first-search, and are handled in batches (using instancing) if possible.
 */
class RenderJobManager
{
private:
	struct MeshNode
	{
		std::shared_ptr<Mesh> mesh;
	
		uint instance_count;
		std::vector<Entity> enities;	///< List of entites that use this mesh. The leaves of the tree datastructure.
	};

	struct MaterialNode
	{
		std::shared_ptr<Material> material;
	
		std::vector<MeshNode> meshes;
	};

	struct ShaderNode
	{
		std::shared_ptr<GLSLProgram> shader_prgm;
	
		std::vector<MaterialNode> materials;
	};

	// Idea: Camera or Framebuffer node? Each camera owns a framebuffer, and choice of camera/fbo is part of a draw job

	struct RootNode
	{
		uint num_render_jobs;

		std::vector<ShaderNode> shaders;
	};

	RootNode m_root;

	MTQueue<RenderJob> m_job_queue;

	/**
	 * Pointer to active TransformComonentManger. Transforms information is essential to rendering
	 */
	TransformComponentManager* transform_components;
	CameraComponentManager*  camera_components;

public:
	RenderJobManager();
	~RenderJobManager();

	void addRenderJob(Entity entity, std::string material, std::string mesh);

	/**
	 * 
	 */
	void processRenderJobs();
};

#endif