cbuffer cbPerObject
{
	matrix WVP;
};

struct Input 
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct Output
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

Output main(Input input)
{
	Output output;

	output.position = mul(float4(input.position,1.0f), WVP);
	//output.position = float4(output.position.x, output.position.y, output.position.z, 1);
	output.color = input.color;

	return output;
}