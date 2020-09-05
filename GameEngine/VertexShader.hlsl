struct VS_INPUT
{
	float4 pos0: POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float4 color: COLOR0;
	float tex:TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 pos: SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: TEXCOORD1;
	float4 color:COLOR0;
	float tex:TEXCOORD2;
	float4 worldpos: POSITION;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_light_direction;
	float4 m_camera_position;
	float m_time;
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//WATER WAVES
	if (input.tex == 0)
	{
		input.pos0.y = input.pos0.y -0.1f+(0.1f* sin(m_time+input.pos0.x+input.pos0.z));
	}

	//WORLD SPACE
	output.worldpos = mul(input.pos0, m_world);
	output.direction_to_camera = normalize(output.worldpos - m_camera_position.xyz);

	//VIEW SPACE
	output.pos = mul(output.worldpos, m_view);

	//SCREEN SPACE
	output.pos = mul(output.pos, m_proj);
	output.texcoord = input.texcoord;
	output.normal = input.normal;
	output.color = input.color;
	output.tex = input.tex;
		
	return output;
}