#pragma pack_matrix( row_major )

TextureCube baseTexture : register(t0); // first texture

SamplerState filters[2] : register(s0); //0 = clamp 1 = wrap

cbuffer TIME : register(b0)
{
	float4 TotalTimeElapsed;
};

float4 main(float3 uvwOut : UVW) : SV_TARGET
{
	uvwOut.x = uvwOut.x + sin(uvwOut.y * 200 + (TotalTimeElapsed.x * 52)) * 0.005 * (uvwOut.y - 1);

	return 	baseTexture.Sample(filters[0], uvwOut);
	//return baseTexture.Sample(filters[0], uvwOut);
}