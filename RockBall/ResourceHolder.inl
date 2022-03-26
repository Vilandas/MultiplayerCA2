template<typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename)
{
	//Create and load resource
	std::unique_ptr<Resource> resource(new Resource());
	if(!resource->loadFromFile(filename))
	{
		throw std::runtime_error("ResouceHolder::load - Failed to load " + filename);
	}
	//If loading successful insert resource into map
	InsertResource(id, std::move(resource));
}

template<typename Resource, typename Identifier>
template<typename Parameter>
void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename, const Parameter& second_parameter)
{
	//Create and load resource
	std::unique_ptr<Resource> resource(new Resource());
	if (!resource->loadFromFile(filename, second_parameter))
	{
		throw std::runtime_error("ResouceHolder::load - Failed to load " + filename);
	}
	//If loading successful insert resource into map
	InsertResource(id, std::move(resource));
}

template<typename Resource, typename Identifier>
Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id)
{
	auto found = m_resource_map.find(id);
	assert(found != m_resource_map.end());
	return *found->second;
}

template<typename Resource, typename Identifier>
const Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id) const
{
	auto found = m_resource_map.find(id);
	assert(found != m_resource_map.end());
	return *found->second;
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::InsertResource(Identifier id, std::unique_ptr<Resource> resource)
{
	//Insert and check success
	auto inserted = m_resource_map.insert(std::make_pair(id, std::move(resource)));
	assert(inserted.second);
}

