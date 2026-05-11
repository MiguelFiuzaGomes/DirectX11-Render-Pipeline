// Light shader.h
// Basic single light shader setup
#ifndef _TESSELATIONSHADER_H_
#define _TESSELEATIONSHADER_H_

#include "DXF.h"
#include "Attenuation.h"

using namespace std;
using namespace DirectX;


class TesselationShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[3];
		XMFLOAT4 diffuse[3];
		XMFLOAT3 direction[3];
		XMFLOAT3 position[3];
		XMFLOAT2 padding;
	};

	// Attenuation for the Point Lights
	struct AttenuationBufferType
	{
		float constantAtt;
		float linearAtt;
		float quadraticAtt;
		float padding;
	};

	struct TimeBufferType
	{
		float time;
		XMFLOAT3 padding;
	};

	struct ScreenSizeBufferType
	{
		float width;
		float height;
		XMFLOAT2 padding;
	};

public:

	TesselationShader(ID3D11Device* device, HWND hwnd, const wchar_t* hsFileName, const wchar_t* dsFileName);
	~TesselationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, Light* lights[], Attenuation* att, float w, float h);
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, Light* lights[], Attenuation* att, float timer, float w, float h);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* attenuationBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* screenSizeBuffer;


	float currentTime = 0.f;
};

#endif