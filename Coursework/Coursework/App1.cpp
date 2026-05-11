// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"


App1::App1()
{
	
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.

	width = float(screenWidth);
	height = float(screenHeight);

	// Render Textures
	prePP = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	modelRender = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	depthRender = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	edgeRender = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Meshes
	waterPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	// Models
	mountain = new AModel(renderer->getDevice(), "res/MountainV1LowerPoly.obj");
	
	// Texture Manager
	textureMgr->loadTexture(L"Mountain", L"res/MountainUV.png");
	textureMgr->loadTexture(L"Brick", L"res/Default/brick1.dds");
	textureMgr->loadTexture(L"Water", L"res/waterTexture.jpg");

	// Shaders
	
	
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	tesselationShader = new TesselationShader(renderer->getDevice(), hwnd, L"tesselation_hs.cso", L"tesselation_ds.cso");
	manipulationShader = new TesselationShader(renderer->getDevice(), hwnd, L"VertexManipulation_hs.cso", L"VertexManipulation_ds.cso");
	edgeShader = new EdgeDetectionShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// This is your shadow map
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	// Lights

	// Directional Light
	dirLight = new Light();
	//dirLight->setAmbientColour(0.992f, 0.984f, 0.827f, 1.f); // rgb values for sunlight colour
	dirLight->setDiffuseColour(1.f, 1.f, 1.f, 1.f);
	dirLight->setDirection(0.2f, -0.05f, 0.2f);
	dirLight->setPosition(-10.f, 30.f, -10.f);
	dirLight->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	// Direction for the directional light
	dirLightX = dirLight->getDirection().x;
	dirLightY = dirLight->getDirection().y;
	dirLightZ = dirLight->getDirection().z;

	// Point Lights
	pointLight = new Light();
	pointLight->setAmbientColour(0.f, 0.f, 0.f, 1.f);
	pointLight->setDiffuseColour(1.f, 0.f, 0.f, 1.f);
	pointLight->setPosition(10.f, 3.f, 5.f);

	//Position for the first light
	PosX = pointLight->getPosition().x;
	PosY = pointLight->getPosition().y;
	PosZ = pointLight->getPosition().z;

	pointLight2 = new Light();
	pointLight2->setAmbientColour(0.f, 0.f, 0.f, 1.f);
	pointLight2->setDiffuseColour(0.f, 1.f, 0.f, 1.f);
	pointLight2->setPosition(-7.f, 3.f, 0.f);
	
	// Lights Array
	lights[0] = dirLight;
	lights[1] = pointLight;
	lights[2] = pointLight2;
	
	// Attenuation
	att = new Attenuation();
	att->setConstantAtt(0.5f);
	att->setLinearAtt(0.125f);
	att->setQuadraticAtt(0.f);

	// Initializing the colours for the day/night cycle
	dayColour = XMFLOAT4(0.278f, 0.494f, 1.f, 1.0f);
	nightColour = XMFLOAT4(0.f, 0.054f, 0.341f, 1.0f);
	currentColour = dayColour;
	
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}


// First pass to calculate depth for use in the shadows
void App1::depthPass()
{
	depthRender->setRenderTarget(renderer->getDeviceContext());
	depthRender->clearRenderTarget(renderer->getDeviceContext(), nightColour.x, nightColour.y, nightColour.z, 1.f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	lights[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = lights[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lights[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	
	// Render Mountain
	mountain->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mountain->getIndexCount());

	
	// Render Water Plane
	XMMATRIX worldMatrix2 = worldMatrix;
	worldMatrix2 *= XMMatrixTranslation(-50.f, 0.2f, -25.f);

	waterPlane->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix2, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());
}

// render the model and water plane, as well as do tesselation
void App1::modelPass()
{
	/*
	
	ADDED
	
	*/
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	modelRender->setRenderTarget(renderer->getDeviceContext());
	modelRender->clearRenderTarget(renderer->getDeviceContext(), nightColour.x, nightColour.y, nightColour.z, 1.f);

/////////////

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render Model and send it to the tesselation shaderes
	mountain->sendData(renderer->getDeviceContext());
	tesselationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"Mountain"), depthRender->getShaderResourceView(), lights, att, width, height);
	tesselationShader->render(renderer->getDeviceContext(), mountain->getIndexCount());



	/*
		Model Coordinate 
		-20 < X < 20
		-20 < Z < 20
		0 < Y < 3

	*/

	// Translate to add the water mesh in the correct position
	XMMATRIX worldMatrix2 = worldMatrix;
	worldMatrix2 *= XMMatrixTranslation(-50.f, 0.2f, -25.f);

	// Render Water Plane

	// Set the time variable to send to the vertex manipulation
	float timeStamp = timer->getTime();

	// Rendering
	waterPlane->sendData(renderer->getDeviceContext());
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix2, viewMatrix, projectionMatrix, textureMgr->getTexture(L"Water"), depthRender->getShaderResourceView(), lights, att, timeStamp, width, height);
	manipulationShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());
	
}

// Render edge detection
void App1::edgePass()
{

	/*
	
		ADDED
	
	*/
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
	

	renderer->setZBuffer(true);

	orthoMesh->sendData(renderer->getDeviceContext());
	edgeShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), camera->getOrthoViewMatrix(), renderer->getOrthoMatrix(), modelRender->getShaderResourceView(), width, height);
	edgeShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());


	////////////////////////////////////////
	
	//camera->update();

	////renderer->setZBuffer(false);

	//XMMATRIX worldMatrix = renderer->getWorldMatrix();
	//XMMATRIX viewMatrix = camera->getViewMatrix();
	//XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//worldMatrix = XMMatrixTranslation(-50.f, 0.2f, -25.f);

	//mountain->sendData(renderer->getDeviceContext());
	//edgeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"Mountain"), width, height);
	//edgeShader->render(renderer->getDeviceContext(), mountain->getIndexCount());

}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::SliderFloat("Directional Light DirX: ", &dirLightX, 0.f, 1.f);
	ImGui::SliderFloat("Directional Light DirY: ", &dirLightY, -1.f, 1.f);
	ImGui::SliderFloat("Directional Light DirZ: ", &dirLightZ, 0.f, 1.f);

	ImGui::SliderFloat("Point Light X", &PosX, 0, 50);
	ImGui::SliderFloat("Point Light Y", &PosY, 0, 3);
	ImGui::SliderFloat("Point Light Z", &PosZ, 0, 50);
	
	
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool App1::render()
{
	renderer->beginScene(currentColour.x, currentColour.y, currentColour.z, dayColour.w);

	
	depthPass();

	
	ID3D11ShaderResourceView* nullSRV[3] = { nullptr };
	renderer->getDeviceContext()->PSSetShaderResources(0, 3, nullSRV); // Unbind prePP as an SRV
	prePP->setRenderTarget(renderer->getDeviceContext());
	prePP->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Clear the scene. (default blue colour)
	

	
	modelPass();

	edgePass();

	//GUI
	gui();
	lights[0]->setDirection(dirLightX, dirLightY, dirLightZ);
	lights[1]->setPosition(PosX, PosY, PosZ);

	renderer->endScene();


	return true;
}

