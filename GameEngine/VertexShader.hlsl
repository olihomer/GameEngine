struct VS_INPUT
{
	float4 pos0: POSITION;
	float3 color0: COLOR;
};

struct VS_OUTPUT
{
	float4 pos: SV_POSITION;
	float3 color: COLOR;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	unsigned int m_time;

}


VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//WORLD SPACE
	output.pos = mul(input.pos0, m_world);

	//VIEW SPACE
	output.pos = mul(output.pos, m_view);

	//SCREEN SPACE
	output.pos = mul(output.pos, m_proj);

	output.color = input.color0; //lerp(input.color0, input.color1, sin(m_time / 500.0f) + 1.0f / 2.0f);
		
	return output;
}