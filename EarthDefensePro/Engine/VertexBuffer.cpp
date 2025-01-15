#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::Initialize(ID3D11Device* device, const void* data, unsigned int sizeInBytes)
{
    D3D11_SUBRESOURCE_DATA bData = { data, 0, 0 };
    CD3D11_BUFFER_DESC bDesc(sizeInBytes, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    HRESULT hr = device->CreateBuffer(&bDesc, &bData, vertexBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

void VertexBuffer::Bind(ID3D11DeviceContext* context, UINT stride, UINT offset)
{
    const UINT offsets[] = { offset };
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, offsets);
}
