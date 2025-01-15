#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer()
{
}

InstanceBuffer::~InstanceBuffer()
{
}

bool InstanceBuffer::Initialize(ID3D11Device* device, const void* data, unsigned int sizeInBytes)
{
    D3D11_SUBRESOURCE_DATA bData = { data, 0, 0 };
    CD3D11_BUFFER_DESC bDesc(sizeInBytes, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    HRESULT hr = device->CreateBuffer(&bDesc, &bData, instanceBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

void InstanceBuffer::Bind(ID3D11DeviceContext* context, UINT stride, UINT offset)
{
    const UINT offsets[] = { offset };
    context->IASetVertexBuffers(1, 1, instanceBuffer.GetAddressOf(), &stride, offsets);
}
