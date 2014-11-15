#pragma pack_matrix(row_major)
struct GSOutput
{
	float2 uv : UV;
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
// convert each incoming world-space line segment into a projected triangle. 
[maxvertexcount(4)] // max vertex data to be output (limit 1024 total scalars)
void main(point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	// red green and blue vertex
	GSOutput verts[4] = 
	{
		float2(0, 0), float4(1, 1, 1, 1), float4(0, 0, 0, 1),
		float2(1, 0), float4(1, 1, 1, 1), float4(0, 0, 0, 1),
		float2(0, 1), float4(1, 1, 1, 1), float4(0, 0, 0, 1),
		float2(1, 1), float4(1, 1, 1, 1), float4(0, 0, 0, 1)

	};
	
	// bottom left
	verts[0].posH.xyz = input[0].xyz;
	verts[0].posH.x -= 2.5f;
	verts[0].posH.z += 2.5f;
	verts[1].posH.xyz = input[0].xyz;
	verts[1].posH.x += 2.5f;
	verts[1].posH.z += 2.5f;
	verts[2].posH.xyz = input[0].xyz;
	verts[2].posH.x -= 2.5f;
	verts[2].posH.z -= 2.5f;
	verts[3].posH.xyz = input[0].xyz;
	verts[3].posH.x += 2.5f;
	verts[3].posH.z -= 2.5f;

	// prep triangle for rasterization
	float4x4 mVP = mul(viewMatrix, projectionMatrix);
		for (uint i = 0; i < 4; ++i)
		{
			verts[i].posH = mul(verts[i].posH, worldMatrix);
			verts[i].posH = mul(verts[i].posH, mVP);
		}
			//send verts to the rasterizer
	output.Append(verts[0]);
	output.Append(verts[1]);
	output.Append(verts[2]);
	output.Append(verts[3]);
	// do not connect to other triangles
}
