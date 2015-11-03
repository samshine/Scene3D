namespace { const char *accumulate_lpv_hlsl() { return R"shaderend(

#define TILE_SIZE 16

[numthreads(TILE_SIZE,TILE_SIZE,1)]
void main(uint3 globalThreadID : SV_DispatchThreadID)
{
}

)shaderend"; } }
