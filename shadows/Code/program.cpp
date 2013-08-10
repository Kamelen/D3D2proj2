#include "Program.h"


program::program(void)
{
	this->apply = 0;
	this->switchTime = 0.0f;
}


program::~program(void)
{
	SAFE_DELETE(map);
	SAFE_DELETE(cam);
	SAFE_DELETE(input);
	SAFE_DELETE(pSys);
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
	string textureNames[] = {"Fire.png", "texture02.dds","texture03.dds"};
	map = new Terrain("cloudy.Raw",256,256,0.5,0,this->device,this->deviceContext,textureNames,"terrainblend.png");
	
	D3DXVECTOR3 position = map->getTerrainPos(254,254);
	D3DXVECTOR3 look = D3DXVECTOR3(0,0,1);
	D3DXVECTOR3 right = D3DXVECTOR3(1,0,0);
	D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);

	cam = new Camera(position, look, right, up);

	position = D3DXVECTOR3(1, 180, 0);
	look = D3DXVECTOR3(0,-1,0);
	right = D3DXVECTOR3(0,0,1);
	up = D3DXVECTOR3(1,0,0);

	lightCam = new Camera(position, look, right, up);
	
	
	input = new Input();
	pSys = new ParticleSystem();

	shadowMap = new ShadowMap(this->device, this->deviceContext,4024, 4024);



	// -------------------------OBJREADER TEST ---------------------------------
	objReader = new OBJReader();												//
	mesh = objReader->getOBJfromFile("coolaModellerFixed/alduin.obj", this->nrOfVerts);


	D3DXMATRIX world, translate;
	D3DXMatrixScaling(&world,0.1,0.1,0.1);
	D3DXMatrixTranslation(&translate,1,90,0);

	this->objects.push_back(D3DObject(objReader->getOBJfromFile("coolaModellerFixed/alduin.obj", this->nrOfVerts), this->nrOfVerts, (world * translate)));
	
	for(int i = 0; i < this->objects.size(); i++)
	{
		this->objects.at(i).initBuffer(this->device,this->deviceContext);
	}
	
	
	BUFFER_INIT_DESC vertexBufferDesc;											//
 	vertexBufferDesc.ElementSize = sizeof(Vertex);								//
	vertexBufferDesc.InitData = mesh;											//
	vertexBufferDesc.NumElements = this->nrOfVerts;								//
	vertexBufferDesc.Type = VERTEX_BUFFER;										//
	vertexBufferDesc.Usage = BUFFER_DEFAULT;									//
																				//
	this->vBuffer = new Buffer();												//
	if(FAILED(this->vBuffer->Init(device, deviceContext, vertexBufferDesc)))	//
	{																			//
		return false;															//
	}																			//
	//--------------------------OBJREADER TEST ----------------------------------


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
	//pos.y = this->map->getY(pos.x,pos.z);
	this->cam->setPos(D3DXVECTOR3((float)pos.x,(float)pos.y,(float)pos.z));

	checkKeyBoard(deltaTime);

	this->pSys->update(this->deviceContext,this->device);

	return 0;
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
	/*lightWVP = world * lightViewProj;

	this->map->getVertexBuffer()->Apply();
	this->map->getIndexBuffer()->Apply();

	shadowShader->SetMatrix("LightWVP",lightWVP);
	shadowShader->Apply(0);

	this->deviceContext->DrawIndexed(256*256*6,0,0);*/
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
	D3DXMATRIX world, view, proj, wvp, lightWVP, translate, rotate, lightViewProj;

	cam->setLens(0.45f*PI,  1024.0f / 768.0f,1,1000);
	cam->updateViewMatrix();

	view = cam->getView();
	proj = cam->getProj();

	buildShadowMap(lightViewProj);

	static float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	D3D11Handler::deviceContext->ClearRenderTargetView( renderTargetView, ClearColor );
	
    //clear depth info
	this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	//set topology
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//calculate WVP matrix

	
	D3DXMatrixIdentity(&world);
	static float rotY = 0.0f;

	
	wvp = world * view * proj;
	this->map->getVertexBuffer()->Apply();
	this->map->getIndexBuffer()->Apply();
	this->shader->SetMatrix("WVP" , wvp);
	this->shader->SetMatrix("W", world);

	lightWVP = world * lightViewProj;

	this->shader->SetMatrix("LightWVP", lightWVP);

	this->shader->SetFloat("SMAP_SIZE", 4024);
	this->shader->SetResource("diffuseMap", map->getTerTexture(0));
	this->shader->SetResource("shadowMap", this->shadowMap->DepthMapSRV());

	this->shader->Apply(0);
	this->deviceContext->DrawIndexed(256*256*6,0,0);
	this->pSys->render(deviceContext,wvp);

	D3DXMatrixScaling(&world,0.1,0.1,0.1);
	D3DXMatrixTranslation(&translate,1,150,0);
	
	wvp = (this->objects.at(0).getWorldMatrix()) * view * proj;
	
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->shader->SetMatrix("WVP" , wvp);
	this->shader->SetMatrix("W", world);


	lightWVP = (this->objects.at(0).getWorldMatrix()) * lightViewProj;

	this->shader->SetMatrix("LightWVP", lightWVP);
	this->shader->SetFloat("SMAP_SIZE", 4024);
	this->shader->SetResource("diffuseMap", map->getTerTexture(0));
	this->shader->SetResource("shadowMap", this->shadowMap->DepthMapSRV());

	this->objects.at(0).getVertexBuffer()->Apply();
	this->shader->Apply(0);
	this->deviceContext->Draw(this->objects.at(0).getNrOfVertices(),0);


	


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
}



