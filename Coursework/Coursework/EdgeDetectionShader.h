#ifndef _EDGEDETECTIONSHADER_H
#define _EDGEDETECTIONSHADER_H

#include "DXF.h"

class EdgeDetectionShader : public BaseShader
{
	struct MatrixBufferType
	{
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};

	struct ScreenSizeBufferType
	{
		float width;
		float height;
		XMFLOAT2 padding;
	};

public:
	EdgeDetectionShader(ID3D11Device* device, HWND hwnd);
	~EdgeDetectionShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, float width, float height);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFileName);

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* screenSizeBuffer;

	ID3D11SamplerState* sampleState;

};

#endif