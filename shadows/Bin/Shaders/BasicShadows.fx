
SamplerState shadowMapSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border;
	AddressV = Border;

	BorderColor = float4(1,1,1,1);
};

SamplerState textureSampler
{
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState cubeSampler
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
	AddressU = WRAP;
	AddressV = WRAP;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

cbuffer EveryFrame
{
	matrix WVP;
	matrix W;
	matrix LightWVP;

	static const float SHADOW_EPSILON = 0.000001f;
	float SMAP_SIZE;

	float texTrans;
	float4 cameraPos;
	bool useCubeMap;
	bool useBlending;
};

Texture2D diffuseMap1;
Texture2D diffuseMap2;
Texture2D diffuseMap3;

Texture2D blendMap;

Texture2D shadowMap;
TextureCube cubeMap;

struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Normal:NORMAL;
	float2 tex:TEXTCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD1;
	float4 posLightH : TEXCOORD3;
	float4 PosW : POSITION;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output;
	output.Pos = mul(float4(input.Pos,1.0f), WVP);
	output.normal = mul(input.Normal,W);    
	output.tex = input.tex;
	output.posLightH = mul(float4(input.Pos,1.0f), LightWVP);
	output.PosW = mul(float4(input.Pos,1.0f), W);
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PS_INPUT input) : SV_Target
{	

	float4 texColor = (0,0,0,0);
	float4 finalColor = (0,0,0,0);
	float4 blend = (0,0,0,0);
	float4 w1 = (0,0,0,0);
	float4 w2 = (0,0,0,0);
	float4 w3 = (0,0,0,0);


	if(texTrans != 5)
	{
		input.tex.x += texTrans;	
	}
	float3 toEye = normalize(cameraPos.xyz - input.PosW.xyz);
	float3 incident = -toEye;
	float3 reflectionVector = reflect(incident, normalize(input.normal));
	float4 reflectionsColor = cubeMap.Sample(cubeSampler, normalize(reflectionVector));
	

	//get the blending of the textures if it is to be used
	if(useBlending == true)
	{
		
		blend = blendMap.Sample(textureSampler,input.tex);
		w1 = blend.x/(blend.x + blend.y + blend.z);
		w2 = blend.y/(blend.x + blend.y + blend.z);
		w3 = blend.z/(blend.x + blend.y + blend.z);

		texColor = diffuseMap1.Sample(textureSampler,input.tex)*w1 + diffuseMap2.Sample(textureSampler,input.tex)*w2 + diffuseMap3.Sample(textureSampler,input.tex)*w3;
		return texColor;
	}
	else
	{
		texColor = diffuseMap1.Sample(textureSampler, input.tex);
	}
	
	if(useCubeMap == true)
	{
		return reflectionsColor;	
	}

	return float4(texColor);
}


technique11 BasicShadowTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    SetRasterizerState( NoCulling );
    }
}