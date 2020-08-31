Texture2D Texture: register(t0);
sampler TextureSampler: register(s0);

struct PS_INPUT
{
	float4 pos:SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: TEXCOORD1;
};

cbuffer constant:  register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_light_direction;
	float4 m_camera_position;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//return Texture.Sample(TextureSampler, input.texcoord);

	//AMBIENT LIGHT
	
	float ka = 0.2;  //ambient reflection
	float3 ia = float3(1.0,1.0,1.0); //ambient light source
	float3 ambient_light = ka * ia; //ambient component

	//DIFFUSE LIGHT

	float kd = 0.7; //diffuse reflection
	float3 id = float3(1.0, 1.0, 1.0); //diffuse light source
	float amount_diffuse_light = max(0.0,dot(m_light_direction.xyz, input.normal));
	float3 diffuse_light = kd * id * amount_diffuse_light; //diffuse component

	//SPECULAR LIGHT

	/*float ks = 1.0; //specular reflection
	float3 is = float3(1.0, 1.0, 1.0); //specular light source
	float3 reflected_light = reflect(m_light_direction.xyz, input.normal);
	float shininess = 200.0;
	float amount_specular_light = pow(max(0.0, dot(reflected_light,input.direction_to_camera)),shininess);

	float specular_light = ks * amount_specular_light * is;*/

	float specular_light = 0;

	float3 total_light = ambient_light+diffuse_light+specular_light;

	return float4(total_light * Texture.Sample(TextureSampler, input.texcoord), 1.0);// *Texture.Sample(TextureSampler, input.texcoord);

}