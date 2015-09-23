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

#include "Scene3D/Resources/resource_container.h"
#include "Scene3D/Resources/resource.h"

namespace uicore
{

class LogAverageLight
{
public:
	LogAverageLight(GraphicContext &gc, ResourceContainer &inout, int iterations = 8);
	Texture2D &find_log_average_light(GraphicContext &gc, Texture2D &hdr_texture);

private:
	ProgramObject compile_and_link(GraphicContext &gc, const std::string &vertex_source, const std::string &fragment_source);

	// In:
	Resource<Rect> viewport;

	int iterations;
	ProgramObject program0, program1, program2;
	PrimitivesArray prim_array;
	VertexArrayVector<Vec4f> vertices;
	FrameBuffer fb0, fb1, fb2, fb3;
	Texture2D texture0, texture1;
	Texture2D result_texture0, result_texture1;
	int current_index;
	BlendState blend_state;
};

}

