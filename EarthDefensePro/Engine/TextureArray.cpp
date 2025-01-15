#include "TextureArray.h"
#include <comdef.h>


ESG::TextureArray::TextureArray(ID3D11Device* creator,
	std::string* filePath
) {
	HRESULT hr;
	LPCTSTR errorText = NULL;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	//load diffuse, specular, ambient, normal (if the array.length of texturenames is 4)
	int textureArraySize = *(&smrv + 1) - smrv;
	for (int i = 0; i < textureArraySize; i++) {
		const wchar_t* path = widen(filePath[i]);
		if (filePath[i] != "../Textures/") {

			//create DDS from file
			hr = CreateDDSTextureFromFile(
				creator,
				path,
				(ID3D11Resource**)&textures[i],
				&smrv[i],
				0
			);

			//if texture creation failed
			if (FAILED(hr)) {
				//std::wstring ws(filePath[i]);
				_com_error error(hr);
				std::wcout << "Error Failed to create Texture:\n" << error.ErrorMessage() << '\n';
				errorText = NULL;
				//printf("ERROR", (std::string("Failed to create DDS Texture from file path: ") + std::string(ws.begin(), ws.end())).c_str());
				continue;
			}

			textures[i]->GetDesc(&textureDesc);
			viewDesc.Format = textureDesc.Format;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
			viewDesc.Texture2D.MostDetailedMip = 0;

			//create shader resource view
			hr = creator->CreateShaderResourceView(
				(ID3D11Resource*)textures[i],
				&viewDesc,
				&smrv[i]
			);

			//if shader view creation failed
			if (FAILED(hr)) {
				_com_error error(hr);
				std::wcout << "Error Failed to create shader view:\n" << error.ErrorMessage() << '\n';
				errorText = NULL;
				continue;
			}
		}
	}
}

