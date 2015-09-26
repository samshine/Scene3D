
#pragma once

#include "model_data_animation_timeline.h"

/// \brief Stores different values for different animation timelines
///
/// Animatable properties are stored in timelines. There is one timeline
/// per animation supported by the model.
///
/// For properties that are not to be animated, a single timeline is used
/// with a single value stored in it.
template<typename Type>
class ModelDataAnimationData
{
public:
	std::vector<ModelDataAnimationTimeline<Type> > timelines;

	bool has_multiple_values() const
	{
		return (!timelines.empty() && timelines[0].values.size() > 1) || timelines.size() > 1;
	}

	Type get_value(int animation_index, float timestamp) const
	{
		if (timelines.empty())
			return Type();

		int timeline_index = uicore::min(animation_index, (int)timelines.size() - 1);
		return timelines[timeline_index].get_value(timestamp);
	}

	Type get_single_value() const
	{
		return get_value(0, 0.0f);
	}

	void set_single_value(Type value)
	{
		timelines.resize(1);
		timelines[0].timestamps.resize(1);
		timelines[0].timestamps[0] = 0.0f;
		timelines[0].values.resize(1);
		timelines[0].values[0] = value;
	}
};
