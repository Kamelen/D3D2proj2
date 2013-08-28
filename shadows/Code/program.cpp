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

	SAFE_DELETE(this->fullScreenQuad);
	SAFE_RELEASE(this->instancedBuffers[0]);
	SAFE_RELEASE(this->instancedBuffers[1]);
	SAFE_DELETE(instanceBuffer);
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
	map = new Terrain("cloudy.Raw",256,256,1,-30,this->device,this->deviceContext,textureNames,"terrainblend.png");
	
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

	pSys->addFire(this->deviceContext,this->device,D3DXVECTOR3(0,100,0),D3DXVECTOR3(1,0,0),10,5,1);
	pSys->addCloud(this->deviceContext,this->device,D3DXVECTOR3(0,100,0),100,10,5,50,50,50);
	pSys->addRain(this->deviceContext,this->device,D3DXVECTOR3(0,150,0),D3DXVECTOR3(0,-1,0),100,100,1,5000,100,1);
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

	this->objects.push_back(D3DObject(objReader->getOBJfromFile("coolaModellerFixed/sphere2.obj", this->nrOfVerts), this->nrOfVerts, (world * translate)));
	this->objects.push_back(D3DObject(objReader->getOBJfromFile("coolaModellerFixed/sphere.obj", this->nrOfVerts), this->nrOfVerts, (world * translate)));
	
	//this->objects.push_back(D3DObject(objReader->getOBJfromFile("C:\sphere2.obj", this->nrOfVerts), this->nrOfVerts, (world * translate)));
	
	for(int i = 0; i < this->objects.size(); i++)
	{
		this->objects.at(i).initBuffer(this->device,this->deviceContext);
	}
	
	POINTLIGHTINSTANCE *instance = new POINTLIGHTINSTANCE[10];
	instance[0] = POINTLIGHTINSTANCE(D3DXVECTOR3(-400,60,-400) ,D3DXVECTOR3(1,0,0) , 300.0f);
	instance[1] = POINTLIGHTINSTANCE(D3DXVECTOR3(0,40,100) ,D3DXVECTOR3(0,1,0) , 50.0f);
	instance[2] = POINTLIGHTINSTANCE(D3DXVECTOR3(100,60,0) ,D3DXVECTOR3(1,1,1) , 300.0f);
	instance[3] = POINTLIGHTINSTANCE(D3DXVECTOR3(100,40,100) ,D3DXVECTOR3(0,1,0) , 50.0f);
	instance[4] = POINTLIGHTINSTANCE(D3DXVECTOR3(400,60,0) ,D3DXVECTOR3(1,0,0) , 50.0f);
	instance[5] = POINTLIGHTINSTANCE(D3DXVECTOR3(100,40,400) ,D3DXVECTOR3(0,1,0) , 50.0f);
	instance[6] = POINTLIGHTINSTANCE(D3DXVECTOR3(150,60,300) ,D3DXVECTOR3(1,0,0) , 50.0f);
	instance[7] = POINTLIGHTINSTANCE(D3DXVECTOR3(400,40,-400) ,D3DXVECTOR3(0,1,0) , 300.0f);
	instance[8] = POINTLIGHTINSTANCE(D3DXVECTOR3(-400,60,400) ,D3DXVECTOR3(0,0,1) , 300.0f);
	instance[9] = POINTLIGHTINSTANCE(D3DXVECTOR3(400,30,400) ,D3DXVECTOR3(1,1,0) , 300.0f);
	
	BUFFER_INIT_DESC instanceBufferDesc;
	instanceBufferDesc.ElementSize = sizeof(POINTLIGHTINSTANCE);
	instanceBufferDesc.InitData = &instance[0];
	instanceBufferDesc.NumElements = 10;
	instanceBufferDesc.Type = VERTEX_BUFFER;
	instanceBufferDesc.Usage = BUFFER_DEFAULT;
   
	instanceBuffer = new Buffer();
	instanceBuffer->Init(this->device, this->deviceContext, instanceBufferDesc);

	Buffer* vertexBuffer = this->objects[0].getVertexBuffer();

	this->strides[0] = sizeof(Vertex); 
	this->strides[1] = sizeof(POINTLIGHTINSTANCE);
 
	this->offset[0] = 0;
	this->offset[1] = 0;

	instancedBuffers[0] = vertexBuffer->GetBufferPointer();
	instancedBuffers[1] = instanceBuffer->GetBufferPointer();
 
	
	SAFE_DELETE(instance);


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

	this->fullScreenQuad = new Buffer();
	if(FAILED(this->fullScreenQuad->Init(device, deviceContext, vertexBufferDesc)))
	{
	return false;
	}
 
	delete mesh;

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

		D3DXMatrixScaling(&world,0.1f,0.1f,0.1f);
		D3DXMatrixTranslation(&translate,1.0f,150.0f,0.0f);
	
		wvp = (this->objects.at(0).getWorldMatrix()) * view * proj;
	
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		this->shader->SetMatrix("WVP" , wvp);
		this->shader->SetMatrix("W", this->objects.at(1).getWorldMatrix());

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
	static ID3D11ShaderResourceView* const nullPointer[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
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
		this->objects[1].getVertexBuffer()->Apply();
		shader->SetBool("useBlendMap", false);

		shader->SetBool("useCubeMap", true);
		shader->SetBool("useShadowMap",false);
		shader->SetFloat("texTrans", 0);
		shader->SetResource("cubeMap",this->cubeMap->getCubemap());
		shader->SetMatrix("world" , this->objects[1].getWorldMatrix());
		shader->Apply(0);
		this->deviceContext->Draw(this->objects[1].getNrOfVertices(),0);

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

		this->pSys->render(deviceContext, shader);
		

	//---------------------------------------------------------

	//Light  Pass
	//----------------------------------

		//ljusbuffer (vertex och instance)
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		this->deviceContext->IASetVertexBuffers(0,2, this->instancedBuffers, this->strides, this->offset);
		//-----------------------------------------------------------------

		//------------------------------------------------------------------------------

		D3DXMATRIX invertViewProj, viewProj; 
		viewProj = view * proj;
		D3DXMatrixInverse(&invertViewProj , NULL , &viewProj);
		shader = this->setPass(1);
		D3DXVECTOR4 pos = D3DXVECTOR4(cam->getPosition(), 1);
		shader->SetFloat4("cameraPos" , pos);
		deviceContext->PSSetShaderResources(1, 1, &SRVs[0]);
		//shader->SetResource("diffuseAlbedoMap" , this->SRVs[0]);
		shader->SetResource("normalMap" , this->SRVs[1]);
		shader->SetResource("depthMap" , this->SRVs[3]);
		shader->SetMatrix("view", view);
		shader->SetMatrix("proj", proj);
		shader->SetMatrix("invertViewProjection" , invertViewProj);

		shader->Apply(0);
		this->deviceContext->OMSetBlendState(blendState, 0, 0xffffffff);
		this->deviceContext->DrawInstanced(this->objects[0].getNrOfVertices(), 10, 0 , 0);
		this->deviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	//---------------------------------
	//fullscreen quad
	//-------------------------------------------------------------------------------
		
		shader = this->setPass(2);
		shader->SetResource("diffuseAlbedoMap", this->SRVs[0]); 
		shader->SetResource("normalMap", this->SRVs[1]);
		shader->SetResource("lightMap", this->SRVs[2]);
		this->fullScreenQuad->Apply();
		shader->Apply(0);
		this->deviceContext->Draw(6, 0);

		
	//----------------------------------------------------------------------

	
	if(FAILED(D3D11Handler::swapChain->Present( 0, 0 )))
	{
		return;
	}
	deviceContext->PSSetShaderResources(0, 8, nullPointer);
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
	
}



