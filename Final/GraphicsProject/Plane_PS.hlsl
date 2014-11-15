texture2D baseTexture : register(t0); // first texture

//texture2D detailTexture : register(t1); // second texture

SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(float2 baseUV : UV, float4 modulate : COLOR) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filters[0], baseUV) * modulate; // get base color
	return baseColor;
}
