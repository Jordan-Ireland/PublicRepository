#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::Initialize(ID3D11Device* device, const void* data, unsigned int sizeInBytes)
{
    D3D11_SUBRESOURCE_DATA bData = { data, 0, 0 };
    CD3D11_BUFFER_DESC bDesc(sizeInBytes, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    HRESULT hr = device->CreateBuffer(&bDesc, &bData, indexBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

void IndexBuffer::Bind(ID3D11DeviceContext* context, DXGI_FORMAT format, UINT offset)
{
    context->IASetIndexBuffer(indexBuffer.Get(), format, offset);
}
