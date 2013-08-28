#include "Program.h"

program::program(void)
{
	flyMode = true;
	texTrans = 0;
}


program::~program(void)
{
	SAFE_DELETE(map);
	SAFE_DELETE(cam);
	SAFE_DELETE(lightCam);
	SAFE_DELETE(input);
	SAFE_DELETE(pSys);
	SAFE_DELETE(billboardTest);
	SAFE_DELETE(cubeMap);
	SAFE_DELETE(shadowMap);
	SAFE_DELETE(objReader);
} 

bool program::initiate(HINSTANCE hInstance, int nCmdShow)
{
	//Initiate all program resources
	if(FAILED(WinHandler::initWindow(hInstance,nCmdShow)))
	{
		return false;
	}
	if(FAILED(D3D11Handler::InitDirect3D()))
	{
		return false;
	}
	string textureNames[] = {"texture01.dds", "texture02.dds","texture03.dds"};
	map = new Terrain("cloudy.Raw",256,256,0.5,0,this->device,this->deviceContext,textureNames,"terrainblend.png");
	
	D3DXVECTOR3 position = map->getTerrainPos(254,254);
	D3DXVECTOR3 look = D3DXVECTOR3(0,0,1);
	D3DXVECTOR3 right = D3DXVECTOR3(1,0,0);
	D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);

	cam = new Camera(position, look, right, up);

	position = D3DXVECTOR3(1, 200, 0);
	look = D3DXVECTOR3(0,-1,0);
	right = D3DXVECTOR3(0,0,1);
	up = D3DXVECTOR3(1,0,0);

	lightCam = new Camera(position, look, right, up);
	
	
	input = new Input();
	pSys = new ParticleSystem();

	pSys->addRain(this->deviceContext,this->device,"Rain.fx",D3DXVECTOR3(0,150,0),D3DXVECTOR3(0,-1,0),100,100,1,5000,100,1);
	//-14 152 68
	shadowMap = new ShadowMap(this->device, this->deviceContext,4024, 4024);
	cubeMap = new Cubemap(1024,D3DXVECTOR3(-1,100,0),this->device);
	// -------------------------OBJREADER TEST ---------------------------------
	objReader = new OBJReader();												//
	mesh = new Vertex[6];

	mesh[0] = Vertex(D3DXVECTOR3(-1,1,0),D3DXVECTOR3(0,0,-1),D3DXVECTOR2(0,0));
	mesh[1] = Vertex(D3DXVECTOR3(1,1,0),D3DXVECTOR3(0,0,-1),D3DXVECTOR2(1,0));
	mesh[2] = Vertex(D3DXVECTOR3(-1,-1,0),D3DXVECTOR3(0,0,-1),D3DXVECTOR2(0,1));
	
	mesh[3] = Vertex(D3DXVECTOR3(-1,-1,0),D3DXVECTOR3(0,0,-1),D3DXVECTOR2(0,1));
	mesh[4] = Vertex(D3DXVECTOR3(1,1,0),D3DXVECTOR3(0,0,-1),D3DXVECTOR2(1,0));
	mesh[5] = Vertex(D3DXVECTOR3(1,-1,0),D3DXVECTOR3(0,0,-1),D3DXVECTOR2(1,1));
	
	billboardTest = new Billboard(mesh,6,D3DXVECTOR3(0,180,0),this->device,this->deviceContext);

	D3DXMATRIX world, translate;
	D3DXMatrixScaling(&world,4,4,4);
	D3DXMatrixTranslation(&translate,-1,100,0);

	this->objects.push_back(D3DObject(objReader->getOBJfromFile("coolaModellerFixed/sphere.obj", this->nrOfVerts), this->nrOfVerts, (world * translate)));
	
	for(int i = 0; i < this->objects.size(); i++)
	{
		this->objects.at(i).initBuffer(this->device,this->deviceContext);
	}
	
	return true;
}

void program::run(float deltaTime)
{
	int gameState;
	
	
	gameState = update(deltaTime);

	render(deltaTime);
}

