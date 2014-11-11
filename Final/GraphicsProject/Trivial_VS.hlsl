#pragma pack_matrix(row_major)
struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float3 uvwIn : UVW;
	float3 nrm : NORMAL;
};

struct OUTPUT_VERTEX
{
	float3 uvwOut : UVW;
	float3 surfacePos : SURFACE_POSITION;
	float3 normal : NORMAL;
	float4 projectedCoordinate : SV_POSITION;
};

// TODO: PART 3 STEP 2a
cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}
OUTPUT_VERTEX main(INPUT_VERTEX input)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 localH = float4(input.coordinate, 1);





	localH = mul(localH, worldMatrix);
	sendToRasterizer.surfacePos = localH.xyz;
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	sendToRasterizer.projectedCoordinate = localH;
	sendToRasterizer.uvwOut = input.uvwIn;
	sendToRasterizer.normal = input.nrm;

	return sendToRasterizer;
}