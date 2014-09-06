/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include "Scene3D/Model/model_mesh_visitor.h"

namespace clan
{

class Scene_Impl;

class TransparencyPass : ModelMeshVisitor
{
public:
	TransparencyPass(ResourceContainer &inout);
	void run(GraphicContext &gc, Scene_Impl *scene);

private:
	void setup(GraphicContext &gc);
	void render(GraphicContext &gc, ModelLOD *model_lod, int num_instances);

	// In:
	Resource<Rect> viewport;
	Resource<float> field_of_view;
	Resource<Mat4f> world_to_eye;
	Resource<Texture2D> zbuffer;

	// InOut:
	Resource<Texture2D> final_color;

	FrameBuffer fb_transparency;
	GraphicContext gc;
	BlendState blend_state;
	DepthStencilState depth_stencil_state;
};

}