int program::update(float deltaTime)
{
	//walk
	D3DXVECTOR3 pos = this->cam->getPosition();

	if(!flyMode)
	{
		pos.y = this->map->getY((int)pos.x,(int)pos.z); //följer terrängs Y coord
	}
	
	this->cam->setPos(D3DXVECTOR3((float)pos.x,(float)pos.y,(float)pos.z));
	checkKeyBoard(deltaTime);

	this->pSys->update(this->deviceContext,this->device);

	this->texTrans += 0.01f;
	if(texTrans > 1.0f)
	{
		this->texTrans = 0.0f;
	}
	
	return 0;
}




void program::buildCubeMap(D3DXMATRIX &lightViewProj)
{
	ID3D11RenderTargetView* renderTargets[1];
	static float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3DXMATRIX world, lightWVP, view, proj, wvp, translate;
	Camera* currCam;
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->RSSetViewports(1, &cubeMap->getViewport());
	this->shader->SetBool("useblendMap" ,false);
	for(int i = 0; i < 6; ++i)
	{
		//clear
		deviceContext->ClearRenderTargetView(cubeMap->getRT(i),ClearColor);
		deviceContext->ClearDepthStencilView(cubeMap->getStencilView(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0);
		
		//bind
		renderTargets[0] = cubeMap->getRT(i);
		deviceContext->OMSetRenderTargets(1,renderTargets,cubeMap->getStencilView());

		//draw to fill the resourceview
		currCam = cubeMap->getCam(i);
		//get cams view and proj
		D3DXMatrixIdentity(&world);
		static float rotY = 0.0f;

		view = currCam->getView();
		proj = currCam->getProj();
	
		wvp = world * view * proj;
		this->map->getVertexBuffer()->Apply();
		this->map->getIndexBuffer()->Apply();
		this->shader->SetMatrix("WVP" , wvp);
		this->shader->SetMatrix("W", world);

		this->shader->SetFloat("texTrans", texTrans);
		this->shader->SetBool("useBlending", true);

		this->shader->SetResource("blendMap", map->getTexture());
		this->shader->SetResource("diffuseMap1", map->getTerTexture(0));
		this->shader->SetResource("diffuseMap2", map->getTerTexture(1));
		this->shader->SetResource("diffuseMap3", map->getTerTexture(2));

		this->shader->Apply(0);
		this->deviceContext->DrawIndexed(256*256*6,0,0);
		this->pSys->render(deviceContext,wvp);

		D3DXMatrixScaling(&world,0.1f,0.1f,0.1f);
		D3DXMatrixTranslation(&translate,1.0f,150.0f,0.0f);
	
		wvp = (this->objects.at(0).getWorldMatrix()) * view * proj;
	
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		this->shader->SetMatrix("WVP" , wvp);
		this->shader->SetMatrix("W", this->objects.at(0).getWorldMatrix());

		this->shader->SetFloat("texTrans", texTrans);
		this->shader->SetBool("useCubeMap", false);
		this->shader->SetBool("useBlending", false);
		this->shader->SetResource("diffuseMap1", map->getTerTexture(1));
		this->objects.at(0).getVertexBuffer()->Apply();
		this->shader->Apply(0);
		//this->deviceContext->Draw(this->objects.at(0).getNrOfVertices(),0);
	
		wvp = billboardTest->getUpdatedWorldMat(this->cam->getPosition()) * view * proj;

		this->shader->SetMatrix("W",billboardTest->getUpdatedWorldMat(this->cam->getPosition()));
		this->shader->SetMatrix("WVP" , wvp);
		this->shader->SetFloat("texTrans", texTrans);
		this->shader->Apply(0);
		billboardTest->getVertexBuffer()->Apply();
		this->deviceContext->Draw(billboardTest->getNrOfVerts(),0);
		
	}

	this->resetRT();
	this->deviceContext->GenerateMips(cubeMap->getCubemap());
}

void program::buildShadowMap(D3DXMATRIX &lightViewProj)
{
	Shader* shadowShader = this->shadowMap->getShadowShader();
	//matrix initiation and calcs
	D3DXMATRIX world, lightWVP, translate, rotate;
	D3DXMatrixIdentity(&world);

	lightCam->setLens(0.45f*PI,  1024.0f / 768.0f,1,1000);
	lightCam->updateViewMatrix();

	lightViewProj = lightCam->getView() * lightCam->getProj();
	//matrix initiation and calcs

	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shadowMap->outputMerger(this->deviceContext);


	//shadows for terrain
	lightWVP = world * lightViewProj;

	this->map->getVertexBuffer()->Apply();
	this->map->getIndexBuffer()->Apply();

	shadowShader->SetMatrix("LightWVP",lightWVP);
	shadowShader->Apply(0);

	this->deviceContext->DrawIndexed(256*256*6,0,0);
	//shadows for terrain

	//shadows for objects
	for(int i = 0; i < this->objects.size(); i++)
	{
		lightWVP = (this->objects.at(i).getWorldMatrix()) * lightViewProj;
		this->objects.at(i).getVertexBuffer()->Apply();

		shadowShader->SetMatrix("LightWVP", lightWVP);
		shadowShader->Apply(0);

		this->deviceContext->Draw(this->objects.at(i).getNrOfVertices(),0);
	}
	//shadows for objects


	this->resetRT();
}

void program::render(float deltaTime)
{
	Shader* shader;
	D3DXMATRIX world, view, proj, wvp, lightWVP, translate, rotate, lightViewProj;

	cam->setLens(0.45f*PI,  1024.0f / 768.0f,1,1000);
	cam->updateViewMatrix();

	view = cam->getView();
	proj = cam->getProj();

	buildShadowMap(lightViewProj);
	buildCubeMap(world);

	D3D11Handler::clearAndBindTarget();

	//set topology
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	D3DXMatrixIdentity(&world);
	static float rotY = 0.0f;

	//Geometry Pass
	//--------------------------------------------------------
		//rita ut terräng med blendmap och shadowmap på
		shader = this->setPass(0);
		this->map->getVertexBuffer()->Apply();
		this->map->getIndexBuffer()->Apply();
		shader->SetMatrix("world" , world);
		shader->SetMatrix("view", view);
		shader->SetMatrix("proj", proj);
		shader->SetBool("useCubeMap", false);
		shader->SetBool("useBlendMap", true);
		shader->SetBool("useShadowMap", true);

		shader->SetMatrix("lightWVP",lightViewProj * world);
		shader->SetFloat("SMAP_SIZE", 4024);
		shader->SetFloat("texTrans", 0);

		shader->SetFloat4("cameraPos", D3DXVECTOR4(cam->getPosition(),1));
		shader->SetResource("texture1" , map->getTerTexture(0));
		shader->SetResource("texture2" , map->getTerTexture(1));
		shader->SetResource("texture3" , map->getTerTexture(2));
		shader->SetResource("blendMap" , map->getTexture());
		shader->SetResource("shadowMap" , shadowMap->DepthMapSRV());
		shader->Apply(0);
		this->deviceContext->DrawIndexed(256*256*6,0,0);


		//rita ut en sfär med cubemap på
		this->objects[0].getVertexBuffer()->Apply();
		shader->SetBool("useBlendMap", false);
		shader->SetBool("useCubeMap", true);
		shader->SetBool("useShadowMap",false);
		shader->SetFloat("texTrans", 0);
		shader->SetResource("cubeMap",this->cubeMap->getCubemap());
		shader->SetMatrix("world" , this->objects[0].getWorldMatrix());
		shader->Apply(0);
		this->deviceContext->Draw(this->objects[0].getNrOfVertices(),0);

		//rita ut billboard
		this->billboardTest->getVertexBuffer()->Apply();
		shader->SetBool("useBlendMap", false);
		shader->SetBool("useCubeMap", true);
		shader->SetBool("useShadowMap",false);
		shader->SetFloat("texTrans", 0);
		shader->SetResource("cubeMap",this->cubeMap->getCubemap());
		shader->SetMatrix("world" , billboardTest->getUpdatedWorldMat(this->cam->getPosition()));
		shader->Apply(0);
		this->deviceContext->Draw(6,0);
	//---------------------------------------------------------

	//Light  Pass
	//----------------------------------

		//ljusbuffer (vertex och instance)
		//-----------------------------------------------------------------
	//		POINTLIGHTINSTANCE *instance = new POINTLIGHTINSTANCE[2];
	//		instance[0] = POINTLIGHTINSTANCE(D3DXVECTOR3(0,0,0) ,D3DXVECTOR3(1,0,0) , 100f);
	//		instance[1] = POINTLIGHTINSTANCE(D3DXVECTOR3(256,0,256) ,D3DXVECTOR3(0,1,0) , 100f);
	//	
	//		BUFFER_INIT_DESC instanceBufferDesc;
	//		instanceBufferDesc.ElementSize = sizeof(POINTLIGHTINSTANCE);
	//		instanceBufferDesc.InitData = &instance[0];
	//		instanceBufferDesc.NumElements = 2;
	//		instanceBufferDesc.Type = VERTEX_BUFFER;
	//		instanceBufferDesc.Usage = BUFFER_DEFAULT;
 //		
	//		Buffer* instanceBuffer;
	//		instanceBuffer = new Buffer();
	//		instanceBuffer->Init(this->device, this->deviceContext, instanceBufferDesc);

	//		Buffer* vertexBuffer = this->objects[0].getVertexBuffer();

	//		UINT strides[2] = {sizeof(Vertex) , sizeof(POINTLIGHTINSTANCE)};
	//		UINT offset[2] = {0,0};
	//		ID3D11Buffer* buffers[2] = {vertexBuffer , instanceBuffer};

	//		this->deviceContext->IASetVertexBuffers(0,2,buffers, strides, offset);
	//	//------------------------------------------------------------------------------

	//	D3DXMatrix invertViewProj; 
	//	D3DXMatrixInverse(D3DXMatrixMultiply(invertViewProj, view , proj);

	//	shader = this->setPass(1);

	//	shader->SetFloat4("cameraPos" , D3DXVECTOR4(cam->getPosition, 0));
	//	shader->SetResource("diffuseAlbedoMap" , this->SRVs[0]);
	//	shader->SetResource("normalMap" , this->SRVs[1]);
	//	shader->SetResource("depthMap" , this->SRVs[3];
	//	shader->SetMatrix("view", view);
	//	shader->SetMatrix("proj", proj);
	//	shader->SetMatrix("inverViewProjection" , invertViewProj);

	//	shader->Apply(0);
	//	this->deviceContext->DrawInstanced(this->objects[0].getNrOfVertices(), 2, 0 , 0);
	////---------------------------------


	//fullscreen quad
	//-------------------------------------------------------------------------------
		mesh = new Vertex[6];
		mesh[0] = Vertex(D3DXVECTOR3(1,-1,0) ,D3DXVECTOR3(0,0,-1),D3DXVECTOR2(0,0));
		mesh[1] = Vertex(D3DXVECTOR3(-1,-1,0) ,D3DXVECTOR3(0,0,-1),D3DXVECTOR2(1,0));
		mesh[2] = Vertex(D3DXVECTOR3(1,1,0) ,D3DXVECTOR3(0,0,-1),D3DXVECTOR2(0,1));
	
		mesh[3] = Vertex(D3DXVECTOR3(1,1,0) ,D3DXVECTOR3(0,0,-1),D3DXVECTOR2(0,1));
		mesh[4] = Vertex(D3DXVECTOR3(-1,-1,0) ,D3DXVECTOR3(0,0,-1),D3DXVECTOR2(1,0));
		mesh[5] = Vertex(D3DXVECTOR3(-1,1,0) ,D3DXVECTOR3(0,0,-1),D3DXVECTOR2(1,1));
	
		BUFFER_INIT_DESC vertexBufferDesc;
 		vertexBufferDesc.ElementSize = sizeof(Vertex);
		vertexBufferDesc.InitData = &mesh[0];
		vertexBufferDesc.NumElements = 6;
		vertexBufferDesc.Type = VERTEX_BUFFER;
		vertexBufferDesc.Usage = BUFFER_DEFAULT;

		Buffer *vB;
		vB = new Buffer();
		if(FAILED(vB->Init(device, deviceContext, vertexBufferDesc)))
		{
			return;
		}
		SAFE_DELETE(mesh);

		shader = this->setPass(2);
		shader->SetResource("diffuseAlbedoMap", this->SRVs[0]); 
		shader->SetResource("normalMap", this->SRVs[1]);
		shader->SetResource("lightMap", this->SRVs[2]);
		vB->Apply();
		shader->Apply(0);
		this->deviceContext->Draw(6, 0);
	//----------------------------------------------------------------------


		//DepthStencilState
	//------------------------
		 D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		 ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		 depthStencilDesc.DepthEnable = FALSE;
		 depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		 depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		 depthStencilDesc.StencilEnable = FALSE;
		 depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		 depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		 depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		 depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		 depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		 depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		 depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		 depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		 depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		 depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		 ID3D11DepthStencilState* depthStencilState;
		 this->device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	//-------------------------
	this->deviceContext->OMSetRenderTargets(1, &this->backBufferRTV, this->DSV);
	this->deviceContext->OMSetDepthStencilState(depthStencilState, NULL);
	this->pSys->render(deviceContext,wvp);
	this->deviceContext->OMSetDepthStencilState(NULL, NULL);
	//Shadowmap grejer
	/*
	lightWVP = world * lightViewProj;

	this->shader->SetMatrix("LightWVP", lightWVP);

	this->shader->SetFloat("SMAP_SIZE", 4024);
	this->shader->SetFloat("texTrans", texTrans);
	this->shader->SetBool("useCubeMap", false);
	this->shader->SetFloat4("cameraPos",D3DXVECTOR4(cam->getPosition(),0));
	this->shader->SetResource("diffuseMap1", map->getTerTexture(1));
	this->shader->SetResource("shadowMap", this->shadowMap->DepthMapSRV());
	
	/******************************************************************
	this->shader->SetResource("cubeMap", cubeMap->getCubemap());

	this->shader->Apply(0);
	this->deviceContext->DrawIndexed(256*256*6,0,0);
	this->pSys->render(deviceContext,wvp);
	/******************************************************************

	D3DXMatrixScaling(&world,0.1,0.1,0.1);
	D3DXMatrixTranslation(&translate,1,150,0);
	
	wvp = (this->objects.at(0).getWorldMatrix()) * view * proj;
	
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->shader->SetMatrix("WVP" , wvp);
	this->shader->SetMatrix("W", this->objects.at(0).getWorldMatrix());


	lightWVP = (this->objects.at(0).getWorldMatrix()) * lightViewProj;

	this->shader->SetMatrix("LightWVP", lightWVP);
	this->shader->SetFloat("SMAP_SIZE", 4024);
	this->shader->SetFloat("texTrans", texTrans);
	this->shader->SetBool("useCubeMap", true);
	this->shader->SetFloat4("cameraPos",D3DXVECTOR4(cam->getPosition(),0));
	this->shader->SetResource("diffuseMap1", map->getTerTexture(1));
	this->shader->SetResource("shadowMap", this->shadowMap->DepthMapSRV());
	this->shader->SetResource("cubeMap", cubeMap->getCubemap());
	this->objects.at(0).getVertexBuffer()->Apply();
	this->shader->Apply(0);
	this->deviceContext->Draw(this->objects.at(0).getNrOfVertices(),0);
	world = billboardTest->getUpdatedWorldMat(this->cam->getPosition());
	wvp = world * view * proj;

	this->shader->SetMatrix("W",world);
	this->shader->SetMatrix("WVP" , wvp);
	this->shader->SetFloat("texTrans", texTrans);
	this->shader->Apply(0);
	billboardTest->getVertexBuffer()->Apply();
	this->deviceContext->Draw(billboardTest->getNrOfVerts(),0);
	*/
	SAFE_DELETE(vB);

	if(FAILED(D3D11Handler::swapChain->Present( 0, 0 )))
	{
		return;
	}
		
}
void program::mouseOnMove(WPARAM btnState, int x, int y)
{
	this->cam->pitch(this->input->mousePitch(btnState, x, y) * 2);
	this->cam->rotateY(this->input->mouseRotateY(btnState, x, y) * 2);
}

void program::checkKeyBoard(float deltaTime)
{
	deltaTime *= 2;
	if(input->checkKeyDown('W'))
	{
		this->cam->walk(50.0f * deltaTime);
	}

	if(input->checkKeyDown('S'))
	{
		this->cam->walk(-50.0f * deltaTime);
	}

	if(input->checkKeyDown('A'))
	{
		this->cam->strafe(-50.0f * deltaTime);
	}

	if(input->checkKeyDown('D'))
	{
		this->cam->strafe(50.0f * deltaTime);
	}

	if(input->checkKeyDown('P'))
	{
		if(flyMode)
		{
			flyMode = false;
		}
		else
		{
			flyMode = true;
		}
	}
	
	if(input->checkKeyDown('K'))
	{
		cam->getPosition();
	}
}



