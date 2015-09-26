
#pragma once

/// \brief A list of time stamps and the values to be used for each.
template<typename Type>
class ModelDataAnimationTimeline
{
public:
	std::vector<float> timestamps;
	std::vector<Type> values;

	Type get_value(float timestamp) const
	{
		size_t index, index2;
		float t = find_animation_indices(timestamp, index, index2);
		return mix(values[index], values[index2], t);
	}

	float find_animation_indices(float timestamp, size_t &index, size_t &index2) const
	{
		if (timestamps.empty())
		{
			index = 0;
			index2 = 0;
			return 0.0f;
		}

		index = binary_search(timestamp);
		index2 = uicore::min(index + 1, timestamps.size() - 1);

		float start = timestamps[index];
		float end = timestamps[index2];
		if (start != end)
			return uicore::clamp((timestamp - start) / (end - start), 0.0f, 1.0f);
		else
			return 0.0f;
	}

private:
	size_t binary_search(float timestamp) const
	{
		int search_max = static_cast<int>(timestamps.size()) - 1;
		int imin = 0;
		int imax = search_max;
		while (imin < imax)
		{
			int imid = imin + (imax - imin) / 2;
			if (timestamps[imid] > timestamp)
				imax = imid;
			else
				imin = imid + 1;
		}
		bool found = (imax == imin && timestamps[imin] > timestamp);
		return found ? imin - 1 : search_max;
	}
};

template<>
inline uicore::Quaternionf ModelDataAnimationTimeline<uicore::Quaternionf>::get_value(float timestamp) const
{
	size_t index, index2;
	float t = find_animation_indices(timestamp, index, index2);
	return uicore::Quaternionf::slerp(values[index], values[index2], t);
}

template<>
inline uicore::Mat4f ModelDataAnimationTimeline<uicore::Mat4f>::get_value(float timestamp) const
{
	size_t index, index2;
	float t = find_animation_indices(timestamp, index, index2);
	return values[index];
}
