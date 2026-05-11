// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "TesselationShader.h"
#include "DepthShader.h"
#include "EdgeDetectionShader.h"



class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();


	void modelPass();
	void depthPass();
	void edgePass();
	

protected:
	void gui();
	bool render();

private:

	float width, height;

	// Render Textures
	RenderTexture* prePP;
	RenderTexture* depthRender;
	RenderTexture* modelRender;
	RenderTexture* edgeRender;

	// Water Mesh
	PlaneMesh* waterPlane;

	// Ortho Mesh
	OrthoMesh* orthoMesh;
	
	// Models
	AModel* mountain;
	
	// Lights
	Light* lights[3];
	Light* dirLight;
	Light* pointLight;
	Light* pointLight2;

	// Attenuation
	Attenuation* att;

	// Shadow Map
	ShadowMap* shadowMap;

	ID3D11ShaderResourceView* shadows;

	// Shaders
	
	TesselationShader* tesselationShader;
	TesselationShader* manipulationShader;
	DepthShader* depthShader;
	EdgeDetectionShader* edgeShader;

	float PosX, PosY, PosZ;
	float dirLightX, dirLightY, dirLightZ;

	XMFLOAT4 nightColour;
	XMFLOAT4 dayColour;
	XMFLOAT4 currentColour;

};

#endif