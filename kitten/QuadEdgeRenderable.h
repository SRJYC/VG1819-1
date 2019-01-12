#pragma once
#include "kitten\mouse picking\ClickableBox.h"
#include "kitten\K_Renderable.h"
#include "puppy\VertexEnvironment.h"
#include "puppy\Material.h"

#define P1_EDGE_TEXTURE "textures/ui/saphire.png"
#define P2_EDGE_TEXTURE "textures/ui/topaz.png"
#define DEFAULT_EDGE_TEXTURE "textures/ui/white.png"

namespace kitten
{
	class QuadEdgeRenderable : public kitten::K_Renderable
	{
	private:
		static puppy::VertexEnvironment* sm_vao;
		static int sm_instances;

		//puppy::VertexEnvironment* m_vao;
		puppy::Material* m_mat;


		virtual void start() override;
		virtual void onEnabled() override;
		virtual void onDisabled() override;

	public:
		QuadEdgeRenderable();
		~QuadEdgeRenderable();

		virtual void render(kitten::Camera* p_cam) override;
		void setTexture(const std::string& p_tex);
	};
}
