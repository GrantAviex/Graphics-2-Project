#pragma pack_matrix( row_major )

TextureCube baseTexture : register(t0); // first texture

SamplerState filters[2] : register(s0); //0 = clamp 1 = wrap

cbuffer TIME : register(b0)
{
	float4 TotalTimeElapsed;
};
cbuffer directionalLightInfo : register(b1)
{
	float4 directionalLightColor;
	float4 directionalLightDirection;
};

float4 main(float3 uvwOut : UVW, float3 nrm : NORMAL) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filters[0], uvwOut);
	//uvwOut.x = uvwOut.x + sin(uvwOut.y * 200 + (TotalTimeElapsed.x * 52)) * 0.005 * (uvwOut.y - 1);
	float4 dayTime = float4(1, 1, 1, 1);
	float dirLightRatio = (dot(-directionalLightDirection, dayTime) + 2) / 3;
	float4 dirLightResult = dirLightRatio * directionalLightColor * baseColor;

	return dirLightResult;
	//return baseTexture.Sample(filters[0], uvwOut);
}