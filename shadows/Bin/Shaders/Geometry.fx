Texture2D texture1   : register(t0);
Texture2D texture2   : register(t1);
Texture2D texture3   : register(t2);
Texture2D blendMap   : register(t3);
Texture2D shadowMap  : register(t4);

TextureCube cubeMap	  : register(t5);

SamplerState anisoSampler
{
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState cubeSampler
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState shadowMapSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border;
	AddressV = Border;

	BorderColor = float4(1,1,1,1);
};

float specularIntensity = 0.8f;
float specularPower     = 0.5f;

cbuffer EveryFrame
{
	matrix world;
	matrix view;
	matrix proj;
	matrix lightWVP;

	static const float SHADOW_EPSILON = 0.000001f;
	float SMAP_SIZE;
	float texTrans;

	float4 cameraPos;
	bool useCubeMap;
	bool useBlendMap;
	bool useShadowMap;
};

//--------------------------
//input for VSScene
//--------------------------
struct VSIn
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXTCOORD;
};

struct PSIn
{
	float4 posCS   : SV_Position;
	float4 posW    : POSITION;
	float4 normalW : NORMAL;
	float4 posLightH : TEXTCOORD0;
	float2 uv      : TEXTCOORD1;
	float2 depth   : DEPTH;
};

struct PSOut
{
	float4 diffuseAlbedo  : SV_TARGET0;
	float4 normal         : SV_TARGET1;
	float4 light          : SV_TARGET2;
	float4 depth          : SV_TARGET3;
};

float computeShadows(float4 posLightH)
{

	//Project the texture coords and scale/offset to [0, 1].
	posLightH.xy /= posLightH.w;
	
	//Compute shadow map tex coord
	float2 smTex = float2(0.5f*posLightH.x, -0.5f*posLightH.y) + 0.5f;
	
	//Compute pixel depth for shadowing.
	float depth = posLightH.z / posLightH.w;
	
	// 2x2 percentage closest filter.
	float dx = 1.0f / SMAP_SIZE;
	float s0 = (shadowMap.Sample(shadowMapSampler, smTex).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s1 = (shadowMap.Sample(shadowMapSampler, smTex + float2(dx, 0.0f)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s2 = (shadowMap.Sample(shadowMapSampler, smTex + float2(0.0f, dx)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s3 = (shadowMap.Sample(shadowMapSampler, smTex + float2(dx, dx)).r   + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	
	// Transform to texel space
	float2 texelPos = smTex * SMAP_SIZE;
	
	// Determine the lerp amounts.           
	float2 lerps = frac( texelPos );
	float shadowCoeff = lerp( lerp( s0, s1, lerps.x ),lerp( s2, s3, lerps.x ),lerps.y );
	return shadowCoeff;
};

PSIn VSScene(VSIn input)
{
	PSIn output = (PSIn)0;
	output.posCS = mul(float4(input.pos, 1) , world);
	output.posCS = mul(output.posCS, mul(view, proj));
	output.posW  = mul(float4(input.pos, 1) , world);
	output.posLightH = mul(float4(input.pos,1.0f), lightWVP);
	output.normalW = normalize(mul(float4(input.normal, 0) , world));
	output.uv = input.uv;
	output.depth = float2(output.posCS.z , output.posCS.w);
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
PSOut PSScene(PSIn input)
{
	PSOut output = (PSOut)0;
	float4 diffuseAlbedo = (0,0,0,0);

	float4 texColor = (0,0,0,0);
	float4 blend = (0,0,0,0);
	float4 w1 = (0,0,0,0);
	float4 w2 = (0,0,0,0);
	float4 w3 = (0,0,0,0);

	input.uv.x += texTrans;

	float4 normalW = float4( 0.5f * (normalize(input.normalW).rgb + 1.0f), specularPower);
	output.normal = normalW;

	diffuseAlbedo = texture1.Sample( anisoSampler , input.uv);

	if(useCubeMap == true)
	{
		float3 toEye = normalize(cameraPos.xyz - input.posW.xyz);
		float3 incident = -toEye;
		float3 reflectionVector = reflect(incident, normalize(input.normalW));
		float4 reflectionsColor = cubeMap.Sample(cubeSampler, normalize(reflectionVector));

		diffuseAlbedo = reflectionsColor;
	}
	
	if(useBlendMap == true)
	{
		blend = blendMap.Sample(anisoSampler,input.uv);
		w1 = blend.x/(blend.x + blend.y + blend.z);
		w2 = blend.y/(blend.x + blend.y + blend.z);
		w3 = blend.z/(blend.x + blend.y + blend.z);

		diffuseAlbedo = texture1.Sample(anisoSampler,input.uv)*w1 + texture2.Sample(anisoSampler,input.uv)*w2 + texture3.Sample(anisoSampler,input.uv)*w3;
	}

	if(useShadowMap == true)
	{
		diffuseAlbedo = computeShadows(input.posLightH) * diffuseAlbedo;
	}

	output.light = 0.2f;
	output.light.a = 0;
	output.depth = input.depth.x / input.depth.y;
	output.diffuseAlbedo = diffuseAlbedo;
	output.diffuseAlbedo.a = specularIntensity;
	return output;
}

RasterizerState NoCulling
{
	CullMode = BACK;
};
RasterizerState wire
{
	CullMode = NONE;
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
	    
	    SetRasterizerState( NoCulling );
    }

}




