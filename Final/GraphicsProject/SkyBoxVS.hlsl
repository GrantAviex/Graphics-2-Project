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
	float4 Pos : SV_POSITION;
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
	OUTPUT_VERTEX output = (OUTPUT_VERTEX)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	float4 localH = float4(input.coordinate, 1);
	localH = mul(localH, viewMatrix).xyzw;
	localH = mul(localH, projectionMatrix).xyww;
	//localH = mul(localH, viewMatrix).xyww;
	output.Pos = localH;
	output.uvwOut = input.coordinate;

	return output;
}