Texture2D normalMap          : register(t0);
Texture2D diffuseAlbedoMap   : register(t1);
Texture2D lightMap           : register(t2);

struct VSIn
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXTCOORD;
};

struct PSIn
{
	float4 pos : SV_Position;
};

PSIn VSScene(VSIn input)
{	
	PSIn output =(PSIn)0;
	output.pos = float4(input.pos, 1);
	return output;
}


float4 PSScene(PSIn input) : SV_Target
{
	int3 sampleIndices = int3(input.pos.xyz);

	//float3 light = lightMap.Load(sampleIndices).xyz;
	float3 diffuse = diffuseAlbedoMap.Load(sampleIndices).xyz;
	float3 normal = normalMap.Load(sampleIndices).xyz;
	//float3 sum = saturate((diffuse) * light);
	
	return float4(diffuse, 1);
	return float4(normal, 1.0f);
	//return float4(light, 1.0f);
	//return float4(sum, 1.0f);
}


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
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader(vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    
	    SetRasterizerState( NoCulling );
    } 
}

