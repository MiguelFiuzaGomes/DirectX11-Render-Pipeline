#include "Attenuation.h"

void Attenuation::setConstantAtt(float constAtt)
{
	this->constantAtt = constAtt;

}

float Attenuation::getConstantAtt()
{
	return this->constantAtt;
}

void Attenuation::setLinearAtt(float linearAtt)
{
	this->linearAtt = linearAtt;
}

float Attenuation::getLinearAtt()
{
	return this->linearAtt;
}

void Attenuation::setQuadraticAtt(float quadAtt)
{
	this->quadraticAtt = quadAtt;
}

float Attenuation::getQuadraticAtt()
{
	return this->quadraticAtt;
}