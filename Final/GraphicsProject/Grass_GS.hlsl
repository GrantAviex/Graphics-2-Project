#pragma pack_matrix(row_major)
struct GSOutput
{
	float4 color : COLOR;
	float4 posH : SV_POSITION;
};
cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}
cbuffer GRASS : register(b2)
{
	float2 grassSize;
}
// convert each incoming world-space line segment into a projected triangle. 
[maxvertexcount(3)] // max vertex data to be output (limit 1024 total scalars)
void main(point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	// red green and blue vertex
	GSOutput verts[3] =
	{
		float4(1, 0, 0, 1), float4(0, 0, 0, 1),
		float4(0, 1, 0, 1), float4(0, 0, 0, 1),
		float4(0, 0, 1, 1), float4(0, 0, 0, 1)
	};
	// bottom left
	verts[0].posH.xyz = input[0].xyz;
	verts[0].posH.x -= grassSize.x;
	// bottom right
	verts[2].posH.xyz = input[0].xyz;
	verts[2].posH.x += grassSize.x;
	// top center
	verts[1].posH.xyz = input[0].xyz;
	verts[1].posH.y += grassSize.y;
	// prep triangle for rasterization
	float4x4 mVP = mul(viewMatrix, projectionMatrix);
		for (uint i = 0; i < 3; ++i)
		{
			verts[i].posH = mul(verts[i].posH, worldMatrix);
			verts[i].posH = mul(verts[i].posH, mVP);
		}
	// send verts to the rasterizer
	output.Append(verts[0]);
	output.Append(verts[1]);
	output.Append(verts[2]);
	// do not connect to other triangles
	output.RestartStrip();
}
