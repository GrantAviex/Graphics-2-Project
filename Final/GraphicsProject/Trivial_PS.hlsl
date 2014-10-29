TextureCube baseTexture : register(t0); // first texture
SamplerState filters[2] : register(s0); //0 = clamp 1 = wrap

cbuffer ambLightInfo : register(b0)
{
	float4 ambLightColor;
};
cbuffer pointLightInfo : register(b2)
{
	float4 pointLightPos;
	float4 pointLightColor;
};

cbuffer spotLightInfo : register(b3)
{
	float4 spotLightPos;
	float4 spotLightColor;
	float4 spotLightDirection;
	float4 spotLightInnerConeRatio;
	float4 spotLightOuterConeRatio;
};

cbuffer directionalLightInfo : register(b1)
{
	float4 directionalLightColor;
	float4 directionalLightDirection;
};

float4 main(float3 uvw : UVW, float3 surfacePos : SURFACE_POSITION, float3 normal : NORMAL) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filters[0], uvw);
	//float4 baseColor = baseTexture.Sample(filters[0], uvw.xy);
	//clip(baseColor.w < 0.65f ? -1 : 1);
	float dirLightRatio = saturate(dot(-directionalLightDirection, normal));
	float4 dirLightResult = dirLightRatio * directionalLightColor * baseColor;
		float4 ambLightResult = ambLightColor * baseColor;
		float4 Result = dirLightResult + ambLightResult;
	return Result;
	//	return dirLightResult;
	// lightPos.w = radius
	//float3 lightDir = normalize(lightPos.xyz - surfacePos);
	//float lightRatio = saturate(dot(directionalLightDirection, normal));
	//float attenuation = 1.0 - saturate(length(lightPos.xyz - surfacePos) / lightPos.w);
	//float4 pointLightResult = saturate(attenuation * lightRatio) * lightColor * baseTexture.Sample(filters[0], uv);
	//
	//		//Spot light
	//		float3 spotLightDir = normalize(spotLightPos.xyz - surfacePos);
	//		float surfaceRatio = saturate(dot(-spotLightDir, normalize(spotLightDirection.xyz)));
	//	
	//	float spotLightRatio = saturate(dot(spotLightDir, normal.xyz));
	//	
	//	float spotAttenuation = 1.0 - saturate(
	//		(spotLightInnerConeRatio.x - surfaceRatio) /
	//		(spotLightInnerConeRatio.x - spotLightOuterConeRatio.x));
	//	
	//	float4 spotLightResult = saturate(spotAttenuation * spotLightRatio) * spotLightColor * baseTexture.Sample(filters[0], uv);
	//	
	//		return pointLightResult + spotLightResult;
	 // get base color

}