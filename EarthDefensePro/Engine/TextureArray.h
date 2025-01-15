#ifndef TEXTUREARRAY
#define TEXTUREARRAY

#include <DDSTextureLoader.h>

namespace ESG {
	class TextureArray {
	public:
		ID3D11ShaderResourceView* smrv[4] = { 0 };
		ID3D11Texture2D* textures[4] = { 0 };

		TextureArray(ID3D11Device* creator,
			std::string*
		);
	};
}

#endif
