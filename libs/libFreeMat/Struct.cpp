#include "Array.hpp"
#include "Interpreter.hpp"
#include "Struct.hpp"

SARefPtr::SARefPtr(StructArray* p, Interpreter *eval) : 
  m_ptr(p), m_eval(eval) 
{
  if (p) p->ref();
}

SARefPtr::~SARefPtr() 
{
  if (m_ptr) 
    {
      m_ptr->deref();
      if (m_ptr->getRefCount() == 0)
	m_eval->deleteHandleClass(m_ptr);
    }
}

SARefPtr::SARefPtr(const SARefPtr& copy) : m_ptr(copy.m_ptr),
					   m_eval(copy.m_eval) 
{
  if (m_ptr) m_ptr->ref();
}

StructArray& SARefPtr::operator*() 
{
  return *m_ptr;
}

StructArray* SARefPtr::operator->() 
{
  return m_ptr;
}

const StructArray& SARefPtr::operator*() const 
{
  return *m_ptr;
}

const StructArray* SARefPtr::operator->() const 
{
  return m_ptr;
}

void StructArray::ref()
{
  m_refCount++;
}

void StructArray::deref()
{
  m_refCount--;
}
