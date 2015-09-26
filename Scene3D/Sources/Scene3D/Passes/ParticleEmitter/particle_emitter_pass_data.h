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

#include "Scene3D/scene_particle_emitter.h"
#include "Scene3D/SceneCache/resource.h"
#include "particle.h"
#include "particle_uniforms.h"

namespace uicore
{

class ParticleEmitterPassData
{
public:
	ParticleEmitterPassData();
	bool update(float time_elapsed);

	bool visible;
	bool in_active_list;

	float time_to_next_emit;
	std::vector<Particle> cpu_particles;
	int last_emitted_index;

	SceneParticleEmitter_Impl *emitter;

	Resource<Texture> life_color_gradient;
	Resource<Texture> particle_animation;
	UniformVector<ParticleUniforms> gpu_uniforms;

private:
	bool advance(float time_elapsed);
	void emit();
	static float random(float min_val, float max_val);
};

}

