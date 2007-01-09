#include "WFStepObject.h"

using namespace WFEngine::nmWFStepObject;

WFStepObject::WFStepObject()
{
}

WFStepObject::~WFStepObject()
{
}

WFStepObject *WFStepObject::New()
{
    return new WFStepObject();
}
