#include "MeshRender.h"
#include "DeferredRenderPipeline.h"

#ifndef CXC_DEFERREDRENDER_H
#define CXC_DEFERREDRENDER_H

namespace cxc
{
	class Mesh;
	class LightSource;

	class DeferredRender : public MeshRender
	{
	public:

		DeferredRender();
		virtual ~DeferredRender();

	public:

		virtual bool InitializeRender() override;

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);

	private:

		void CreateGBufferTextures();

	private:

		/* G-Buffer storing vertex position, color ,normal */
		GLuint GeometryFrameBuffer;

		/* Texture storing the vertex position */
		GLuint VertexPositionTexture;

		/* Texture storing the vertex color, which equals to diffuse color + specular color, the ambient color calulated in the lighting pass */
		GLuint VertexColorTexture;

		/* Texture storing the vertex normal */
		GLuint VertexNormalTexture;
	};
}

#endif // CXC_DEFERREDRENDER_H