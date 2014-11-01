//TextureCube baseTexture : register(t0); // first texture
texture2D baseTexture : register(t0); // first texture
SamplerState filters[2] : register(s0); //0 = clamp 1 = wrap

cbuffer ambLightInfo : register(b0)
{
	float4 ambLightColor;
};
cbuffer directionalLightInfo : register(b1)
{
	float4 directionalLightColor;
	float4 directionalLightDirection;
};
cbuffer pointLightInfo : register(b2)
{
	float4 pointLightPos;
	float4 pointLightColor;
};

cbuffer spotLightInfo : register(b3)
{
	float4 spotLightPos[6];
	float4 spotLightColor[6];
	float4 spotLightDirection[6];
	float4 spotLightInnerConeRatio[6];
	float4 spotLightOuterConeRatio[6];
};


float4 main(float3 uvw : UVW, float3 surfacePos : SURFACE_POSITION, float3 normal : NORMAL) : SV_TARGET
{
//BaseColor
	float4 baseColor = baseTexture.Sample(filters[0], uvw.xy);
	clip(baseColor.w < 0.65f ? -1 : 1);

//Directional Light
	float dirLightRatio = saturate(dot(-directionalLightDirection, normal));
	float4 dirLightResult = dirLightRatio * directionalLightColor * baseColor;


//Ambient Light
	float4 ambLightResult = ambLightColor * baseColor;

//Point Light
	// lightPos.w = radius
	float3 pointLightDir = normalize(pointLightPos.xyz - surfacePos);
	float pointLightRatio = saturate(dot(pointLightDir, normal));
	float pointLightAttenuation = 1.0 - saturate(length(pointLightPos.xyz - surfacePos) / pointLightPos.w);
	float4 pointLightResult = saturate(pointLightAttenuation * pointLightRatio) * pointLightColor * baseColor;

		//Spot light
		float4 Result = { 0, 0, 0, 0 };

	for (int i = 0; i < 6; i++)
	{
		float3 spotLightDir = normalize(spotLightPos[i].xyz - surfacePos);
			float surfaceRatio = saturate(dot(-spotLightDir, normalize(spotLightDirection[i].xyz)));

		float spotLightRatio = saturate(dot(spotLightDir, normal.xyz));

		float spotAttenuation = 1.0 - saturate(
			(spotLightInnerConeRatio[i].x - surfaceRatio) /
			(spotLightInnerConeRatio[i].x - spotLightOuterConeRatio[i].x));

		float4 spotLightResult = saturate(spotAttenuation * spotLightRatio) * spotLightColor[i] * baseColor;
		Result = Result + spotLightResult;
	}
		
			//		return pointLightResult + spotLightResult;
	Result = Result + dirLightResult + ambLightResult + pointLightResult;
	return Result;

}