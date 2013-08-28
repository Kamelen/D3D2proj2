Texture2D normalMap        : register(t0);
Texture2D diffuseAlbedoMap : register(t1);
Texture2D depthMap         : register(t2);

float4 cameraPos;
float lightIntensity = 5.0f;

SamplerState diffuseSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerState depthSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerState normalSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};


cbuffer EveryFrame
{
	matrix view;
	matrix proj;
	matrix invertViewProjection;
};

struct VSIn
{
	float3 pos         : POSITION;

	float3 lightPos    : LIGHTPOS;
	float3 lightColor  : LIGHTCOLOR;
	float lightRadius  : LIGHTRADIUS;

	uint instanceID    : SV_InstanceID;
};

struct PSSceneIn
{
	float4 screenPos      : SV_Position;
	float4 screenPosition : ScreenCoord;

	float4 worldLightPos  : LIGHTPOS;
	float3 lightColor     : LIGHTCOLOR;
	float  lightRadius    : LIGHTRADIUS;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;

	float3 worldPos = (input.pos * input.lightRadius) + input.lightPos;
	output.screenPos = mul(float4(worldPos , 1) , mul(view,proj));
	output.screenPosition = output.screenPos;

	output.worldLightPos = float4(input.lightPos, 1);
	output.lightColor = input.lightColor;
	output.lightRadius = input.lightRadius;
	
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{
	//räkna ut UV koordinater för sampling
	input.screenPosition.xy /= input.screenPosition.w;
	float2 uv = 0.5f * (float2(input.screenPosition.x, -input.screenPosition.y) +1 );

	//sampling av texturer
	float4 normalData = normalMap.Sample(normalSampler, uv);
	float3 normal = 2.0f * normalData.xyz - 1.0f;
	normal = -normal;
	float specularPower = normalData.a * 255;
	float specularIntensity = diffuseAlbedoMap.Sample(diffuseSampler, uv).a;
	float depth = depthMap.Sample(depthSampler, uv).r;

	//uträkning av världsposition
	float4 position;
	position.xy = input.screenPosition.xy;
	position.z = depth;
	position.w = 1.0f;
	position = mul(position , invertViewProjection);
	position /= position.w;

	//ljusberäkningar
	//--------------------------------------------------------------------------------
		//Attenuation
		float3 lightVector = input.worldLightPos.xyz - position.xyz;
		float attenuation =  saturate(1.0f - (length(lightVector)/input.lightRadius));

		//diffuseLight
		lightVector = normalize(lightVector);
		float nDL = max(0, dot(normal, lightVector));
		float3 diffuseLight = nDL * input.lightColor.rgb;

		//specularLight
		float3 directionToCamera = normalize(cameraPos - position);
		float3 reflectionVector = normalize(reflect(-lightVector, normal));
		float specularLight = specularIntensity * pow( saturate(dot(reflectionVector , directionToCamera)), specularPower);
		specularLight = 0; 

	//--------------------------------------------------------------------------------

	
	float4 finalLight = attenuation * lightIntensity * float4(diffuseLight.rgb , 1);
	finalLight.a = specularLight;
	return finalLight;

}


//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
RasterizerState LightCulling
{
	CullMode = NONE;
};
RasterizerState wire
{
	CullMode = BACK;
	FillMode = Wireframe;
};

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    
	    SetRasterizerState( LightCulling );

    } 
}
