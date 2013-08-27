Texture2D textures   : register(t0);
Texture2DArray glowMaps	  : register(t1);

SamplerState anisoSampler
{
	AddressU = WRAP;
	AddressV = WRAP;
};

float specularIntensity = 0.8f;
float specularPower     = 0.5f;

cbuffer EveryFrame
{
	matrix world;
	matrix view;
	matrix proj;
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
	float2 uv      : TEXTCOORD0;
};

struct PSOut
{
	float4 diffuseAlbedo  : SV_TARGET0;
	float4 normal         : SV_TARGET1;
};

PSIn VSScene(VSIn input)
{
	PSIn output = (PSIn)0;
	output.posCS = mul(float4(input.pos, 1) , world);
	output.posCS = mul(output.posCS, mul(view, proj));
	output.posW  = mul(float4(input.pos, 1) , world);
	output.normalW = normalize(mul(float4(input.normal, 0) , world));
	output.uv = input.uv;
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
PSOut PSScene(PSIn input)
{
	PSOut output = (PSOut)0;

	float4 diffuseAlbedo = textures.Sample( anisoSampler , input.uv);
	float4 normalW = float4( 0.5f * (normalize(input.normalW).rgb + 1.0f), specularPower);

	output.normal = normalW;
	output.diffuseAlbedo = float4(diffuseAlbedo);
	diffuseAlbedo.a = specularIntensity;

	return output;
}

RasterizerState NoCulling
{
	CullMode = NONE;
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




