#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <Resource.h>
#include <ResourceContainer.h>

class MockResource
{
public:
	char mockData[1024];
};

class MockResourceManager
{
public:
	MockResourceManager()
	{
		counter = 0;
	}

	Resource<MockResource> Create()
	{
		MockResource* resource = new MockResource;
		resource->mockData[0] = 'K';
		InternalResource<MockResource>::Iterator internal = container.AddResource(counter, resource);
		counter++;

		return Resource<MockResource>(internal, &container, std::bind(&MockResourceManager::Destructor, this, std::placeholders::_1));
	}

	void Destructor(const InternalResource<MockResource>::Iterator& iterator)
	{
		container.RemoveResource(iterator);
	}

	
	ResourceContainer<MockResource> container;
	int counter;
};

class ResourceManagerTest : public ::testing::Test
{
protected:
	MockResourceManager rm;
};



TEST_F(ResourceManagerTest, ResourceConstructionAssignment)
{
	{
		/*
		Resource<MockResource> a;
		ASSERT_EQ(a, nullptr);
		*/

		Resource<MockResource> b = rm.Create();
		ASSERT_NE(rm.container.resources.find(0), rm.container.resources.end());

		ASSERT_NE(b, nullptr);
		ASSERT_EQ(b.internal->second.refCount, 1);

		Resource<MockResource> c(b);
		ASSERT_EQ(c, b);
		ASSERT_EQ(c.internal->second.refCount, 2);
		ASSERT_EQ(b.internal->second.refCount, 2);

		Resource<MockResource> a = b;
		ASSERT_NE(a, nullptr);
		ASSERT_EQ(a, b);
		ASSERT_EQ(a.internal->second.refCount, 3);

		ASSERT_EQ(a->mockData[0], 'K');
		ASSERT_EQ((*a).mockData[0], 'K');

		{
			Resource<MockResource> e(a);
			ASSERT_EQ(a.internal->second.refCount, 4);
		}

		ASSERT_EQ(a.internal->second.refCount, 3);

		Resource<MockResource> d = rm.Create();
		ASSERT_NE(rm.container.resources.find(1), rm.container.resources.end()); 
		d = a;
		ASSERT_EQ(rm.container.resources.find(1), rm.container.resources.end());
	}
	
	//ASSERT_EQ(rm.container.resources[0].refCount, 0);
	ASSERT_EQ(rm.container.resources.size(), 0);
}