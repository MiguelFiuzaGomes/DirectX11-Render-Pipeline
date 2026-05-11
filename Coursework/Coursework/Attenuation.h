#pragma once
class Attenuation
{
private:
	float constantAtt;
	float linearAtt;
	float quadraticAtt;

public:
	void setConstantAtt(float constAtt);
	float getConstantAtt();
	void setLinearAtt(float linearAtt);
	float getLinearAtt();
	void setQuadraticAtt(float quadAtt);
	float getQuadraticAtt();
};