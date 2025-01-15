#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class InstanceBuffer
{
public:
    InstanceBuffer();
    ~InstanceBuffer();

    bool Initialize(ID3D11Device* device, const void* data, unsigned int sizeInBytes);
    void Bind(ID3D11DeviceContext* context, UINT stride, UINT offset);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;
};
