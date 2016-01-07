
#include "precomp.h"
#include "Scene3D/Performance/gpu_timer.h"
#ifdef WIN32
#include <d3d11.h>
#endif

#define ENABLE_GPU_TIMER

using namespace uicore;

class GPUTimerImpl
{
public:
#if defined(WIN32) && defined(ENABLE_GPU_TIMER)
	struct Frame
	{
		std::vector<std::string> names;
		std::vector<ComPtr<ID3D11Query> > queries;
		ComPtr<ID3D11Query> disjoint_query;
	};

	void timestamp(const GraphicContextPtr &gc);

	std::vector<ComPtr<ID3D11Query> > unused_queries;
	std::vector<ComPtr<ID3D11Query> > unused_disjoint_queries;

	std::vector<std::shared_ptr<Frame> > frames;
	std::vector<GPUTimer::Result> last_results;
#endif
};


GPUTimer::GPUTimer()
	: impl(std::make_shared<GPUTimerImpl>())
{
}

void GPUTimer::begin_frame(const GraphicContextPtr &gc)
{
#if defined(WIN32) && defined(ENABLE_GPU_TIMER)
	if (gc->shader_language() != shader_hlsl)
		return;

	if (impl->unused_disjoint_queries.empty())
	{
		ID3D11Device *device = D3DTarget::get_device_handle(gc);
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		desc.MiscFlags = 0;

		ComPtr<ID3D11Query> disjoint_query;
		HRESULT result = device->CreateQuery(&desc, disjoint_query.output_variable());
		if (FAILED(result))
			throw Exception("D3D11Device.CreateQuery(D3D11_QUERY_TIMESTAMP_DISJOINT) failed");

		impl->unused_disjoint_queries.push_back(disjoint_query);
	}

	impl->frames.push_back(std::shared_ptr<GPUTimerImpl::Frame>(new GPUTimerImpl::Frame()));
	impl->frames.back()->queries.reserve(impl->unused_queries.capacity());
	impl->frames.back()->disjoint_query = impl->unused_disjoint_queries.back();
	impl->unused_disjoint_queries.pop_back();

	ID3D11DeviceContext *context = D3DTarget::get_device_context_handle(gc);
	context->Begin(impl->frames.back()->disjoint_query.get());
#endif
}

void GPUTimer::begin_time(const GraphicContextPtr &gc, const std::string &name)
{
#if defined(WIN32) && defined(ENABLE_GPU_TIMER)
	if (gc->shader_language() != shader_hlsl)
		return;

	impl->frames.back()->names.push_back(name);
	impl->timestamp(gc);
#endif
}

void GPUTimer::end_time(const GraphicContextPtr &gc)
{
#if defined(WIN32) && defined(ENABLE_GPU_TIMER)
	if (gc->shader_language() != shader_hlsl)
		return;

	impl->timestamp(gc);
#endif
}

void GPUTimer::end_frame(const GraphicContextPtr &gc)
{
#if defined(WIN32) && defined(ENABLE_GPU_TIMER)
	if (gc->shader_language() != shader_hlsl)
		return;

	ID3D11DeviceContext *context = D3DTarget::get_device_context_handle(gc);
	context->End(impl->frames.back()->disjoint_query.get());
#endif
}

std::vector<GPUTimer::Result> GPUTimer::get_results(const GraphicContextPtr &gc)
{
#if defined(WIN32) && defined(ENABLE_GPU_TIMER)
	if (gc->shader_language() != shader_hlsl)
		return std::vector<GPUTimer::Result>();

	ID3D11DeviceContext *context = D3DTarget::get_device_context_handle(gc);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint_data;
	if (context->GetData(impl->frames.front()->disjoint_query.get(), &disjoint_data, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0) != S_OK)
		return impl->last_results;

	std::vector<Result> results;
	for (size_t i = 0; i < impl->frames.front()->names.size(); i++)
	{
		UINT64 start = 0;
		if (context->GetData(impl->frames.front()->queries[i * 2].get(), &start, sizeof(UINT64), 0) != S_OK)
			return impl->last_results;

		UINT64 end = 0;
		if (context->GetData(impl->frames.front()->queries[i * 2 + 1].get(), &end, sizeof(UINT64), 0) != S_OK)
			return impl->last_results;

		float time_elapsed = (float)((end - start) / (double)disjoint_data.Frequency);
		results.push_back(Result(impl->frames.front()->names[i], time_elapsed));
	}
	impl->last_results = results;

	impl->unused_queries.insert(impl->unused_queries.end(), impl->frames.front()->queries.begin(), impl->frames.front()->queries.end());
	impl->unused_disjoint_queries.push_back(impl->frames.front()->disjoint_query);
	impl->frames.erase(impl->frames.begin());

	return results;
#else
	return std::vector<GPUTimer::Result>();
#endif
}

#if defined(WIN32) && defined(ENABLE_GPU_TIMER)
void GPUTimerImpl::timestamp(const GraphicContextPtr &gc)
{
	if (gc->shader_language() != shader_hlsl)
		return;

	if (unused_queries.empty())
	{
		ID3D11Device *device = D3DTarget::get_device_handle(gc);
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_TIMESTAMP;
		desc.MiscFlags = 0;

		ComPtr<ID3D11Query> query;
		HRESULT result = device->CreateQuery(&desc, query.output_variable());
		if (FAILED(result))
			throw Exception("D3D11Device.CreateQuery(D3D11_QUERY_TIMESTAMP) failed");

		unused_queries.push_back(query);
	}

	frames.back()->queries.push_back(unused_queries.back());
	unused_queries.pop_back();

	ID3D11DeviceContext *context = D3DTarget::get_device_context_handle(gc);
	context->End(frames.back()->queries.back().get());
}
#endif
