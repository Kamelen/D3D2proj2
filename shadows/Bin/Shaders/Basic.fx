//--------------------------------------------------------------------------------------
// Basic.fx
// Direct3D 11 Shader Model 4.0 Demo
// Copyright (c) Stefan Petersson, 2011
//--------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

Texture2D blendMap;
Texture2D text1;
Texture2D text2;
Texture2D text3;

SamplerState sState
{
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer PointLight
{
	float4 l_lightPos;
	float4 l_diffuseColor;
	float4 l_ambientColor;
	float4 l_specularColor;
	float4 cameraPos;
	float l_rad;
};


cbuffer EveryFrame
{
	matrix WVP;
	matrix W;
};

struct VSIn
{
	float3 Pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXTCOORD;
};

struct PSSceneIn
{
	float4 Pos  : SV_Position;
	float4 worldPos : worldPos;
	float3 normal : TEXTCOORD0;
	float2 uv : TEXTCOORD2;
};

//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
RasterizerState NoCulling
{
	//CullMode = NONE;
};
RasterizerState wire
{
	CullMode = NONE;
	FillMode = Wireframe;
};



//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;

	output.Pos = mul(float4(input.Pos, 1), WVP);
	output.worldPos =  mul(float4(input.Pos, 1), W);
	
	output.normal = mul(input.normal, W);
	output.normal = normalize(output.normal);
	output.uv = input.uv;
		
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSTerrain(PSSceneIn input) : SV_Target
{	
	float4 finalColor = (0,0,0,0);
	float4 blend = (0,0,0,0);
	float4 w1 = (0,0,0,0);
	float4 w2 = (0,0,0,0);
	float4 w3 = (0,0,0,0);

	//light
	float4 textureColor;
    float lightIntensity;
	float3 lightVec;
	float specFactor;
	float angle;
	float3 reflectVec;
	float3 viewDir;
	//light

	 
	blend = blendMap.Sample(sState, input.uv);

	w1 = blend.x/(blend.x + blend.y + blend.z);
	w2 = blend.y/(blend.x + blend.y + blend.z);
	w3 = blend.z/(blend.x + blend.y + blend.z);

	textureColor = text1.Sample(sState,input.uv)*w1 + text2.Sample(sState,input.uv)*w2 + text3.Sample(sState,input.uv)*w3;
	finalColor = l_ambientColor;


	//light calculations

	lightVec = float3(l_lightPos.xyz) - float3(input.worldPos.xyz);
	lightIntensity = saturate(1 - (length(lightVec) / l_rad));
	angle = max(0.0f , dot(input.normal, normalize(lightVec)));

	if(angle > 0.0f)
	{
		finalColor += (l_diffuseColor * lightIntensity);

		viewDir = normalize(cameraPos.xyz - input.worldPos.xyz);
		reflectVec = normalize(reflect(-lightVec, input.normal));
		specFactor = pow(max(0.0f , dot(reflectVec,viewDir)), 16);
		
	}
	else
	{	
		lightVec = -(float3(l_lightPos.xyz) - float3(input.worldPos.xyz));
		angle = max(0.0f , dot(input.normal, normalize(lightVec)));

		if(angle > 0.0f)
		{
			finalColor += (l_diffuseColor * lightIntensity);

			viewDir = normalize(cameraPos.xyz - input.worldPos.xyz);
			reflectVec = normalize(reflect(-lightVec, input.normal));
			specFactor = pow(max(0.0f , dot(reflectVec,viewDir)), 16);
		
		}
	}
		
	finalColor = finalColor * textureColor;
	finalColor = saturate(finalColor + (specFactor * l_specularColor * angle) * lightIntensity);
	
	return finalColor;

}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSWire(PSSceneIn input) : SV_Target
{
	//wireFrameCol
	return float4(input.normal.x,0,input.normal.z,0);
	//return float4(0,1,0,1);
}

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
        SetPixelShader( CompileShader( ps_4_0, PSTerrain() ) );
	    
	    SetRasterizerState( NoCulling );
    } 
	pass p1
	{
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSWire() ) );
	    
	    SetRasterizerState( wire );
	}
}
