#include "BaseObject.h"


BaseObject::BaseObject()
{
	m_no = "";
	m_property_flag = 0;
}


BaseObject::~BaseObject()
{
}


void BaseObject::SetNo(string no)
{
	this->m_no = no;
}
void BaseObject::SetPropertyFlag(int property_flag)
{
	this->m_property_flag = property_flag;
}
string BaseObject::GetNo()
{
	return this->m_no;
}
int BaseObject::GetPropertyFlag()
{
	return this->m_property_flag;
}

void BaseObject::SetRegisterClass(string register_class)
{
	this->m_register_class = register_class;
}
string BaseObject::GetRegisterClass()
{
	return this->m_register_class;
}

void BaseObject::SetObjName(string obj_name)
{
	this->m_obj_name = obj_name;
}
string BaseObject::GetObjName()
{
	return this->m_obj_name;
}
