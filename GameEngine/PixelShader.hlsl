Texture2D Texture: register(t0);
sampler TextureSampler: register(s0);

struct PS_INPUT
{
	float4 pos:SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
};

cbuffer constant:  register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_light_direction;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//return Texture.Sample(TextureSampler, input.texcoord);

	//AMBIENT LIGHT
	
	float ka = 0.1;  //ambient reflection
	float3 ia = float3(1.0,1.0,1.0); //ambient light source
	float3 ambient_light = ka * ia; //ambient component

	//DIFFUSE LIGHT

	float kd = 0.8; //diffuse reflection
	float3 id = float3(1.0, 1.0, 1.0); //diffuse light source
	float amount_diffuse_light = max(0.0,dot(m_light_direction.xyz, input.normal));
	float3 diffuse_light = kd * id * amount_diffuse_light;

	float3 total_light = ambient_light+diffuse_light;

	return float4(total_light, 1.0);


}