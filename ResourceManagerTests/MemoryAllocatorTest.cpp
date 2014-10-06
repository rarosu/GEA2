#include <gtest/gtest.h>
#include <MemoryAllocator.h>

class MemoryAllocatorTest : public ::testing::Test
{
public:
	MemoryAllocatorTest()
		: allocator() {}
protected:
	std::stringstream ss;
	MemoryAllocator allocator;
};

TEST_F(MemoryAllocatorTest, Allocation)
{
	MemoryAllocatorInterface interface = allocator.GetInterface("TestAllocator");
	
	size_t datasize = 128;
	char* data = (char*) interface.Alloc(datasize);
	
	// Assert that the pointer is non-null.
	ASSERT_NE(data, nullptr);

	// Assert that all data places can be accessed.
	for (size_t i = 0; i < datasize; ++i)
	{
		data[i] = 'A';
		ASSERT_EQ(data[i], 'A');
	}

	interface.Free(data);
}