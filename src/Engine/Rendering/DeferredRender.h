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
		DeferredRender(const std::string& Name);
		virtual ~DeferredRender();

	public:

		virtual bool InitializeRender() override;

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	private:

		void CreateGBufferTextures();

	public:

		GLuint GetGBufferID() const { return GeometryFrameBuffer; }
		GLuint GetVertexPositionTextureID() const { return VertexPositionTexture; }
		GLuint GetVertexDiffuseTextureID() const { return VertexDiffuseTexture; }
		GLuint GetVertexNormalTextureID() const { return VertexNormalTexture; }

		void SetDeferredRenderPipeline(std::shared_ptr<DeferredRenderPipeline> Pipeline);

	private:

		/* Geometry passing pipeline */
		std::shared_ptr<DeferredRenderPipeline> pDeferredRenderPipeline;

		/* G-Buffer storing vertex position, color ,normal */
		GLuint GeometryFrameBuffer;

		/* Texture storing the vertex position */
		GLuint VertexPositionTexture;

		/* Texture storing the vertex color, which equals to diffuse color + specular color, the ambient color calulated in the lighting pass */
		GLuint VertexDiffuseTexture;

		/* Texture storing the vertex normal */
		GLuint VertexNormalTexture;
	};
}

#endif // CXC_DEFERREDRENDER_H