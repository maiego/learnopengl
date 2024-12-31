#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create()
{
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();
    return std::move(vertexLayout);
}

VertexLayout::~VertexLayout()
{
    if (m_vertexArrayObject != 0)
        glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void VertexLayout::Bind() const
{
    glBindVertexArray(m_vertexArrayObject);
}

void VertexLayout::SetAttrib(
    uint32_t attribIndex, int count,
    uint32_t type, bool normalized,
    size_t stride, uint64_t offset) const
{
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(attribIndex, count, type, normalized, stride, (void*)offset);
}

void VertexLayout::Init()
{
    glGenVertexArrays(1, &m_vertexArrayObject);
    Bind();
}