#include "SubMeshRenderer.h"
#include "DeferredRenderPipeline.h"

#ifndef CXC_DEFERREDRENDER_H
#define CXC_DEFERREDRENDER_H

namespace cxc
{
	class SubMesh;
	class LightSource;

	class DeferredRenderer : public SubMeshRenderer
	{
	public:

		DeferredRenderer();
		DeferredRenderer(const std::string& Name);
		virtual ~DeferredRenderer();

	public:

		virtual bool InitializeRenderer() override;

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	private:

		void CreateGBufferTextures();

	public:

		GLuint GetGBufferID() const { return GeometryFrameBuffer; }
		GLuint GetVertexPositionTextureID() const { return VertexPositionTexture; }
		GLuint GetVertexDiffuseTextureID() const { return VertexDiffuseTexture; }
		GLuint GetVertexNormalTextureID() const { return VertexNormalTexture; }

	private:

		/* G-Buffer storing vertex position, color ,normal */
		GLuint GeometryFrameBuffer;

		/* Depth buffer */
		GLuint DepthBuffer;

		/* Texture storing the vertex position */
		GLuint VertexPositionTexture;

		/* Texture storing the vertex color, which equals to diffuse color + specular color, the ambient color calulated in the lighting pass */
		GLuint VertexDiffuseTexture;

		/* Texture storing the vertex normal */
		GLuint VertexNormalTexture;
		
	};
}

#endif // CXC_DEFERREDRENDER_H