#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Scene\Lighting.h"

#else

#include "../General/DefineTypes.h"
#include "../Scene/Lighting.h"

#endif

#ifndef CXC_RENDER_H
#define CXC_RENDER_H

#include "RenderPipeline.h"

namespace cxc
{
	class Mesh;
	class Object3D;

	class MeshRender : public std::enable_shared_from_this<MeshRender>
	{

	public:

		MeshRender();
		MeshRender(const std::string& Name);

		virtual ~MeshRender();

	public:

		virtual bool InitializeRender();
		void AddRenderingPipeline(std::shared_ptr<RenderPipeline> pPipeline);

	public:

		std::shared_ptr<RenderPipeline> GetCurrentUsedPipeline() { return CurrentUsedPipeline; }
		std::string GetRenderName() const { return RenderName; }
		void SetRenderName(const std::string& NewName) { RenderName = NewName; }
		std::shared_ptr<RenderPipeline> GetPipelinePtr(const std::string& PipelineName);
		void UsePipeline(std::shared_ptr<RenderPipeline> Pipeline);
		void BindCameraUniforms(GLuint ProgramID);

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);

	protected:

		std::shared_ptr<RenderPipeline> CurrentUsedPipeline;

		// Name of the render
		std::string RenderName;

		// Rendering pipeline
		std::unordered_map<std::string, std::shared_ptr<RenderPipeline>> pRenderPipelines;
	};
}

#endif // CXC_RENDER_H