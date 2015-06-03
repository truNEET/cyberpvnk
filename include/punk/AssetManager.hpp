#ifndef PK_ASSET_MANAGER
#define PK_ASSET_MANAGER

#include <string>

namespace pk {

class AssetManager
{
public:
	AssetManager();
	void addTexture(const std::string &key, const std::string &path);
};

}

#endif
