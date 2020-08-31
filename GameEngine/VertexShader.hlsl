struct VS_INPUT
{
	float4 pos0: POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float4 color: COLOR0;
};

struct VS_OUTPUT
{
	float4 pos: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: TEXCOORD1;
	float4 color:COLOR0;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_light_direction;
	float4 m_camera_position;
}


VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//WORLD SPACE
	output.pos = mul(input.pos0, m_world);
	output.direction_to_camera = normalize(output.pos - m_camera_position.xyz);

	//VIEW SPACE
	output.pos = mul(output.pos, m_view);

	//SCREEN SPACE
	output.pos = mul(output.pos, m_proj);
	output.texcoord = input.texcoord;
	output.normal = input.normal;
	output.color = input.color;
		
	return output;
}