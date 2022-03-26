#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cassert>

template <typename Resource, typename Identifier>
class ResourceHolder
{
public:
	void Load(Identifier id, const std::string& filename);
	template<typename Parameter>
	void Load(Identifier id, const std::string& filename, const Parameter& secondParam);
	Resource& Get(Identifier id);
	const Resource& Get(Identifier id) const;

private:
	void InsertResource(Identifier id, std::unique_ptr<Resource> resource);

private:
	std::map<Identifier, std::unique_ptr<Resource>> m_resource_map;
};
#include "ResourceHolder.inl"
