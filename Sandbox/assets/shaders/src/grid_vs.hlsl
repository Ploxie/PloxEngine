#include "bindings.hlsli"

struct VSOutput
{
	float4 position : SV_Position;
};

struct Constants
{
	float4x4 modelMatrix;
	float4x4 viewProjectionMatrix;
	float4 thinLineColor;
	float4 thickLineColor;
	float3 cameraPos;
	float cellSize;
	float3 gridNormal;
	float gridSize;
};

//ConstantBuffer<Constants> g_Constants : REGISTER_CBV(0, 0, 0);

VSOutput main(uint vertexID : SV_VertexID)
{
	uint positionIndex = vertexID % 6;

	float2 positions[6] =
	{
		float2(0.0, 0.0),
		float2(1.0, 0.0),
		float2(1.0, 1.0),
		float2(0.0, 1.0),
		float2(0.0, 0.0),
		float2(1.0, 0.0),
	};

	VSOutput output = (VSOutput)0;
	//output.gridPos = positions[positionIndex] * 200.0f;
	//float2 objectSpacePos = output.gridPos - 200.0f * 0.5f;
	//output.worldSpacePos = mul(g_Constants.modelMatrix, float4(objectSpacePos.x, 0.0f, objectSpacePos.y, 1.0f)).xyz;
	//output.position = mul(g_Constants.viewProjectionMatrix, float4(output.worldSpacePos, 1.0f));
	output.position = float4(positions[positionIndex] * 200.0f, 0.5f, 0.5f);

	return output;
}