Texture2D Texture1: register(t0);
Texture2D Texture2: register(t1);
Texture2D Texture3: register(t2);
Texture2D Texture4: register(t3);

sampler TextureSampler: register(s0);

struct PS_INPUT
{
	float4 pos:SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 direction_to_camera: TEXCOORD1;
	float4 color:COLOR0;
	float tex:TEXCOORD2;
	float4 worldpos: POSITION;
};

cbuffer constant:  register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_light_direction;
	float4 m_camera_position;
	float m_time;
}

float4 main(PS_INPUT input) : SV_TARGET
{
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

	float ks = 1.0; //specular reflection
	float3 is = float3(1.0, 1.0, 1.0); //specular light source
	float3 reflected_light = reflect(m_light_direction.xyz, input.normal);
	float shininess = 200.0;
	float amount_specular_light = pow(max(0.0, dot(reflected_light,input.direction_to_camera)),shininess);

	float specular_light = ks * amount_specular_light * is;

	if(input.tex>0)specular_light = 0; //specular on water only

	float3 total_light=ambient_light + diffuse_light + specular_light;
	float3 textured_light1;
	float3 textured_light2;
	float3 textured_light3;
	float3 textured_light4;
	
	textured_light1 = total_light * Texture1.Sample(TextureSampler, input.texcoord);
	textured_light2 = total_light * Texture2.Sample(TextureSampler, input.texcoord);
	textured_light3 = total_light * Texture3.Sample(TextureSampler, input.texcoord);
	textured_light4 = total_light * Texture4.Sample(TextureSampler, input.texcoord/16);

	float level = input.worldpos.y;
	
	float4 f4_light;
	float index;

	if(level<18)
	{
		index = (level - 16)/ 2;
		f4_light = ((1-index) * float4(textured_light1, 1.0) + index* float4(textured_light2, 1.0));
	}
	else
		if (level< 26)
		{
			index = (level - 18) / 8;
			f4_light = ((1-index) * float4(textured_light2, 1.0) + index * float4(textured_light3, 1.0));
		}
		else
		{
			index = (level - 26) / 14;
			f4_light = ((1-index) * float4(textured_light3, 1.0) + index * float4(textured_light4, 1.0));
		}
		
	return f4_light; 
}