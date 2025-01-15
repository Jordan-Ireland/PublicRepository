#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();

    bool Initialize(ID3D11Device* device, const void* data, unsigned int sizeInBytes);
    void Bind(ID3D11DeviceContext* context, DXGI_FORMAT format, UINT offset);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};
